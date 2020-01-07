#pragma once

#include "Oyl3D/Events/EventListener.h"

namespace oyl::internal
{
    class GamepadListener : public EventListener
    {
    public:
        explicit GamepadListener();
		virtual ~GamepadListener() = default;

        // Called once per frame
        virtual void onUpdate() = 0;

        static Ref<GamepadListener> create();
    private:
		static bool s_init;
    };
}