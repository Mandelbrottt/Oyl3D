#pragma once

#include "Oyl3D/Input/Input.h"

namespace oyl
{
    class Win32Input : public Input
    {
    protected:
        virtual bool isKeyPressedImpl(uint keyCode) override;
        virtual bool isMousePressedImpl(uint button) override;
        bool         isGamepadButtonPressedImpl(uint button, uint gid) override;

        virtual glm::vec2 getMousePosImpl() override;
        virtual float     getMouseXImpl() override;
        virtual float     getMouseYImpl() override;

        glm::vec2 getGamepadLeftStickImpl(uint gid) override;
        float     getGamepadLeftStickXImpl(uint gid) override;
        float     getGamepadLeftStickYImpl(uint gid) override;

        glm::vec2 getGamepadRightStickImpl(uint gid) override;
        float     getGamepadRightStickXImpl(uint gid) override;
        float     getGamepadRightStickYImpl(uint gid) override;

        float getGamepadLeftTriggerImpl(uint gid) override;
        float getGamepadRightTriggerImpl(uint gid) override;
    };

    extern uint OylToGlfwCodes(uint code);
    extern uint glfwToOylCode(uint code);
}
