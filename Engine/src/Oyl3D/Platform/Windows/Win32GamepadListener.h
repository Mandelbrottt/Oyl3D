#pragma once

#include "Input/GamepadListener.h"
#include "Input/Input.h"

namespace oyl::internal
{
class Win32GamepadListener : public GamepadListener
{
public:
    explicit Win32GamepadListener() = default;
    virtual  ~Win32GamepadListener() = default;

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
