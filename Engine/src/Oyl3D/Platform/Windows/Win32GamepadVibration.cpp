#include "oylpch.h"
#include "Win32GamepadVibration.h"

#include "Xinput.h"

#pragma comment(lib, "Xinput.lib");

namespace oyl::_internal
{
    bool Win32GamepadVibration::onEvent(Ref<Event> event)
    {
        auto e = (GamepadVibrationEvent) *event;

        OYL_ASSERT(e.gid < 4);

        f32& currLeftTime  = gamepads[e.gid].leftTime;
        f32& currRightTime = gamepads[e.gid].rightTime;
        f32& currLeftVibe  = gamepads[e.gid].leftVibration;
        f32& currRightVibe = gamepads[e.gid].rightVibration;

        e.leftMotor = glm::clamp(e.leftMotor, 0.0f, 1.0f);
        e.rightMotor = glm::clamp(e.rightMotor, 0.0f, 1.0f);

        if (e.leftTime > 0.0f)
        {
            currLeftTime = std::max(currLeftTime, e.leftTime);
            currLeftVibe = std::max(currLeftVibe, e.leftMotor);
        }
        if (e.rightTime > 0.0f)
        {
            currRightTime = std::max(currRightTime, e.rightTime);
            currRightVibe = std::max(currRightVibe, e.rightMotor);
        }
        return false;
    }

    void Win32GamepadVibration::onUpdate(Timestep dt)
    {
        XINPUT_VIBRATION vibration[4];

        ZeroMemory(vibration, sizeof(XINPUT_VIBRATION) * 4);

        for (int i = 0; i < 4; i++)
        {
            _Gamepad& currentGamepad = gamepads[i];

            vibration[i].wLeftMotorSpeed  = (u16) (65535.0f * currentGamepad.leftVibration);
            vibration[i].wRightMotorSpeed = (u16) (65535.0f * currentGamepad.rightVibration);

            XInputSetState(i, &vibration[i]);

            currentGamepad.leftTime -= dt.getSeconds();
            currentGamepad.rightTime -= dt.getSeconds();

            if (currentGamepad.leftTime <= 0)
            {
                currentGamepad.leftTime      = 0;
                currentGamepad.leftVibration = 0;
            }
            if (currentGamepad.rightTime <= 0)
            {
                currentGamepad.rightTime      = 0;
                currentGamepad.rightVibration = 0;
            }
        }
    }
}
