#pragma once
// TODO: Maybe come up with a better file name

#include "Typedefs.h"

namespace oyl
{
    // TODO: Make enums more verbose
    enum OylEnum : u32
    {
        None = 0,

        // Fullscreen Types //
        Windowed = 1,
        Borderless,
        Fullscreen = Borderless,

        // Event Types //
        TypeWindowClosed = 1, TypeWindowMoved, TypeWindowResized, TypeWindowFocused,
        TypeViewportResized, TypeViewportHandleChanged,
        TypeKeyPressed, TypeKeyTyped, TypeKeyReleased,
        TypeMousePressed, TypeMouseReleased, TypeMouseMoved, TypeMouseScrolled,
        TypeGamepadConnected, TypeGamepadDisconnected,
        TypeGamepadButtonPressed, TypeGamepadButtonReleased,
        TypeGamepadStickMoved, TypeGamepadTriggerPressed,
        TypeGamepadVibration, // TODO: Rename to GamepadVibrationRequest
        TypeCursorStateRequest,
        TypePhysicsCollision, TypePhysicsResetWorld,
        TypeEditorViewportResized, TypeEditorViewportHandleChanged, TypeEditorEntitySelected,
        EventTypeCustomStart,

        // Event Categories //
        CategoryApplication = 1,
        CategoryWindow,
        CategoryViewport,
        CategoryKeyboard,
        CategoryMouse,
        CategoryGamepad,
        CategoryGamepadVibration,
        CategoryCursorStateRequest,
        CategoryAudio,
        CategoryPhysics,
        CategoryEditor,
        CategoryCustomStart,

        // Shader Data Types //
        UInt = 1, Uint2, Uint3, Uint4,
        Int, Int2, Int3, Int4,
        Float, Float2, Float3, Float4,
        Mat3, Mat4,
        Bool,

        // Texture Formats //
        RGB8 = 1, RGBA8,

        // Texture Filters //
        Nearest = 1, Linear,

        // Texture Wrappings //
        Clamp = 1, Mirror, Repeat,

        // Shader Types //
        VertexShader = 1,
        TessControlShader,
        TessEvaluationShader,
        GeometryShader,
        PixelShader,
        FragmentShader = PixelShader,
        NumShaderTypes = PixelShader,

        // APIs //
        API_OpenGL = 1,
        API_DirectX,

        // Printable Keys //
        Key_Space = 32,
        Key_Apostrophe = 39,    /* ' */
        Key_Comma = 44,         /* , */
        Key_Minus,              /* - */
        Key_Period,             /* . */
        Key_Slash,              /* / */
        Key_0,
        Key_1,
        Key_2,
        Key_3,
        Key_4,
        Key_5,
        Key_6,
        Key_7,
        Key_8,
        Key_9,
        Key_Semicolon = 59,     /* ; */
        Key_Equal = 61,         /* = */
        Key_A = 65,
        Key_B,
        Key_C,
        Key_D,
        Key_E,
        Key_F,
        Key_G,
        Key_H,
        Key_I,
        Key_J,
        Key_K,
        Key_L,
        Key_M,
        Key_N,
        Key_O,
        Key_P,
        Key_Q,
        Key_R,
        Key_S,
        Key_T,
        Key_U,
        Key_V,
        Key_W,
        Key_X,
        Key_Y,
        Key_Z,
        Key_LeftBracket,		/* [ */
        Key_Backslash,			/* \ */
        Key_RightBracket,		/* ] */
        Key_Grave = 96,		    /* ` */
        Key_World1 = 161,		/* non-US #1 */
        Key_World2,				/* non-US #2 */

        // Function Keys
        Key_Escape,
        Key_Enter,
        Key_Tab,
        Key_Backspace,
        Key_Insert,
        Key_Delete,
        Key_Right,
        Key_Left,
        Key_Down,
        Key_Up,
        Key_PageUp,
        Key_PageDown,
        Key_Home,
        Key_End,
        Key_CapsLock,
        Key_ScrollLock,
        Key_NumLock,
        Key_PrintScreen,
        Key_Pause,
        Key_F1,
        Key_F2,
        Key_F3,
        Key_F4,
        Key_F5,
        Key_F6,
        Key_F7,
        Key_F8,
        Key_F9,
        Key_F10,
        Key_F11,
        Key_F12,
        Key_F13,
        Key_F14,
        Key_F15,
        Key_F16,
        Key_F17,
        Key_F18,
        Key_F19,
        Key_F20,
        Key_F21,
        Key_F22,
        Key_F23,
        Key_F24,
        Key_F25,
        Key_Keypad0,
        Key_Keypad1,
        Key_Keypad2,
        Key_Keypad3,
        Key_Keypad4,
        Key_Keypad5,
        Key_Keypad6,
        Key_Keypad7,
        Key_Keypad8,
        Key_Keypad9,
        Key_KeypadDecimal,
        Key_KeypadDivide,
        Key_KeypadTimes,
        Key_KeypadMinus,
        Key_KeypadPlus,
        Key_KeypadEnter,
        Key_KeypadEqual,
        Key_LeftShift,
        Key_LeftControl,
        Key_LeftAlt,
        Key_LeftSuper, Key_LeftWindows = Key_LeftSuper, Key_LeftCommand = Key_LeftSuper,
        Key_RightShift,
        Key_RightControl,
        Key_RightAlt,
        Key_RightSuper, Key_RightWindows = Key_RightSuper, Key_RightCommand = Key_RightSuper,
        Key_Menu,

        // Mouse Buttons //
        Mouse_1 = 1,
        Mouse_2,
        Mouse_3,
        Mouse_4,
        Mouse_5,
        Mouse_6,
        Mouse_7,
        Mouse_8,
        Mouse_Left = Mouse_1,
        Mouse_Right = Mouse_2,
        Mouse_Middle = Mouse_3,
        Mouse_Back = Mouse_4,
        Mouse_Forward = Mouse_5,

        Cursor_Enabled = 1,
        Cursor_Disabled,
        Cursor_Hidden,

        Gamepad_A = 1,
        Gamepad_B,
        Gamepad_X,
        Gamepad_Y,
        Gamepad_LeftBumper,
        Gamepad_RightBumper,
        Gamepad_Back, Gamepad_Select = Gamepad_Back,
        Gamepad_Start,
        Gamepad_Guide,
        Gamepad_LeftThumb,
        Gamepad_RightThumb,
        Gamepad_DPadUp,
        Gamepad_DPadRight,
        Gamepad_DPadDown,
        Gamepad_DPadLeft,

        Gamepad_LeftStickX = 1,
        Gamepad_LeftStickY,
        Gamepad_RightStickX,
        Gamepad_RightStickY,
        Gamepad_LeftTrigger,
        Gamepad_RightTrigger,
        Gamepad_LeftStick,
        Gamepad_RightStick,

        RigidBody_StaticPlane = 1,
        RigidBody_Sphere,
        RigidBody_Box,
        RigidBody_Capsule,
        RigidBody_Cylinder,
        RigidBody_Cone
    };
}
