#include "oylpch.h"
#include "Win32Input.h"

#include "Oyl3D/App/Application.h"

#include <GLFW/glfw3.h>

namespace oyl
{
    Input* Input::s_instance = new Win32Input();

    bool Win32Input::isKeyPressedImpl(uint keyCode)
    {
        auto window = static_cast<GLFWwindow*>(Application::get().getWindow().getNativeWindow());
        auto state  = glfwGetKey(window, keyCode);
        return state == GLFW_PRESS || state == GLFW_REPEAT;
    }

    bool Win32Input::isMousePressedImpl(uint button)
    {
        auto window = static_cast<GLFWwindow*>(Application::get().getWindow().getNativeWindow());
        auto state  = glfwGetMouseButton(window, button);
        return state == GLFW_PRESS;
    }

    bool Win32Input::isGamepadButtonPressedImpl(uint button, uint gid)
    {
        OYL_ASSERT(glfwJoystickPresent(gid) == GLFW_TRUE, "Joystick should be present!");

        static GLFWgamepadstate state;

        if (glfwJoystickIsGamepad(gid))
        {
            glfwGetGamepadState(gid, &state);
            return state.buttons[OylToGlfwCodes(button)] == GLFW_PRESS;
        }
        return false;
    }

    glm::vec2 Win32Input::getMousePosImpl()
    {
        auto   window = static_cast<GLFWwindow*>(Application::get().getWindow().getNativeWindow());
        double xpos, ypos;
        glfwGetCursorPos(window, &xpos, &ypos);
        return { (float) xpos, (float) ypos };
    }

    float Win32Input::getMouseXImpl()
    {
        return getMousePosImpl().x;
    }

    float Win32Input::getMouseYImpl()
    {
        return getMousePosImpl().y;
    }

    glm::vec2 Win32Input::getGamepadLeftStickImpl(uint gid)
    {
        OYL_ASSERT(glfwJoystickPresent(gid) == GLFW_TRUE, "Joystick should be present!");

        static GLFWgamepadstate state;

        if (glfwJoystickIsGamepad(gid))
        {
            glfwGetGamepadState(gid, &state);
            float xpos = state.axes[GLFW_GAMEPAD_AXIS_LEFT_X];
            float ypos = state.axes[GLFW_GAMEPAD_AXIS_LEFT_Y];
            return { xpos, ypos };
        }
        return { 0, 0 };
    }

    float Win32Input::getGamepadLeftStickXImpl(uint gid)
    {
        return getGamepadLeftStickImpl(gid).x;
    }

    float Win32Input::getGamepadLeftStickYImpl(uint gid)
    {
        return getGamepadLeftStickImpl(gid).y;
    }

    glm::vec2 Win32Input::getGamepadRightStickImpl(uint gid)
    {
        OYL_ASSERT(glfwJoystickPresent(gid) == GLFW_TRUE, "Joystick should be present!");

        static GLFWgamepadstate state;

        if (glfwJoystickIsGamepad(gid))
        {
            glfwGetGamepadState(gid, &state);
            float xpos = state.axes[GLFW_GAMEPAD_AXIS_RIGHT_X];
            float ypos = state.axes[GLFW_GAMEPAD_AXIS_RIGHT_Y];
            return { xpos, ypos };
        }
        return { 0, 0 };
    }

    float Win32Input::getGamepadRightStickXImpl(uint gid)
    {
        return getGamepadLeftStickImpl(gid).y;
    }

    float Win32Input::getGamepadRightStickYImpl(uint gid)
    {
        return getGamepadLeftStickImpl(gid).y;
    }

    float Win32Input::getGamepadLeftTriggerImpl(uint gid)
    {
        OYL_ASSERT(glfwJoystickPresent(gid) == GLFW_TRUE, "Joystick should be present!");

        static GLFWgamepadstate state;

        if (glfwJoystickIsGamepad(gid))
        {
            glfwGetGamepadState(gid, &state);
            float pos = state.axes[GLFW_GAMEPAD_AXIS_LEFT_TRIGGER];
            return pos;
        }
        return 0.0f;
    }

