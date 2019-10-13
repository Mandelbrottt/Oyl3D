#pragma once

#include "Oyl3D/oylpch.h"

namespace oyl
{
    class Input
    {
    public:
        virtual ~Input() = default;

        inline static bool isKeyPressed(uint keyCode) { return s_instance->isKeyPressedImpl(keyCode); }
        inline static bool isMousePressed(uint button = Mouse_1) { return s_instance->isMousePressedImpl(button); }

        inline static bool isGamepadButtonPressed(uint button, uint gid = 0)
        {
            return s_instance->isGamepadButtonPressedImpl(button, gid);
        }

        inline static glm::vec2 getMousePos() { return s_instance->getMousePosImpl(); }
        inline static float     getMouseX() { return s_instance->getMouseXImpl(); }
        inline static float     getMouseY() { return s_instance->getMouseYImpl(); }

        inline static glm::vec2 getGamepadLeftStick(uint gid = 0) { return s_instance->getGamepadLeftStickImpl(gid); }
        inline static float     getGamepadLeftStickX(uint gid = 0) { return s_instance->getGamepadLeftStickXImpl(gid); }
        inline static float     getGamepadLeftStickY(uint gid = 0) { return s_instance->getGamepadLeftStickYImpl(gid); }

        inline static glm::vec2 getGamepadRightStick(uint gid = 0) { return s_instance->getGamepadRightStickImpl(gid); }
        inline static float     getGamepadRightStickX(uint gid = 0) { return s_instance->getGamepadRightStickXImpl(gid); }
        inline static float     getGamepadRightStickY(uint gid = 0) { return s_instance->getGamepadRightStickYImpl(gid); }

        inline static float getGamepadLeftTrigger(uint gid = 0) { return s_instance->getGamepadLeftTriggerImpl(gid); }
        inline static float getGamepadRightTrigger(uint gid = 0) { return s_instance->getGamepadRightTriggerImpl(gid); }

    protected:
        virtual bool isKeyPressedImpl(uint keyCode) = 0;
        virtual bool isMousePressedImpl(uint button) = 0;
        virtual bool isGamepadButtonPressedImpl(uint button, uint gid) = 0;

        virtual glm::vec2 getMousePosImpl() = 0;
        virtual float     getMouseXImpl() = 0;
        virtual float     getMouseYImpl() = 0;

        virtual glm::vec2 getGamepadLeftStickImpl(uint gid) = 0;
        virtual float     getGamepadLeftStickXImpl(uint gid) = 0;
        virtual float     getGamepadLeftStickYImpl(uint gid) = 0;

        virtual glm::vec2 getGamepadRightStickImpl(uint gid) = 0;
        virtual float     getGamepadRightStickXImpl(uint gid) = 0;
        virtual float     getGamepadRightStickYImpl(uint gid) = 0;

        virtual float getGamepadLeftTriggerImpl(uint gid) = 0;
        virtual float getGamepadRightTriggerImpl(uint gid) = 0;
    private:
        static Input* s_instance;
    };
}
