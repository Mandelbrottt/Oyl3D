#pragma once

#include "Oyl3D/Input/Input.h"

namespace oyl
{
    class Win32Input : public Input
    {
    protected:
        virtual bool isKeyPressedImpl(uint keyCode) override;
        virtual bool isMousePressedImpl(uint button) override;

        virtual glm::vec2 getMousePosImpl() override;
        virtual float     getMouseXImpl() override;
        virtual float     getMouseYImpl() override;
    };

    extern uint OylToGlfwCodes(uint code);
    extern uint glfwToOylCode(uint code);
}
