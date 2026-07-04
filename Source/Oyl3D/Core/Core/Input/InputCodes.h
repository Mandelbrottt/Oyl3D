#pragma once
#include "Core/TypeTraits.h"

namespace Oyl::Input
{
	enum KeyboardKey : int16
	{
		KB_0 = '0',
		KB_1,
		KB_2,
		KB_3,
		KB_4,
		KB_5,
		KB_6,
		KB_7,
		KB_8,
		KB_9,
		KB_NumbersFirst = KB_0,
		KB_NumbersLast  = KB_9,

		KB_A = 'A',
		KB_B,
		KB_C,
		KB_D,
		KB_E,
		KB_F,
		KB_G,
		KB_H,
		KB_I,
		KB_J,
		KB_K,
		KB_L,
		KB_M,
		KB_N,
		KB_O,
		KB_P,
		KB_Q,
		KB_R,
		KB_S,
		KB_T,
		KB_U,
		KB_V,
		KB_W,
		KB_X,
		KB_Y,
		KB_Z,
		KB_LettersFirst = KB_A,
		KB_LettersLast  = KB_Z,

		KB_Space        = ' ',  /*   */
		KB_Apostrophe   = '\'', /* ' */
		KB_Comma        = ',',  /* , */
		KB_Minus        = '-',  /* - */
		KB_Period       = '.',  /* . */
		KB_Slash        = '/',  /* / */
		KB_Semicolon    = ';',  /* ; */
		KB_Equal        = '=',  /* = */
		KB_Backslash    = '\\', /* \ */
		KB_LeftBracket  = '[',  /* [ */
		KB_RightBracket = ']',  /* ] */
		KB_Grave        = '`',  /* ` */
		KB_SymbolsFirst = KB_Space,
		KB_SymbolsLast  = KB_Grave,

		KB_Numpad1 = 1 << 8,
		KB_Numpad2,
		KB_Numpad3,
		KB_Numpad4,
		KB_Numpad5,
		KB_Numpad6,
		KB_Numpad7,
		KB_Numpad8,
		KB_Numpad9,
		KB_Numpad0,
		KB_NumpadAdd,
		KB_NumpadSubtract,
		KB_NumpadMultiply,
		KB_NumpadDivide,
		KB_NumpadEnter,
		KB_NumpadDecimal,
		KB_NumLock,
		KB_NumpadFirst = KB_Numpad1,
		KB_NumpadLast  = KB_NumLock,

		KB_F1,
		KB_F2,
		KB_F3,
		KB_F4,
		KB_F5,
		KB_F6,
		KB_F7,
		KB_F8,
		KB_F9,
		KB_F10,
		KB_F11,
		KB_F12,
		KB_F13,
		KB_F14,
		KB_F15,
		KB_F16,
		KB_F17,
		KB_F18,
		KB_F19,
		KB_F20,
		KB_F21,
		KB_F22,
		KB_F23,
		KB_F24,
		KB_F25,
		KB_FunctionKeysFirst = KB_F1,
		KB_FunctionKeysLast  = KB_F25,

		KB_Escape,
		KB_Tab,
		KB_CapsLock,
		KB_LeftShift,
		KB_RightShift,
		KB_LeftCtrl,
		KB_RightCtrl,
		KB_LeftAlt,
		KB_RightAlt,
		KB_LeftSuper,
		KB_RightSuper,
		KB_Menu,
		KB_Backspace,
		KB_Enter,
		KB_Up,
		KB_Down,
		KB_Left,
		KB_Right,
		KB_Insert,
		KB_Delete,
		KB_Home,
		KB_End,
		KB_PageUp,
		KB_PageDown,
		KB_PrintScreen,
		KB_ScrollLock,
		KB_PauseBreak,
		KB_SpecialFirst = KB_Escape,
		KB_SpecialLast  = KB_End,

		KB_First = KB_NumbersFirst,
		KB_Last  = KB_SpecialLast,
	};

	enum MouseButton : int8
	{
		MB_Button1,
		MB_Button2,
		MB_Button3,
		MB_Button4,
		MB_Button5,
		MB_Button6,
		MB_Button7,
		MB_Button8,

		MB_Left    = MB_Button1,
		MB_Right   = MB_Button2,
		MB_Middle  = MB_Button3,
		MB_Forward = MB_Button4,
		MB_Back    = MB_Button5,

		MB_First = MB_Button1,
		MB_Last  = MB_Button8,
	};

	enum GamePadButton : int8
	{
		GP_North,
		GP_South,
		GP_East,
		GP_West,
		GP_LeftShoulder,
		GP_RightShoulder,
		GP_DPadUp,
		GP_DPadDown,
		GP_DPadLeft,
		GP_DPadRight,
		GP_Start,
		GP_Back,
		GP_Share,
		GP_Home,
		GP_LeftStickButton,
		GP_RightStickButton,

		GP_ButtonFirst = GP_North,
		GP_ButtonLast  = GP_RightStickButton,
	};

	enum GamePadAxis : int8
	{
		GP_LeftTrigger,
		GP_RightTrigger,
		GP_LeftStick,
		GP_RightStick,

		GP_AxisFirst = GP_LeftTrigger,
		GP_AxisLast  = GP_RightStick,
	};
}

#if defined(_OYL_PACKAGE_SPDLOG)
#include <spdlog/fmt/bundled/core.h>

template<>
struct fmt::formatter<Oyl::Input::KeyboardKey> : formatter<std::string>
{
	using KeyboardKeyUnderlying = Oyl::Traits::UnderlyingType_T<Oyl::Input::KeyboardKey>;

	auto
	format(
		Oyl::Input::KeyboardKey a_key,
		format_context& a_ctx
	) const -> decltype(a_ctx.out());
};

template<>
struct fmt::formatter<Oyl::Input::MouseButton> : formatter<std::string>
{
	using MouseButtonUnderlying = Oyl::Traits::UnderlyingType_T<Oyl::Input::MouseButton>;

	auto
	format(
		Oyl::Input::MouseButton a_button,
		format_context& a_ctx
	) const -> decltype(a_ctx.out());
};

template<>
struct fmt::formatter<Oyl::Input::GamePadButton> : formatter<std::string>
{
	using GamePadButtonUnderlying = Oyl::Traits::UnderlyingType_T<Oyl::Input::GamePadButton>;

	auto
	format(
		Oyl::Input::GamePadButton a_button,
		format_context& a_ctx
	) const -> decltype(a_ctx.out());
};

template<>
struct fmt::formatter<Oyl::Input::GamePadAxis> : formatter<std::string>
{
	using GamePadAxisUnderlying = Oyl::Traits::UnderlyingType_T<Oyl::Input::GamePadAxis>;

	auto
	format(
		Oyl::Input::GamePadAxis a_button,
		format_context& a_ctx
	) const -> decltype(a_ctx.out());
};
#endif
