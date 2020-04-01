#include "oylpch.h"
#include "AudioPlayer.h"

#include <fmod/fmod.hpp>
#include <fmod/fmod_studio.hpp>
#include <fmod/fmod_errors.h>

static bool _fmod_call(FMOD_RESULT a_result)
{
    if (a_result != FMOD_OK)
    {
        OYL_LOG_ERROR("FMOD ERROR: {}", FMOD_ErrorString(a_result));
        return false;
    }

    // All good
    return true;
}

#if !defined OYL_DISTRIBUTION
    #define FMOD_CALL(call) OYL_ASSERT(_fmod_call(call))
#else
    #define FMOD_CALL(call)
#endif

static float dbToVolume(float db)
{
    return powf(10.0f, 0.05f * db);
}

static float volumeTodb(float volume)
{
    return 20.0f * log10f(volume);
}

namespace oyl
{
    namespace internal
    {
        FMOD::Studio::System* AudioPlayer::s_studioSystem = nullptr;
        FMOD::System*         AudioPlayer::s_system       = nullptr;

        int AudioPlayer::s_nextChannelID = 0;

        AudioPlayer::SoundMap   AudioPlayer::s_sounds;
        AudioPlayer::ChannelMap AudioPlayer::s_channels;
        AudioPlayer::EventMap   AudioPlayer::s_events;
        AudioPlayer::GUIDMap    AudioPlayer::s_guids;
        AudioPlayer::BankMap    AudioPlayer::s_banks;

        void AudioPlayer::init()
        {
            OYL_ASSERT(!s_system && !s_studioSystem);
            
            FMOD_CALL(FMOD::Studio::System::create(&s_studioSystem));
            FMOD_CALL(s_studioSystem->initialize(32, FMOD_STUDIO_INIT_LIVEUPDATE, FMOD_INIT_PROFILE_ENABLE, NULL));

            FMOD_CALL(s_studioSystem->getCoreSystem(&s_system));

            loadGUIDs();
        }

        void AudioPlayer::shutdown()
        {
            FMOD_CALL(s_studioSystem->unloadAll());
            FMOD_CALL(s_studioSystem->release());

            s_system = nullptr;
            s_studioSystem = nullptr;
        }

        void AudioPlayer::update()
        {
            for (auto it = s_channels.begin(); it != s_channels.end();)
            {
                bool isPlaying = false;
                FMOD_CALL(it->second->isPlaying(&isPlaying));
                if (!isPlaying)
                    it = s_channels.erase(it);
                else
                    ++it;
            }

            FMOD_CALL(s_studioSystem->update());
        }

        void AudioPlayer::loadGUIDs()
        {
            std::ifstream guidFile("res/assets/sounds/GUIDs.txt");

            if (!guidFile)
            {
                OYL_LOG_ERROR("FMOD Error: Could not open res/sounds/GUIDs.txt!");
                return;
            }

            std::string currentLine;

            while (!guidFile.eof())
            {
                getline(guidFile, currentLine);

                auto guid = currentLine.substr(0, currentLine.find(' '));

                auto key = currentLine.substr(currentLine.find(' ') + 1, currentLine.length() - guid.length() - 1);

                s_guids[key] = guid;
            }

            guidFile.close();
        }
    }

    using internal::AudioPlayer;

    void Audio::loadBank(const std::string& a_bankName)
    {
        auto& banks = AudioPlayer::s_banks;

        auto foundIt = banks.find(a_bankName);
        if (foundIt != banks.end())
            return;

        FMOD::Studio::Bank* bank;
        FMOD_CALL(AudioPlayer::s_studioSystem->loadBankFile(a_bankName.c_str(), FMOD_STUDIO_LOAD_BANK_NORMAL, &bank));

        if (bank) banks[a_bankName] = bank;
    }
    
    void Audio::unloadAllBanks()
    {
        FMOD_CALL(AudioPlayer::s_studioSystem->unloadAll());
    }

    void Audio::loadEvent(const std::string& a_eventName)
    {
        auto& events = AudioPlayer::s_events;
        auto& guids  = AudioPlayer::s_guids;
        
        auto foundIt = events.find(a_eventName);
        if (foundIt != events.end())
            return;

        auto foundGUID = guids.find("event:/" + a_eventName);
        if (foundGUID == guids.end())
            return;

        const std::string& newEventGUID = foundGUID->second;

        FMOD::Studio::EventDescription* eventDescription = nullptr;
        FMOD_CALL(AudioPlayer::s_studioSystem->getEvent(newEventGUID.c_str(), &eventDescription));
        if (eventDescription)
        {
            FMOD::Studio::EventInstance* eventInstance = nullptr;
            FMOD_CALL(eventDescription->createInstance(&eventInstance));
            if (eventInstance)
                AudioPlayer::s_events[a_eventName] = eventInstance;
        }
    }

    void Audio::playEvent(const std::string& a_eventName)
    {
        auto it = AudioPlayer::s_events.find(a_eventName);
        if (it == AudioPlayer::s_events.end())
            return;

        FMOD_CALL(it->second->start());
    }

    void Audio::stopEvent(const std::string& a_eventName, bool a_fadeOut)
    {
        auto it = AudioPlayer::s_events.find(a_eventName);
        if (it == AudioPlayer::s_events.end())
            return;

        FMOD_STUDIO_STOP_MODE mode = a_fadeOut ? FMOD_STUDIO_STOP_ALLOWFADEOUT : FMOD_STUDIO_STOP_IMMEDIATE;
        FMOD_CALL(it->second->stop(mode));
    }

    float Audio::getEventVolume(const std::string& a_eventName)
    {
        auto it = AudioPlayer::s_events.find(a_eventName);
        if (it == AudioPlayer::s_events.end())
            return 0.0f;

        float ret;
        FMOD_CALL(it->second->getVolume(&ret));
        return ret;
    }
    
    void Audio::setEventVolume(const std::string& a_eventName, float a_volume)
    {
        auto it = AudioPlayer::s_events.find(a_eventName);
        if (it == AudioPlayer::s_events.end())
            return;

        FMOD_CALL(it->second->setVolume(a_volume));
    }

    bool Audio::isEventPlaying(const std::string& a_eventName)
    {
        auto it = AudioPlayer::s_events.find(a_eventName);
        if (it == AudioPlayer::s_events.end())
            return false;

        FMOD_STUDIO_PLAYBACK_STATE state;
        FMOD_CALL(it->second->getPlaybackState(&state));
        return state == FMOD_STUDIO_PLAYBACK_PLAYING;
    }

    float Audio::getEventParameter(const std::string& a_eventName, const std::string& a_eventParameter)
    {
        auto it = AudioPlayer::s_events.find(a_eventName);
        if (it == AudioPlayer::s_events.end())
            return NAN;

        float ret;
        FMOD_CALL(it->second->getParameterByName(a_eventParameter.c_str(), &ret));
        return ret;
    }

    void Audio::setEventParameter(const std::string& a_eventName, const std::string& a_eventParameter, float a_value)
    {
        auto it = AudioPlayer::s_events.find(a_eventName);
        if (it == AudioPlayer::s_events.end())
            return;

        FMOD_CALL(it->second->setParameterByName(a_eventParameter.c_str(), a_value));
    }

    void Audio::setGlobalParameter(const std::string& a_parameterName, float a_value)
    {
        FMOD_CALL(AudioPlayer::s_studioSystem->setParameterByName(a_parameterName.c_str(), a_value));
    }
}
