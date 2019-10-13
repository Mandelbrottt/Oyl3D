#pragma once

#include "Oyl3D/Events/EventListener.h"

namespace oyl::_internal
{
    class GamepadListener : public EventListener
    {
    public:
        explicit GamepadListener();
		virtual ~GamepadListener() = default;

        // Called once per frame
        virtual void onUpdate(Timestep dt) = 0;

        static Ref<GamepadListener> create();
    private:
		static bool s_init;
    };
}