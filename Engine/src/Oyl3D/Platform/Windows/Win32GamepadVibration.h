#pragma once

#include "Input/GamepadVibration.h"
#include "Input/Input.h"

namespace oyl::_internal
{
    class Win32GamepadVibration : public GamepadVibration
    {
    public:
        explicit Win32GamepadVibration()  = default;
        virtual  ~Win32GamepadVibration() = default;

        virtual bool onEvent(Ref<Event> event) override;

        virtual void onUpdate(Timestep dt) override;

    protected:
        struct _Gamepad
        {
            f32 leftTime = 0.0f, rightTime = 0.0f;
            f32 leftVibration = 0.0f, rightVibration = 0.0f;
        };
        
        _Gamepad gamepads[4];
    };
}