    float Win32Input::getGamepadRightTriggerImpl(uint gid)
    {
        OYL_ASSERT(glfwJoystickPresent(gid) == GLFW_TRUE, "Joystick should be present!");

        static GLFWgamepadstate state;

        if (glfwJoystickIsGamepad(gid))
        {
            glfwGetGamepadState(gid, &state);
            float pos = state.axes[GLFW_GAMEPAD_AXIS_RIGHT_TRIGGER];
            return pos;
        }
        return 0.0f;
    }

    uint OylToGlfwCodes(uint code)
    {
        switch (code)
        {
            case Key_Space: return GLFW_KEY_SPACE;
            case Key_Apostrophe: return GLFW_KEY_APOSTROPHE;
            case Key_Comma: return GLFW_KEY_COMMA;
            case Key_Minus: return GLFW_KEY_MINUS;
            case Key_Period: return GLFW_KEY_PERIOD;
            case Key_Slash: return GLFW_KEY_SLASH;
            case Key_0: return GLFW_KEY_0;
            case Key_1: return GLFW_KEY_1;
            case Key_2: return GLFW_KEY_2;
            case Key_3: return GLFW_KEY_3;
            case Key_4: return GLFW_KEY_4;
            case Key_5: return GLFW_KEY_5;
            case Key_6: return GLFW_KEY_6;
            case Key_7: return GLFW_KEY_7;
            case Key_8: return GLFW_KEY_8;
            case Key_9: return GLFW_KEY_9;
            case Key_Semicolon: return GLFW_KEY_SEMICOLON;
            case Key_Equal: return GLFW_KEY_EQUAL;
            case Key_A: return GLFW_KEY_A;
            case Key_B: return GLFW_KEY_B;
            case Key_C: return GLFW_KEY_C;
            case Key_D: return GLFW_KEY_D;
            case Key_E: return GLFW_KEY_E;
            case Key_F: return GLFW_KEY_F;
            case Key_G: return GLFW_KEY_G;
            case Key_H: return GLFW_KEY_H;
            case Key_I: return GLFW_KEY_I;
            case Key_J: return GLFW_KEY_J;
            case Key_K: return GLFW_KEY_K;
            case Key_L: return GLFW_KEY_L;
            case Key_M: return GLFW_KEY_M;
            case Key_N: return GLFW_KEY_N;
            case Key_O: return GLFW_KEY_O;
            case Key_P: return GLFW_KEY_P;
            case Key_Q: return GLFW_KEY_Q;
            case Key_R: return GLFW_KEY_R;
            case Key_S: return GLFW_KEY_S;
            case Key_T: return GLFW_KEY_T;
            case Key_U: return GLFW_KEY_U;
            case Key_V: return GLFW_KEY_V;
            case Key_W: return GLFW_KEY_W;
            case Key_X: return GLFW_KEY_X;
            case Key_Y: return GLFW_KEY_Y;
            case Key_Z: return GLFW_KEY_Z;
            case Key_LeftBracket: return GLFW_KEY_LEFT_BRACKET;
            case Key_Backslash: return GLFW_KEY_BACKSLASH;
            case Key_RightBracket: return GLFW_KEY_RIGHT_BRACKET;
            case Key_Grave: return GLFW_KEY_GRAVE_ACCENT;
            case Key_World1: return GLFW_KEY_WORLD_1;
            case Key_World2: return GLFW_KEY_WORLD_2;

            case Key_Escape: return GLFW_KEY_ESCAPE;
            case Key_Enter: return GLFW_KEY_ENTER;
            case Key_Tab: return GLFW_KEY_TAB;
            case Key_Backspace: return GLFW_KEY_BACKSPACE;
            case Key_Insert: return GLFW_KEY_INSERT;
            case Key_Delete: return GLFW_KEY_DELETE;
            case Key_Right: return GLFW_KEY_RIGHT;
            case Key_Left: return GLFW_KEY_LEFT;
            case Key_Down: return GLFW_KEY_DOWN;
            case Key_Up: return GLFW_KEY_UP;
            case Key_PageUp: return GLFW_KEY_PAGE_UP;
            case Key_PageDown: return GLFW_KEY_PAGE_DOWN;
            case Key_Home: return GLFW_KEY_HOME;
            case Key_End: return GLFW_KEY_END;
            case Key_CapsLock: return GLFW_KEY_CAPS_LOCK;
            case Key_ScrollLock: return GLFW_KEY_SCROLL_LOCK;
            case Key_NumLock: return GLFW_KEY_NUM_LOCK;
            case Key_PrintScreen: return GLFW_KEY_PRINT_SCREEN;
            case Key_Pause: return GLFW_KEY_PAUSE;
            case Key_F1: return GLFW_KEY_F1;
            case Key_F2: return GLFW_KEY_F2;
            case Key_F3: return GLFW_KEY_F3;
            case Key_F4: return GLFW_KEY_F4;
            case Key_F5: return GLFW_KEY_F5;
            case Key_F6: return GLFW_KEY_F6;
            case Key_F7: return GLFW_KEY_F7;
            case Key_F8: return GLFW_KEY_F8;
            case Key_F9: return GLFW_KEY_F9;
            case Key_F10: return GLFW_KEY_F10;
            case Key_F11: return GLFW_KEY_F11;
            case Key_F12: return GLFW_KEY_F12;
            case Key_F13: return GLFW_KEY_F13;
            case Key_F14: return GLFW_KEY_F14;
            case Key_F15: return GLFW_KEY_F15;
            case Key_F16: return GLFW_KEY_F16;
            case Key_F17: return GLFW_KEY_F17;
            case Key_F18: return GLFW_KEY_F18;
            case Key_F19: return GLFW_KEY_F19;
            case Key_F20: return GLFW_KEY_F20;
            case Key_F21: return GLFW_KEY_F21;
            case Key_F22: return GLFW_KEY_F22;
            case Key_F23: return GLFW_KEY_F23;
            case Key_F24: return GLFW_KEY_F24;
            case Key_F25: return GLFW_KEY_F25;
            case Key_Keypad0: return GLFW_KEY_KP_0;
            case Key_Keypad1: return GLFW_KEY_KP_1;
            case Key_Keypad2: return GLFW_KEY_KP_2;
            case Key_Keypad3: return GLFW_KEY_KP_3;
            case Key_Keypad4: return GLFW_KEY_KP_4;
            case Key_Keypad5: return GLFW_KEY_KP_5;
            case Key_Keypad6: return GLFW_KEY_KP_6;
            case Key_Keypad7: return GLFW_KEY_KP_7;
            case Key_Keypad8: return GLFW_KEY_KP_8;
            case Key_Keypad9: return GLFW_KEY_KP_9;
            case Key_KeypadDecimal: return GLFW_KEY_KP_DECIMAL;
            case Key_KeypadDivide: return GLFW_KEY_KP_DIVIDE;
            case Key_KeypadTimes: return GLFW_KEY_KP_MULTIPLY;
            case Key_KeypadMinus: return GLFW_KEY_KP_SUBTRACT;
            case Key_KeypadPlus: return GLFW_KEY_KP_ADD;
            case Key_KeypadEnter: return GLFW_KEY_KP_ENTER;
            case Key_KeypadEqual: return GLFW_KEY_KP_EQUAL;
            case Key_LeftShift: return GLFW_KEY_LEFT_SHIFT;
            case Key_LeftControl: return GLFW_KEY_LEFT_CONTROL;
            case Key_LeftAlt: return GLFW_KEY_LEFT_ALT;
            case Key_LeftSuper: return GLFW_KEY_LEFT_SUPER;
            case Key_RightShift: return GLFW_KEY_RIGHT_SHIFT;
            case Key_RightControl: return GLFW_KEY_RIGHT_CONTROL;
            case Key_RightAlt: return GLFW_KEY_RIGHT_ALT;
            case Key_RightSuper: return GLFW_KEY_RIGHT_SUPER;
            case Key_Menu: return GLFW_KEY_MENU;

            case Mouse_1: return GLFW_MOUSE_BUTTON_1;
            case Mouse_2: return GLFW_MOUSE_BUTTON_2;
            case Mouse_3: return GLFW_MOUSE_BUTTON_3;
            case Mouse_4: return GLFW_MOUSE_BUTTON_4;
            case Mouse_5: return GLFW_MOUSE_BUTTON_5;
            case Mouse_6: return GLFW_MOUSE_BUTTON_6;
            case Mouse_7: return GLFW_MOUSE_BUTTON_7;
            case Mouse_8: return GLFW_MOUSE_BUTTON_8;

                /* Reference only
                 * case Gamepad_A: return GLFW_GAMEPAD_BUTTON_A;
                 * case Gamepad_B: return GLFW_GAMEPAD_BUTTON_B;
                 * case Gamepad_X: return GLFW_GAMEPAD_BUTTON_X;
                 * case Gamepad_Y: return GLFW_GAMEPAD_BUTTON_Y;
                 * case Gamepad_LeftBumper: return GLFW_GAMEPAD_BUTTON_LEFT_BUMPER;
                 * case Gamepad_RightBumper: return GLFW_GAMEPAD_BUTTON_RIGHT_BUMPER;
                 * case Gamepad_Back: return GLFW_GAMEPAD_BUTTON_BACK;
                 * case Gamepad_Start: return GLFW_GAMEPAD_BUTTON_START;
                 */
            case Gamepad_Guide: return GLFW_GAMEPAD_BUTTON_GUIDE;
            case Gamepad_LeftThumb: return GLFW_GAMEPAD_BUTTON_LEFT_THUMB;
            case Gamepad_RightThumb: return GLFW_GAMEPAD_BUTTON_RIGHT_THUMB;
            case Gamepad_DPadUp: return GLFW_GAMEPAD_BUTTON_DPAD_UP;
            case Gamepad_DPadRight: return GLFW_GAMEPAD_BUTTON_DPAD_RIGHT;
            case Gamepad_DPadDown: return GLFW_GAMEPAD_BUTTON_DPAD_DOWN;
            case Gamepad_DPadLeft: return GLFW_GAMEPAD_BUTTON_DPAD_LEFT;

                /* case Gamepad_LeftStickX: return GLFW_GAMEPAD_AXIS_LEFT_X;
                 * case Gamepad_LeftStickY: return GLFW_GAMEPAD_AXIS_LEFT_Y;
                 * case Gamepad_RightStickX: return GLFW_GAMEPAD_AXIS_RIGHT_X;
                 * case Gamepad_RightStickY: return GLFW_GAMEPAD_AXIS_RIGHT_Y;
                 * case Gamepad_LeftTrigger: return GLFW_GAMEPAD_AXIS_LEFT_TRIGGER;
                 * case Gamepad_RightTrigger: return GLFW_GAMEPAD_AXIS_RIGHT_TRIGGER;
                 */
            default: return 0xDEADBEEF;
        }
    }

