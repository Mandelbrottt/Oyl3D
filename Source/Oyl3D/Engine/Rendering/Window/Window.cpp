#include "Window.h"

namespace Oyl
{
	Window::Window() noexcept {}

	Window::Window(const WindowParams& a_params) noexcept
		: Window()
	{
		OYL_UNUSED(a_params);
	}

	Window::~Window() noexcept {}
}
