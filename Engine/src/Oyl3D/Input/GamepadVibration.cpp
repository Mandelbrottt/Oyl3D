#include "oylpch.h"
#include "GamepadVibration.h"
#include "Platform/Windows/Win32GamepadVibration.h"

namespace oyl::_internal
{
    bool GamepadVibration::s_init = false;

    GamepadVibration::GamepadVibration()
    {
        OYL_ASSERT(!s_init, "GamepadVibration should only be initalized once!");
        s_init = true;

        EventListener::addToEventMask(TypeGamepadVibration);
        EventListener::addToCategoryMask(CategoryGamepadVibration);
    }

    Ref<GamepadVibration> GamepadVibration::create()
    {
#if   defined(OYL_PLATFORM_WINDOWS)
        return Ref<Win32GamepadVibration>::create();
#elif defined(OYL_PLATFORM_OSX)
#error Oyl3D does not support MacOS!
#elif defined(OYL_PLATFORM_LINUX)
#error Oyl3D does not support Linux!
#endif
    }
}
