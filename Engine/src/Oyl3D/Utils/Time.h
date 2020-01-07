#pragma once

namespace oyl
{    
    class Time
    {
        friend class Application;
    public:
        virtual ~Time() = default;
        
        static float time() { return s_time->timeImpl(); }
        static float unscaledTime() { return s_time->timeImpl(); }

        static float deltaTime() { return s_time->deltaTimeImpl(); }
        static float unscaledDeltaTime() { return s_time->unscaledDeltaTimeImpl(); }
        static float fixedDeltaTime() { return s_time->fixedDeltaTimeImpl(); }
        static float smoothDeltaTime() { return s_time->smoothDeltaTimeImpl(); }

        static void setTimeScale(float scale) { s_time->setTimeScaleImpl(scale); }

    protected:
        virtual float timeImpl() = 0;
        virtual float unscaledTimeImpl() = 0;

        virtual float deltaTimeImpl() = 0;
        virtual float unscaledDeltaTimeImpl() = 0;
        virtual float fixedDeltaTimeImpl() = 0;
        virtual float smoothDeltaTimeImpl() = 0;

        virtual void setTimeScaleImpl(float scale) = 0;

        static void init() { s_time->initImpl(); }
        static void update() { s_time->updateImpl(); }

        virtual void initImpl() = 0;
        virtual void updateImpl() = 0;

    private:
        static Time* s_time;
    };
}