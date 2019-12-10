#pragma once

#include "Utils/Time.h"

namespace oyl::internal
{
    class Win32Time : public Time
    {
    public:
        virtual ~Win32Time() = default;
        
    protected:
        float timeImpl() override;
        float unscaledTimeImpl() override;
        
        float deltaTimeImpl() override;
        float fixedDeltaTimeImpl() override;
        float unscaledDeltaTimeImpl() override;
        float smoothDeltaTimeImpl() override;

        void setTimeScaleImpl(float scale) override;

        void initImpl() override;
        void updateImpl() override;
        
    private:
        float m_deltaTime         = 0.0f;
        float m_unscaledDeltaTime = 0.0f;
        float m_fixedDeltaTime    = 1.0f / 60.0f;

        float m_time = 0.0f;
        float m_unscaledTime = 0.0f;
        float m_timeDifference = 0.0f;

        float m_timeScale     = 1.0f;
        float m_timeScaleHint = 1.0f;

        u64    m_timeStart = 0;
        double m_frequency = 0.0f;
    };
}