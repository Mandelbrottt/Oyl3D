#pragma once

#include <unordered_map>

namespace FMOD
{
    class System;
    class Sound;
    class Channel;
    
    namespace Studio
    {
        class System;
        class EventInstance;
        class Bank;
    }
}

namespace oyl
{
    class Application;
    class Audio;
    
    namespace internal
    {
        class AudioPlayer
        {
            friend ::oyl::Application;
            friend ::oyl::Audio;

            using BankMap    = std::unordered_map<std::string, FMOD::Studio::Bank*>;
            using ChannelMap = std::unordered_map<int, FMOD::Channel*>;
            using EventMap   = std::unordered_map<std::string, FMOD::Studio::EventInstance*>;
            using GUIDMap    = std::unordered_map<std::string, std::string>;
            using SoundMap   = std::unordered_map<std::string, FMOD::Sound*>;

        private:
            AudioPlayer() = default;

            static void init();
            static void shutdown();
            
            static void update();

            static void loadGUIDs();

        private:
            static FMOD::Studio::System* s_studioSystem;
            static FMOD::System*         s_system;
            
            static BankMap    s_banks;
            static ChannelMap s_channels;
            static EventMap   s_events;
            static GUIDMap    s_guids;
            static SoundMap   s_sounds;

            static int s_nextChannelID;
        };
    }

    class Audio
    {
    public:
        // Banks
        static void loadBank(const std::string& a_bankName);
        static void unloadAllBanks();

        // Events
        static void loadEvent(const std::string& a_eventName);
        static void playEvent(const std::string& a_eventName);
        static void stopEvent(const std::string& a_eventName, bool a_fadeOut = false);

        static float getEventVolume(const std::string& a_eventName);
        static void  setEventVolume(const std::string& a_eventName, float a_volume);
        
        static bool isEventPlaying(const std::string& a_eventName);

        // Parameters
        static float getEventParameter(const std::string& a_eventName, const std::string& a_eventParameter);
        static void  setEventParameter(const std::string& a_eventName, const std::string& a_eventParameter, float a_value);
        static void  setGlobalParameter(const std::string& a_parameterName, float a_value);
    };
}