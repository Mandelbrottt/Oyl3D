#include "oylpch.h"
#include "GamepadListener.h"
#include "Platform/Windows/Win32GamepadListener.h"

#include "Events/Event.h"

namespace oyl::internal
{
    bool GamepadListener::s_init = false;

    GamepadListener::GamepadListener()
    {
        OYL_ASSERT(!s_init, "GamepadVibration should only be initalized once!");
        s_init = true;

        listenForEventType(EventType::GamepadVibrationRequest);
        listenForEventCategory(EventCategory::GamepadVibration);
    }

    Ref<GamepadListener> GamepadListener::create()
    {
    #if   defined(OYL_PLATFORM_WINDOWS)
        return Ref<Win32GamepadListener>::create();
    #elif defined(OYL_PLATFORM_OSX)
        #error Oyl3D does not support MacOS!
    #elif defined(OYL_PLATFORM_LINUX)
        #error Oyl3D does not support Linux!
    #endif
    }
}
