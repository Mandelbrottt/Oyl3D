#include "InputCodes.h"

auto
std::formatter<Oyl::Input::KeyboardKey>::format(
	Oyl::Input::KeyboardKey a_key,
	std::format_context& a_ctx
) const -> decltype(a_ctx.out())
{
	return format_to(a_ctx.out(), "{}", static_cast<KeyboardKeyUnderlying>(a_key));
}

auto
std::formatter<Oyl::Input::MouseButton>::format(
	Oyl::Input::MouseButton a_button,
	std::format_context& a_ctx
) const -> decltype(a_ctx.out())
{
	return format_to(a_ctx.out(), "{}", static_cast<MouseButtonUnderlying>(a_button));
}

auto
std::formatter<Oyl::Input::GamePadButton>::format(
	Oyl::Input::GamePadButton a_button,
	std::format_context& a_ctx
) const -> decltype(a_ctx.out())
{
	return format_to(a_ctx.out(), "{}", static_cast<GamePadButtonUnderlying>(a_button));
}

auto
std::formatter<Oyl::Input::GamePadAxis>::format(
	Oyl::Input::GamePadAxis a_axis,
	std::format_context& a_ctx
) const -> decltype(a_ctx.out())
{
	return format_to(a_ctx.out(), "{}", static_cast<GamePadAxisUnderlying>(a_axis));
}
