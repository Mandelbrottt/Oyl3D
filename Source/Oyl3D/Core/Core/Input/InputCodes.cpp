#include "InputCodes.h"

#if defined(_OYL_PACKAGE_SPDLOG)
#include <spdlog/fmt/bundled/format.h>

auto
fmt::formatter<Oyl::Input::KeyboardKey>::format(
	Oyl::Input::KeyboardKey a_key,
	format_context& a_ctx
) const -> decltype(a_ctx.out())
{
	// TODO: Use Reflection
	return format_to(a_ctx.out(), "{}", static_cast<KeyboardKeyUnderlying>(a_key));
}

auto
fmt::formatter<Oyl::Input::MouseButton>::format(
	Oyl::Input::MouseButton a_button,
	format_context& a_ctx
) const -> decltype(a_ctx.out())
{
	// TODO: Use Reflection
	return format_to(a_ctx.out(), "{}", static_cast<MouseButtonUnderlying>(a_button));
}

auto
fmt::formatter<Oyl::Input::GamePadButton>::format(
	Oyl::Input::GamePadButton a_button,
	format_context& a_ctx
) const -> decltype(a_ctx.out())
{
	// TODO: Use Reflection
	return format_to(a_ctx.out(), "{}", static_cast<GamePadButtonUnderlying>(a_button));
}

auto
fmt::formatter<Oyl::Input::GamePadAxis>::format(
	Oyl::Input::GamePadAxis a_button,
	format_context& a_ctx
) const -> decltype(a_ctx.out())
{
	// TODO: Use Reflection
	return format_to(a_ctx.out(), "{}", static_cast<GamePadAxisUnderlying>(a_button));
}
#endif