    uint glfwToOylCode(uint code)
    {
        switch (code)
        {
            case GLFW_KEY_SPACE: return Key_Space;
            case GLFW_KEY_APOSTROPHE: return Key_Apostrophe;
            case GLFW_KEY_COMMA: return Key_Comma;
            case GLFW_KEY_MINUS: return Key_Minus;
            case GLFW_KEY_PERIOD: return Key_Period;
            case GLFW_KEY_SLASH: return Key_Slash;
            case GLFW_KEY_0: return Key_0;
            case GLFW_KEY_1: return Key_1;
            case GLFW_KEY_2: return Key_2;
            case GLFW_KEY_3: return Key_3;
            case GLFW_KEY_4: return Key_4;
            case GLFW_KEY_5: return Key_5;
            case GLFW_KEY_6: return Key_6;
            case GLFW_KEY_7: return Key_7;
            case GLFW_KEY_8: return Key_8;
            case GLFW_KEY_9: return Key_9;
            case GLFW_KEY_SEMICOLON: return Key_Semicolon;
            case GLFW_KEY_EQUAL: return Key_Equal;
            case GLFW_KEY_A: return Key_A;
            case GLFW_KEY_B: return Key_B;
            case GLFW_KEY_C: return Key_C;
            case GLFW_KEY_D: return Key_D;
            case GLFW_KEY_E: return Key_E;
            case GLFW_KEY_F: return Key_F;
            case GLFW_KEY_G: return Key_G;
            case GLFW_KEY_H: return Key_H;
            case GLFW_KEY_I: return Key_I;
            case GLFW_KEY_J: return Key_J;
            case GLFW_KEY_K: return Key_K;
            case GLFW_KEY_L: return Key_L;
            case GLFW_KEY_M: return Key_M;
            case GLFW_KEY_N: return Key_N;
            case GLFW_KEY_O: return Key_O;
            case GLFW_KEY_P: return Key_P;
            case GLFW_KEY_Q: return Key_Q;
            case GLFW_KEY_R: return Key_R;
            case GLFW_KEY_S: return Key_S;
            case GLFW_KEY_T: return Key_T;
            case GLFW_KEY_U: return Key_U;
            case GLFW_KEY_V: return Key_V;
            case GLFW_KEY_W: return Key_W;
            case GLFW_KEY_X: return Key_X;
            case GLFW_KEY_Y: return Key_Y;
            case GLFW_KEY_Z: return Key_Z;
            case GLFW_KEY_LEFT_BRACKET: return Key_LeftBracket;
            case GLFW_KEY_BACKSLASH: return Key_Backslash;
            case GLFW_KEY_RIGHT_BRACKET: return Key_RightBracket;
            case GLFW_KEY_GRAVE_ACCENT: return Key_Grave;
            case GLFW_KEY_WORLD_1: return Key_World1;
            case GLFW_KEY_WORLD_2: return Key_World2;

            case GLFW_KEY_ESCAPE: return Key_Escape;
            case GLFW_KEY_ENTER: return Key_Enter;
            case GLFW_KEY_TAB: return Key_Tab;
            case GLFW_KEY_BACKSPACE: return Key_Backspace;
            case GLFW_KEY_INSERT: return Key_Insert;
            case GLFW_KEY_DELETE: return Key_Delete;
            case GLFW_KEY_RIGHT: return Key_Right;
            case GLFW_KEY_LEFT: return Key_Left;
            case GLFW_KEY_DOWN: return Key_Down;
            case GLFW_KEY_UP: return Key_Up;
            case GLFW_KEY_PAGE_UP: return Key_PageUp;
            case GLFW_KEY_PAGE_DOWN: return Key_PageDown;
            case GLFW_KEY_HOME: return Key_Home;
            case GLFW_KEY_END: return Key_End;
            case GLFW_KEY_CAPS_LOCK: return Key_CapsLock;
            case GLFW_KEY_SCROLL_LOCK: return Key_ScrollLock;
            case GLFW_KEY_NUM_LOCK: return Key_NumLock;
            case GLFW_KEY_PRINT_SCREEN: return Key_PrintScreen;
            case GLFW_KEY_PAUSE: return Key_Pause;
            case GLFW_KEY_F1: return Key_F1;
            case GLFW_KEY_F2: return Key_F2;
            case GLFW_KEY_F3: return Key_F3;
            case GLFW_KEY_F4: return Key_F4;
            case GLFW_KEY_F5: return Key_F5;
            case GLFW_KEY_F6: return Key_F6;
            case GLFW_KEY_F7: return Key_F7;
            case GLFW_KEY_F8: return Key_F8;
            case GLFW_KEY_F9: return Key_F9;
            case GLFW_KEY_F10: return Key_F10;
            case GLFW_KEY_F11: return Key_F11;
            case GLFW_KEY_F12: return Key_F12;
            case GLFW_KEY_F13: return Key_F13;
            case GLFW_KEY_F14: return Key_F14;
            case GLFW_KEY_F15: return Key_F15;
            case GLFW_KEY_F16: return Key_F16;
            case GLFW_KEY_F17: return Key_F17;
            case GLFW_KEY_F18: return Key_F18;
            case GLFW_KEY_F19: return Key_F19;
            case GLFW_KEY_F20: return Key_F20;
            case GLFW_KEY_F21: return Key_F21;
            case GLFW_KEY_F22: return Key_F22;
            case GLFW_KEY_F23: return Key_F23;
            case GLFW_KEY_F24: return Key_F24;
            case GLFW_KEY_F25: return Key_F25;
            case GLFW_KEY_KP_0: return Key_Keypad0;
            case GLFW_KEY_KP_1: return Key_Keypad1;
            case GLFW_KEY_KP_2: return Key_Keypad2;
            case GLFW_KEY_KP_3: return Key_Keypad3;
            case GLFW_KEY_KP_4: return Key_Keypad4;
            case GLFW_KEY_KP_5: return Key_Keypad5;
            case GLFW_KEY_KP_6: return Key_Keypad6;
            case GLFW_KEY_KP_7: return Key_Keypad7;
            case GLFW_KEY_KP_8: return Key_Keypad8;
            case GLFW_KEY_KP_9: return Key_Keypad9;
            case GLFW_KEY_KP_DECIMAL: return Key_KeypadDecimal;
            case GLFW_KEY_KP_DIVIDE: return Key_KeypadDivide;
            case GLFW_KEY_KP_MULTIPLY: return Key_KeypadTimes;
            case GLFW_KEY_KP_SUBTRACT: return Key_KeypadMinus;
            case GLFW_KEY_KP_ADD: return Key_KeypadPlus;
            case GLFW_KEY_KP_ENTER: return Key_KeypadEnter;
            case GLFW_KEY_KP_EQUAL: return Key_KeypadEqual;
            case GLFW_KEY_LEFT_SHIFT: return Key_LeftShift;
            case GLFW_KEY_LEFT_CONTROL: return Key_LeftControl;
            case GLFW_KEY_LEFT_ALT: return Key_LeftAlt;
            case GLFW_KEY_LEFT_SUPER: return Key_LeftSuper;
            case GLFW_KEY_RIGHT_SHIFT: return Key_RightShift;
            case GLFW_KEY_RIGHT_CONTROL: return Key_RightControl;
            case GLFW_KEY_RIGHT_ALT: return Key_RightAlt;
            case GLFW_KEY_RIGHT_SUPER: return Key_RightSuper;
            case GLFW_KEY_MENU: return Key_Menu;

            case GLFW_MOUSE_BUTTON_1: return Mouse_1;
            case GLFW_MOUSE_BUTTON_2: return Mouse_2;
            case GLFW_MOUSE_BUTTON_3: return Mouse_3;
            case GLFW_MOUSE_BUTTON_4: return Mouse_4;
            case GLFW_MOUSE_BUTTON_5: return Mouse_5;
            case GLFW_MOUSE_BUTTON_6: return Mouse_6;
            case GLFW_MOUSE_BUTTON_7: return Mouse_7;
            case GLFW_MOUSE_BUTTON_8: return Mouse_8;

                /* Reference only
                 * case GLFW_GAMEPAD_BUTTON_A: return Gamepad_A;
                 * case GLFW_GAMEPAD_BUTTON_B: return Gamepad_B;
                 * case GLFW_GAMEPAD_BUTTON_X: return Gamepad_X;
                 * case GLFW_GAMEPAD_BUTTON_Y: return Gamepad_Y;
                 * case GLFW_GAMEPAD_BUTTON_LEFT_BUMPER: return Gamepad_LeftBumper;
                 * case GLFW_GAMEPAD_BUTTON_RIGHT_BUMPER: return Gamepad_RightBumper;
                 * case GLFW_GAMEPAD_BUTTON_BACK: return Gamepad_Back;
                 * case GLFW_GAMEPAD_BUTTON_START: return Gamepad_Start;
                 */
            case GLFW_GAMEPAD_BUTTON_GUIDE: return Gamepad_Guide;
            case GLFW_GAMEPAD_BUTTON_LEFT_THUMB: return Gamepad_LeftThumb;
            case GLFW_GAMEPAD_BUTTON_RIGHT_THUMB: return Gamepad_RightThumb;
            case GLFW_GAMEPAD_BUTTON_DPAD_UP: return Gamepad_DPadUp;
            case GLFW_GAMEPAD_BUTTON_DPAD_RIGHT: return Gamepad_DPadRight;
            case GLFW_GAMEPAD_BUTTON_DPAD_DOWN: return Gamepad_DPadDown;
            case GLFW_GAMEPAD_BUTTON_DPAD_LEFT: return Gamepad_DPadLeft;

                /* Reference Only
                 * case GLFW_GAMEPAD_AXIS_LEFT_X: return Gamepad_LeftStickX;
                 * case GLFW_GAMEPAD_AXIS_LEFT_Y: return Gamepad_LeftStickY;
                 * case GLFW_GAMEPAD_AXIS_RIGHT_X: return Gamepad_RightStickX;
                 * case GLFW_GAMEPAD_AXIS_RIGHT_Y: return Gamepad_RightStickY;
                 * case GLFW_GAMEPAD_AXIS_LEFT_TRIGGER: return Gamepad_LeftTrigger;
                 * case GLFW_GAMEPAD_AXIS_RIGHT_TRIGGER: return Gamepad_RightTrigger;
                 */
            default: return 0xDEADBEEF;
        }
    }
}
