#pragma once

#include "Oyl3D/Events/EventListener.h"

namespace oyl::_internal
{
    class GamepadVibration : public EventListener
    {
    public:
        explicit GamepadVibration();
		virtual ~GamepadVibration() = default;

        // Called once per frame
        virtual void onUpdate(Timestep dt) = 0;

        static Ref<GamepadVibration> create();
    private:
		static bool s_init;
    };
}