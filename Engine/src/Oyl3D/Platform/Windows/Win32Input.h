#pragma once

#include "Oyl3D/Input/Input.h"

namespace oyl
{
    class Win32Input final : public Input
    {
    protected:
        bool isKeyPressedImpl(Key keyCode) override;
        bool isMousePressedImpl(Mouse button) override;
        bool isGamepadButtonPressedImpl(Gamepad button, uint gid) override;

        glm::vec2 getMousePosImpl() override;
        float     getMouseXImpl() override;
        float     getMouseYImpl() override;

        glm::vec2 getGamepadLeftStickImpl(uint gid) override;
        float     getGamepadLeftStickXImpl(uint gid) override;
        float     getGamepadLeftStickYImpl(uint gid) override;

        glm::vec2 getGamepadRightStickImpl(uint gid) override;
        float     getGamepadRightStickXImpl(uint gid) override;
        float     getGamepadRightStickYImpl(uint gid) override;

        float getGamepadLeftTriggerImpl(uint gid) override;
        float getGamepadRightTriggerImpl(uint gid) override;
    };

    extern uint OylToGlfwKeyCode(Key code);
    extern Key glfwToOylKeyCode(uint code);

    extern uint OylToGlfwMouseCode(Mouse code);
    extern Mouse glfwToOylMouseCode(uint code);

    extern uint OylToGlfwGamepadCode(Gamepad code);
    extern Gamepad glfwToOylGamepadCode(uint code);
}
