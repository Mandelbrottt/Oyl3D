#pragma once

namespace oyl
{
    enum class Key : i32
    {
        Unknown = -1,

        // Printable Keys
        Space = 32,
        Apostrophe = 39,    /* ' */
        Comma = 44,         /* , */
        Minus,              /* - */
        Period,             /* . */
        Slash,              /* / */
        Alpha0,
        Alpha1,
        Alpha2,
        Alpha3,
        Alpha4,
        Alpha5,
        Alpha6,
        Alpha7,
        Alpha8,
        Alpha9,
        Semicolon = 59,     /* ; */
        Equal = 61,         /* = */
        A = 65,
        B,
        C,
        D,
        E,
        F,
        G,
        H,
        I,
        J,
        K,
        L,
        M,
        N,
        O,
        P,
        Q,
        R,
        S,
        T,
        U,
        V,
        W,
        X,
        Y,
        Z,
        LeftBracket,		/* [ */
        Backslash,			/* \ */
        RightBracket,		/* ] */
        Grave = 96,		    /* ` */
        World1 = 161,		/* non-US #1 */
        World2,				/* non-US #2 */

        // Function Keys
        Escape,
        Enter,
        Tab,
        Backspace,
        Insert,
        Delete,
        Right,
        Left,
        Down,
        Up,
        PageUp,
        PageDown,
        Home,
        End,
        CapsLock,
        ScrollLock,
        NumLock,
        PrintScreen,
        Pause,
        F1,
        F2,
        F3,
        F4,
        F5,
        F6,
        F7,
        F8,
        F9,
        F10,
        F11,
        F12,
        F13,
        F14,
        F15,
        F16,
        F17,
        F18,
        F19,
        F20,
        F21,
        F22,
        F23,
        F24,
        F25,
        Keypad0,
        Keypad1,
        Keypad2,
        Keypad3,
        Keypad4,
        Keypad5,
        Keypad6,
        Keypad7,
        Keypad8,
        Keypad9,
        KeypadDecimal,
        KeypadDivide,
        KeypadTimes,
        KeypadMinus,
        KeypadPlus,
        KeypadEnter,
        KeypadEqual,
        LeftShift,
        LeftControl,
        LeftAlt,
        LeftSuper, LeftWindows = LeftSuper, LeftCommand = LeftSuper,
        RightShift,
        RightControl,
        RightAlt,
        RightSuper, RightWindows = RightSuper, RightCommand = RightSuper,
        Menu,
    };

    enum class Mouse : i32
    {
        Unknown = -1,
        M1,
        M2,
        M3,
        M4,
        M5,
        M6,
        M7,
        M8,
        Left    = M1,
        Right   = M2,
        Middle  = M3,
        Back    = M4,
        Forward = M5,
    };

    enum class Gamepad : i32
    {
        Unknown = -1,

        A,
        B,
        X,
        Y,
        LeftBumper,
        RightBumper,
        Back, Select = Back,
        Start,
        Guide,
        LeftThumb,
        RightThumb,
        DPadUp,
        DPadRight,
        DPadDown,
        DPadLeft,

        LeftStickX = A,
        LeftStickY,
        RightStickX,
        RightStickY,
        LeftTrigger,
        RightTrigger,
        LeftStick,
        RightStick,
    };
}