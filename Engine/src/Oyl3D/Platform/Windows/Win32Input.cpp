#include "oylpch.h"
#include "Win32Input.h"

#include "Oyl3D/App/Application.h"

#include <GLFW/glfw3.h>

namespace oyl
{
    Input* Input::s_instance = new Win32Input();

    bool Win32Input::isKeyPressedImpl(Key keyCode)
    {
        auto window = static_cast<GLFWwindow*>(Application::get().getWindow().getNativeWindow());
        auto state  = glfwGetKey(window, OylToGlfwKeyCode(keyCode));
        return state == GLFW_PRESS || state == GLFW_REPEAT;
    }

    bool Win32Input::isMousePressedImpl(Mouse button)
    {
        auto window = static_cast<GLFWwindow*>(Application::get().getWindow().getNativeWindow());
        auto state  = glfwGetMouseButton(window, OylToGlfwMouseCode(button));
        return state == GLFW_PRESS;
    }

    bool Win32Input::isGamepadButtonPressedImpl(Gamepad button, uint gid)
    {
        // OYL_ASSERT(glfwJoystickPresent(gid) == GLFW_TRUE, "Joystick should be present!");

        static GLFWgamepadstate state;

        if (glfwJoystickIsGamepad(gid))
        {
            glfwGetGamepadState(gid, &state);
            return state.buttons[OylToGlfwGamepadCode(button)] == GLFW_PRESS;
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
        // OYL_ASSERT(glfwJoystickPresent(gid) == GLFW_TRUE, "Joystick should be present!");

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
        //OYL_ASSERT(glfwJoystickPresent(gid) == GLFW_TRUE, "Joystick should be present!");

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
        //OYL_ASSERT(glfwJoystickPresent(gid) == GLFW_TRUE, "Joystick should be present!");

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
        //OYL_ASSERT(glfwJoystickPresent(gid) == GLFW_TRUE, "Joystick should be present!");

        static GLFWgamepadstate state;

        if (glfwJoystickIsGamepad(gid))
        {
            glfwGetGamepadState(gid, &state);
            float pos = state.axes[GLFW_GAMEPAD_AXIS_RIGHT_TRIGGER];
            return pos;
        }
        return 0.0f;
    }

    uint OylToGlfwKeyCode(Key code)
    {
        switch (code)
        {
            case Key::Space: return GLFW_KEY_SPACE;
            case Key::Apostrophe: return GLFW_KEY_APOSTROPHE;
            case Key::Comma: return GLFW_KEY_COMMA;
            case Key::Minus: return GLFW_KEY_MINUS;
            case Key::Period: return GLFW_KEY_PERIOD;
            case Key::Slash: return GLFW_KEY_SLASH;
            case Key::Alpha0: return GLFW_KEY_0;
            case Key::Alpha1: return GLFW_KEY_1;
            case Key::Alpha2: return GLFW_KEY_2;
            case Key::Alpha3: return GLFW_KEY_3;
            case Key::Alpha4: return GLFW_KEY_4;
            case Key::Alpha5: return GLFW_KEY_5;
            case Key::Alpha6: return GLFW_KEY_6;
            case Key::Alpha7: return GLFW_KEY_7;
            case Key::Alpha8: return GLFW_KEY_8;
            case Key::Alpha9: return GLFW_KEY_9;
            case Key::Semicolon: return GLFW_KEY_SEMICOLON;
            case Key::Equal: return GLFW_KEY_EQUAL;
            case Key::A: return GLFW_KEY_A;
            case Key::B: return GLFW_KEY_B;
            case Key::C: return GLFW_KEY_C;
            case Key::D: return GLFW_KEY_D;
            case Key::E: return GLFW_KEY_E;
            case Key::F: return GLFW_KEY_F;
            case Key::G: return GLFW_KEY_G;
            case Key::H: return GLFW_KEY_H;
            case Key::I: return GLFW_KEY_I;
            case Key::J: return GLFW_KEY_J;
            case Key::K: return GLFW_KEY_K;
            case Key::L: return GLFW_KEY_L;
            case Key::M: return GLFW_KEY_M;
            case Key::N: return GLFW_KEY_N;
            case Key::O: return GLFW_KEY_O;
            case Key::P: return GLFW_KEY_P;
            case Key::Q: return GLFW_KEY_Q;
            case Key::R: return GLFW_KEY_R;
            case Key::S: return GLFW_KEY_S;
            case Key::T: return GLFW_KEY_T;
            case Key::U: return GLFW_KEY_U;
            case Key::V: return GLFW_KEY_V;
            case Key::W: return GLFW_KEY_W;
            case Key::X: return GLFW_KEY_X;
            case Key::Y: return GLFW_KEY_Y;
            case Key::Z: return GLFW_KEY_Z;
            case Key::LeftBracket: return GLFW_KEY_LEFT_BRACKET;
            case Key::Backslash: return GLFW_KEY_BACKSLASH;
            case Key::RightBracket: return GLFW_KEY_RIGHT_BRACKET;
            case Key::Grave: return GLFW_KEY_GRAVE_ACCENT;
            case Key::World1: return GLFW_KEY_WORLD_1;
            case Key::World2: return GLFW_KEY_WORLD_2;

            case Key::Escape: return GLFW_KEY_ESCAPE;
            case Key::Enter: return GLFW_KEY_ENTER;
            case Key::Tab: return GLFW_KEY_TAB;
            case Key::Backspace: return GLFW_KEY_BACKSPACE;
            case Key::Insert: return GLFW_KEY_INSERT;
            case Key::Delete: return GLFW_KEY_DELETE;
            case Key::Right: return GLFW_KEY_RIGHT;
            case Key::Left: return GLFW_KEY_LEFT;
            case Key::Down: return GLFW_KEY_DOWN;
            case Key::Up: return GLFW_KEY_UP;
            case Key::PageUp: return GLFW_KEY_PAGE_UP;
            case Key::PageDown: return GLFW_KEY_PAGE_DOWN;
            case Key::Home: return GLFW_KEY_HOME;
            case Key::End: return GLFW_KEY_END;
            case Key::CapsLock: return GLFW_KEY_CAPS_LOCK;
            case Key::ScrollLock: return GLFW_KEY_SCROLL_LOCK;
            case Key::NumLock: return GLFW_KEY_NUM_LOCK;
            case Key::PrintScreen: return GLFW_KEY_PRINT_SCREEN;
            case Key::Pause: return GLFW_KEY_PAUSE;
            case Key::F1: return GLFW_KEY_F1;
            case Key::F2: return GLFW_KEY_F2;
            case Key::F3: return GLFW_KEY_F3;
            case Key::F4: return GLFW_KEY_F4;
            case Key::F5: return GLFW_KEY_F5;
            case Key::F6: return GLFW_KEY_F6;
            case Key::F7: return GLFW_KEY_F7;
            case Key::F8: return GLFW_KEY_F8;
            case Key::F9: return GLFW_KEY_F9;
            case Key::F10: return GLFW_KEY_F10;
            case Key::F11: return GLFW_KEY_F11;
            case Key::F12: return GLFW_KEY_F12;
            case Key::F13: return GLFW_KEY_F13;
            case Key::F14: return GLFW_KEY_F14;
            case Key::F15: return GLFW_KEY_F15;
            case Key::F16: return GLFW_KEY_F16;
            case Key::F17: return GLFW_KEY_F17;
            case Key::F18: return GLFW_KEY_F18;
            case Key::F19: return GLFW_KEY_F19;
            case Key::F20: return GLFW_KEY_F20;
            case Key::F21: return GLFW_KEY_F21;
            case Key::F22: return GLFW_KEY_F22;
            case Key::F23: return GLFW_KEY_F23;
            case Key::F24: return GLFW_KEY_F24;
            case Key::F25: return GLFW_KEY_F25;
            case Key::Keypad0: return GLFW_KEY_KP_0;
            case Key::Keypad1: return GLFW_KEY_KP_1;
            case Key::Keypad2: return GLFW_KEY_KP_2;
            case Key::Keypad3: return GLFW_KEY_KP_3;
            case Key::Keypad4: return GLFW_KEY_KP_4;
            case Key::Keypad5: return GLFW_KEY_KP_5;
            case Key::Keypad6: return GLFW_KEY_KP_6;
            case Key::Keypad7: return GLFW_KEY_KP_7;
            case Key::Keypad8: return GLFW_KEY_KP_8;
            case Key::Keypad9: return GLFW_KEY_KP_9;
            case Key::KeypadDecimal: return GLFW_KEY_KP_DECIMAL;
            case Key::KeypadDivide: return GLFW_KEY_KP_DIVIDE;
            case Key::KeypadTimes: return GLFW_KEY_KP_MULTIPLY;
            case Key::KeypadMinus: return GLFW_KEY_KP_SUBTRACT;
            case Key::KeypadPlus: return GLFW_KEY_KP_ADD;
            case Key::KeypadEnter: return GLFW_KEY_KP_ENTER;
            case Key::KeypadEqual: return GLFW_KEY_KP_EQUAL;
            case Key::LeftShift: return GLFW_KEY_LEFT_SHIFT;
            case Key::LeftControl: return GLFW_KEY_LEFT_CONTROL;
            case Key::LeftAlt: return GLFW_KEY_LEFT_ALT;
            case Key::LeftSuper: return GLFW_KEY_LEFT_SUPER;
            case Key::RightShift: return GLFW_KEY_RIGHT_SHIFT;
            case Key::RightControl: return GLFW_KEY_RIGHT_CONTROL;
            case Key::RightAlt: return GLFW_KEY_RIGHT_ALT;
            case Key::RightSuper: return GLFW_KEY_RIGHT_SUPER;
            case Key::Menu: return GLFW_KEY_MENU;
        }
        return GLFW_KEY_UNKNOWN;
    }

    uint OylToGlfwMouseCode(Mouse code)
    {
        switch (code)
        {
            case Mouse::M1: return GLFW_MOUSE_BUTTON_1;
            case Mouse::M2: return GLFW_MOUSE_BUTTON_2;
            case Mouse::M3: return GLFW_MOUSE_BUTTON_3;
            case Mouse::M4: return GLFW_MOUSE_BUTTON_4;
            case Mouse::M5: return GLFW_MOUSE_BUTTON_5;
            case Mouse::M6: return GLFW_MOUSE_BUTTON_6;
            case Mouse::M7: return GLFW_MOUSE_BUTTON_7;
            case Mouse::M8: return GLFW_MOUSE_BUTTON_8;
        }
        return GLFW_MOUSE_BUTTON_1;
    }

    uint OylToGlfwGamepadCode(Gamepad code)
    {
        switch (code)
        {
            case Gamepad::A: return GLFW_GAMEPAD_BUTTON_A;
            case Gamepad::B: return GLFW_GAMEPAD_BUTTON_B;
            case Gamepad::X: return GLFW_GAMEPAD_BUTTON_X;
            case Gamepad::Y: return GLFW_GAMEPAD_BUTTON_Y;
            case Gamepad::LeftBumper: return GLFW_GAMEPAD_BUTTON_LEFT_BUMPER;
            case Gamepad::RightBumper: return GLFW_GAMEPAD_BUTTON_RIGHT_BUMPER;
            case Gamepad::Back: return GLFW_GAMEPAD_BUTTON_BACK;
            case Gamepad::Start: return GLFW_GAMEPAD_BUTTON_START;            
            case Gamepad::Guide: return GLFW_GAMEPAD_BUTTON_GUIDE;
            case Gamepad::LeftThumb: return GLFW_GAMEPAD_BUTTON_LEFT_THUMB;
            case Gamepad::RightThumb: return GLFW_GAMEPAD_BUTTON_RIGHT_THUMB;
            case Gamepad::DPadUp: return GLFW_GAMEPAD_BUTTON_DPAD_UP;
            case Gamepad::DPadRight: return GLFW_GAMEPAD_BUTTON_DPAD_RIGHT;
            case Gamepad::DPadDown: return GLFW_GAMEPAD_BUTTON_DPAD_DOWN;
            case Gamepad::DPadLeft: return GLFW_GAMEPAD_BUTTON_DPAD_LEFT;
        }
        return -1;
    }

    Key glfwToOylKeyCode(uint code)
    {
        switch (code)
        {
            case GLFW_KEY_SPACE: return Key::Space;
            case GLFW_KEY_APOSTROPHE: return Key::Apostrophe;
            case GLFW_KEY_COMMA: return Key::Comma;
            case GLFW_KEY_MINUS: return Key::Minus;
            case GLFW_KEY_PERIOD: return Key::Period;
            case GLFW_KEY_SLASH: return Key::Slash;
            case GLFW_KEY_0: return Key::Alpha0;
            case GLFW_KEY_1: return Key::Alpha1;
            case GLFW_KEY_2: return Key::Alpha2;
            case GLFW_KEY_3: return Key::Alpha3;
            case GLFW_KEY_4: return Key::Alpha4;
            case GLFW_KEY_5: return Key::Alpha5;
            case GLFW_KEY_6: return Key::Alpha6;
            case GLFW_KEY_7: return Key::Alpha7;
            case GLFW_KEY_8: return Key::Alpha8;
            case GLFW_KEY_9: return Key::Alpha9;
            case GLFW_KEY_SEMICOLON: return Key::Semicolon;
            case GLFW_KEY_EQUAL: return Key::Equal;
            case GLFW_KEY_A: return Key::A;
            case GLFW_KEY_B: return Key::B;
            case GLFW_KEY_C: return Key::C;
            case GLFW_KEY_D: return Key::D;
            case GLFW_KEY_E: return Key::E;
            case GLFW_KEY_F: return Key::F;
            case GLFW_KEY_G: return Key::G;
            case GLFW_KEY_H: return Key::H;
            case GLFW_KEY_I: return Key::I;
            case GLFW_KEY_J: return Key::J;
            case GLFW_KEY_K: return Key::K;
            case GLFW_KEY_L: return Key::L;
            case GLFW_KEY_M: return Key::M;
            case GLFW_KEY_N: return Key::N;
            case GLFW_KEY_O: return Key::O;
            case GLFW_KEY_P: return Key::P;
            case GLFW_KEY_Q: return Key::Q;
            case GLFW_KEY_R: return Key::R;
            case GLFW_KEY_S: return Key::S;
            case GLFW_KEY_T: return Key::T;
            case GLFW_KEY_U: return Key::U;
            case GLFW_KEY_V: return Key::V;
            case GLFW_KEY_W: return Key::W;
            case GLFW_KEY_X: return Key::X;
            case GLFW_KEY_Y: return Key::Y;
            case GLFW_KEY_Z: return Key::Z;
            case GLFW_KEY_LEFT_BRACKET: return Key::LeftBracket;
            case GLFW_KEY_BACKSLASH: return Key::Backslash;
            case GLFW_KEY_RIGHT_BRACKET: return Key::RightBracket;
            case GLFW_KEY_GRAVE_ACCENT: return Key::Grave;
            case GLFW_KEY_WORLD_1: return Key::World1;
            case GLFW_KEY_WORLD_2: return Key::World2;

            case GLFW_KEY_ESCAPE: return Key::Escape;
            case GLFW_KEY_ENTER: return Key::Enter;
            case GLFW_KEY_TAB: return Key::Tab;
            case GLFW_KEY_BACKSPACE: return Key::Backspace;
            case GLFW_KEY_INSERT: return Key::Insert;
            case GLFW_KEY_DELETE: return Key::Delete;
            case GLFW_KEY_RIGHT: return Key::Right;
            case GLFW_KEY_LEFT: return Key::Left;
            case GLFW_KEY_DOWN: return Key::Down;
            case GLFW_KEY_UP: return Key::Up;
            case GLFW_KEY_PAGE_UP: return Key::PageUp;
            case GLFW_KEY_PAGE_DOWN: return Key::PageDown;
            case GLFW_KEY_HOME: return Key::Home;
            case GLFW_KEY_END: return Key::End;
            case GLFW_KEY_CAPS_LOCK: return Key::CapsLock;
            case GLFW_KEY_SCROLL_LOCK: return Key::ScrollLock;
            case GLFW_KEY_NUM_LOCK: return Key::NumLock;
            case GLFW_KEY_PRINT_SCREEN: return Key::PrintScreen;
            case GLFW_KEY_PAUSE: return Key::Pause;
            case GLFW_KEY_F1: return Key::F1;
            case GLFW_KEY_F2: return Key::F2;
            case GLFW_KEY_F3: return Key::F3;
            case GLFW_KEY_F4: return Key::F4;
            case GLFW_KEY_F5: return Key::F5;
            case GLFW_KEY_F6: return Key::F6;
            case GLFW_KEY_F7: return Key::F7;
            case GLFW_KEY_F8: return Key::F8;
            case GLFW_KEY_F9: return Key::F9;
            case GLFW_KEY_F10: return Key::F10;
            case GLFW_KEY_F11: return Key::F11;
            case GLFW_KEY_F12: return Key::F12;
            case GLFW_KEY_F13: return Key::F13;
            case GLFW_KEY_F14: return Key::F14;
            case GLFW_KEY_F15: return Key::F15;
            case GLFW_KEY_F16: return Key::F16;
            case GLFW_KEY_F17: return Key::F17;
            case GLFW_KEY_F18: return Key::F18;
            case GLFW_KEY_F19: return Key::F19;
            case GLFW_KEY_F20: return Key::F20;
            case GLFW_KEY_F21: return Key::F21;
            case GLFW_KEY_F22: return Key::F22;
            case GLFW_KEY_F23: return Key::F23;
            case GLFW_KEY_F24: return Key::F24;
            case GLFW_KEY_F25: return Key::F25;
            case GLFW_KEY_KP_0: return Key::Keypad0;
            case GLFW_KEY_KP_1: return Key::Keypad1;
            case GLFW_KEY_KP_2: return Key::Keypad2;
            case GLFW_KEY_KP_3: return Key::Keypad3;
            case GLFW_KEY_KP_4: return Key::Keypad4;
            case GLFW_KEY_KP_5: return Key::Keypad5;
            case GLFW_KEY_KP_6: return Key::Keypad6;
            case GLFW_KEY_KP_7: return Key::Keypad7;
            case GLFW_KEY_KP_8: return Key::Keypad8;
            case GLFW_KEY_KP_9: return Key::Keypad9;
            case GLFW_KEY_KP_DECIMAL: return Key::KeypadDecimal;
            case GLFW_KEY_KP_DIVIDE: return Key::KeypadDivide;
            case GLFW_KEY_KP_MULTIPLY: return Key::KeypadTimes;
            case GLFW_KEY_KP_SUBTRACT: return Key::KeypadMinus;
            case GLFW_KEY_KP_ADD: return Key::KeypadPlus;
            case GLFW_KEY_KP_ENTER: return Key::KeypadEnter;
            case GLFW_KEY_KP_EQUAL: return Key::KeypadEqual;
            case GLFW_KEY_LEFT_SHIFT: return Key::LeftShift;
            case GLFW_KEY_LEFT_CONTROL: return Key::LeftControl;
            case GLFW_KEY_LEFT_ALT: return Key::LeftAlt;
            case GLFW_KEY_LEFT_SUPER: return Key::LeftSuper;
            case GLFW_KEY_RIGHT_SHIFT: return Key::RightShift;
            case GLFW_KEY_RIGHT_CONTROL: return Key::RightControl;
            case GLFW_KEY_RIGHT_ALT: return Key::RightAlt;
            case GLFW_KEY_RIGHT_SUPER: return Key::RightSuper;
            case GLFW_KEY_MENU: return Key::Menu;
        }
        return Key::Unknown;
    }

    Mouse glfwToOylMouseCode(uint code)
    {
        switch (code)
        {
            case GLFW_MOUSE_BUTTON_1: return Mouse::M1;
            case GLFW_MOUSE_BUTTON_2: return Mouse::M2;
            case GLFW_MOUSE_BUTTON_3: return Mouse::M3;
            case GLFW_MOUSE_BUTTON_4: return Mouse::M4;
            case GLFW_MOUSE_BUTTON_5: return Mouse::M5;
            case GLFW_MOUSE_BUTTON_6: return Mouse::M6;
            case GLFW_MOUSE_BUTTON_7: return Mouse::M7;
            case GLFW_MOUSE_BUTTON_8: return Mouse::M8;
        }
        return Mouse::Unknown;
    }

    Gamepad glfwToOylGamepadCode(uint code)
    {
        switch (code)
        {
            case GLFW_GAMEPAD_BUTTON_A: return Gamepad::A;
            case GLFW_GAMEPAD_BUTTON_B: return Gamepad::B;
            case GLFW_GAMEPAD_BUTTON_X: return Gamepad::X;
            case GLFW_GAMEPAD_BUTTON_Y: return Gamepad::Y;
            case GLFW_GAMEPAD_BUTTON_LEFT_BUMPER: return Gamepad::LeftBumper;
            case GLFW_GAMEPAD_BUTTON_RIGHT_BUMPER: return Gamepad::RightBumper;
            case GLFW_GAMEPAD_BUTTON_BACK: return Gamepad::Back;
            case GLFW_GAMEPAD_BUTTON_START: return Gamepad::Start;
            
            case GLFW_GAMEPAD_BUTTON_GUIDE: return Gamepad::Guide;
            case GLFW_GAMEPAD_BUTTON_LEFT_THUMB: return Gamepad::LeftThumb;
            case GLFW_GAMEPAD_BUTTON_RIGHT_THUMB: return Gamepad::RightThumb;
            case GLFW_GAMEPAD_BUTTON_DPAD_UP: return Gamepad::DPadUp;
            case GLFW_GAMEPAD_BUTTON_DPAD_RIGHT: return Gamepad::DPadRight;
            case GLFW_GAMEPAD_BUTTON_DPAD_DOWN: return Gamepad::DPadDown;
            case GLFW_GAMEPAD_BUTTON_DPAD_LEFT: return Gamepad::DPadLeft;
        }
        return Gamepad::Unknown;
    }
}
