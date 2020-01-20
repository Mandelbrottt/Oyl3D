#include "oylpch.h"
#include "Win32Time.h"

namespace oyl
{
    Time* Time::s_time = new internal::Win32Time;   
}

namespace oyl::internal
{
    void Win32Time::initImpl()
    {
        LARGE_INTEGER   li;
        QueryPerformanceCounter(&li);
        m_timeStart = li.QuadPart;

        QueryPerformanceFrequency(&li);
        m_frequency = static_cast<double>(li.QuadPart);
    }

    float Win32Time::timeImpl()
    {
        return m_time;
    }

    float Win32Time::unscaledTimeImpl()
    {
        return m_unscaledTime;   
    }

    float Win32Time::deltaTimeImpl()
    {
        return m_deltaTime;
    }
    
    float Win32Time::fixedDeltaTimeImpl()
    {
        return m_fixedDeltaTime;
    }

    float Win32Time::unscaledDeltaTimeImpl()
    {
        return m_unscaledDeltaTime;
    }

    float Win32Time::smoothDeltaTimeImpl()
    {
        return m_deltaTime;
    }

    void Win32Time::setTimeScaleImpl(float scale)
    {
        m_timeScaleHint = scale;
    }
    
    void Win32Time::updateImpl()
    {
        static LARGE_INTEGER lli;

        const int numFramesToHold = 10;
        static float prevFrames[numFramesToHold]{ 0.0f };
        static int prevFrameIndex = 0;

        m_timeScale = m_timeScaleHint;

        LARGE_INTEGER li;
        QueryPerformanceCounter(&li);

        m_unscaledTime = static_cast<float>(static_cast<double>(li.QuadPart - m_timeStart) / m_frequency);

        double currentTime = static_cast<double>(li.QuadPart - lli.QuadPart) / m_frequency;

        prevFrames[prevFrameIndex++] = glm::clamp(static_cast<float>(currentTime), 0.0f, 0.1f);
        float sum = 0.0f;
        int numCounts = 0;
        for (float frame : prevFrames)
            sum += frame, numCounts += (frame != 0.0f);
        sum /= (float) numCounts;
        prevFrameIndex %= 10;

        m_unscaledDeltaTime = sum;

        if (m_unscaledDeltaTime > 0.02f)
            OYL_LOG_INFO("{0}", m_unscaledDeltaTime);
        
        //m_unscaledDeltaTime = glm::clamp(static_cast<float>(currentTime), 0.0001f, 0.1f);
        //m_unscaledDeltaTime = glm::max(static_cast<float>(currentTime), 0.0f);
        m_deltaTime = m_unscaledDeltaTime * m_timeScale;

        m_timeDifference = m_timeDifference + m_unscaledDeltaTime - m_deltaTime;

        m_time = m_unscaledTime + m_timeDifference;

        lli = li;
    }
}
