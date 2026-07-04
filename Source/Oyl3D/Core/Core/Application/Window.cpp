#include "Window.h"

#include <vulkan/vulkan.h>

#include <GLFW/glfw3.h>

#include "Core/Events/EventDispatcher.h"
#include "Core/Logging/Logging.h"

namespace Oyl
{
	struct Window::Impl
	{
		std::string title;

		Vector2i position;
		Vector2i size;

		WindowStateFlags windowState;
		CursorState cursorState;

		GLFWwindow* glfwWindow = nullptr;

		EventDelegate onEventCallback;
	};

	Window::Window()
		: m_impl(new Impl) {}

	Window::Window(const WindowParams& a_params)
		: Window()
	{
		Init(a_params);
	}

	Window::Window(Window&& a_other) noexcept
	{
		*this = std::move(a_other);
	}

	Window&
	Window::operator=(Window&& a_other) noexcept
	{
		std::swap(m_impl, a_other.m_impl);
		return *this;
	}

	Window::~Window()
	{
		if (!m_impl)
			return;

		Destroy();

		std::memset(m_impl, 0, sizeof(Impl));
		delete m_impl;
		m_impl = nullptr;
	}

	void
	Window::Init(const WindowParams& a_params)
	{
		m_impl->title = a_params.title;

		m_impl->position = a_params.position;
		m_impl->size = a_params.size;

		m_impl->windowState = a_params.windowState;
		m_impl->cursorState = a_params.cursorState;

		auto monitor = glfwGetPrimaryMonitor();

		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		auto size = m_impl->size;
		m_impl->glfwWindow = glfwCreateWindow(
			size.x,
			size.y,
			m_impl->title.c_str(),
			m_impl->windowState & WS_Fullscreen ? monitor : nullptr,
			nullptr
		);

		auto* mode = glfwGetVideoMode(monitor);
		m_impl->position.x = mode->width / 2 - a_params.size.x / 2;
		m_impl->position.y = mode->height / 2 - a_params.size.y / 2;

		glfwSetWindowAspectRatio(m_impl->glfwWindow, 16, 9);

		if (m_impl->windowState ^ WS_Fullscreen)
		{
			glfwSetWindowMonitor(
				m_impl->glfwWindow,
				nullptr,
				m_impl->position.x,
				m_impl->position.y,
				m_impl->size.x,
				m_impl->size.y,
				mode->refreshRate
			);
		}

		glfwSetWindowUserPointer(m_impl->glfwWindow, m_impl);

		glfwSetWindowSizeCallback(
			m_impl->glfwWindow,
			[](GLFWwindow* a_window, int a_width, int a_height)
			{
				if (a_width <= 0 || a_height <= 0)
					return;

				Impl* impl = reinterpret_cast<Impl*>(glfwGetWindowUserPointer(a_window));
				impl->size = { a_width, a_height };

				WindowResizedEvent event;
				event.size = impl->size;
				impl->onEventCallback(event);
			}
		);
	}

	void
	Window::Destroy()
	{
		if (!m_impl->glfwWindow)
			return;

		// TODO: Destroy context alongside window
		glfwDestroyWindow(m_impl->glfwWindow);
		m_impl->glfwWindow = nullptr;
	}

	void
	Window::Update()
	{
		glfwPollEvents();
	}

	bool
	Window::IsValid() const
	{
		return m_impl->glfwWindow != nullptr;
	}

	void
	Window::SetEventCallback(EventDelegate a_delegate)
	{
		m_impl->onEventCallback = std::move(a_delegate);
	}

	Vector2i
	Window::GetSize() const
	{
		return m_impl->size;
	}

	void
	Window::SetSize(Vector2i a_size)
	{
		auto monitor = glfwGetWindowMonitor(m_impl->glfwWindow);
		auto mode = glfwGetVideoMode(monitor);

		glfwSetWindowMonitor(
			m_impl->glfwWindow,
			monitor,
			m_impl->position.x,
			m_impl->position.y,
			a_size.x,
			a_size.y,
			mode->refreshRate
		);

		m_impl->size = a_size;
	}

	Vector2i
	Window::GetPosition() const
	{
		return m_impl->position;
	}

	void
	Window::SetPosition(Vector2i a_position)
	{
		auto monitor = glfwGetWindowMonitor(m_impl->glfwWindow);
		auto mode = glfwGetVideoMode(monitor);

		glfwSetWindowMonitor(
			m_impl->glfwWindow,
			monitor,
			a_position.x,
			a_position.y,
			m_impl->size.x,
			m_impl->size.y,
			mode->refreshRate
		);

		m_impl->position = a_position;
	}

	std::string_view
	Window::GetTitle() const
	{
		return m_impl->title;
	}

	void
	Window::SetTitle(std::string_view a_title)
	{
		m_impl->title = std::string(a_title);
		glfwSetWindowTitle(m_impl->glfwWindow, m_impl->title.c_str());
	}

	WindowStateFlags
	Window::GetWindowStateFlags() const
	{
		return m_impl->windowState;
	}

	void
	Window::SetWindowStateFlags(WindowStateFlags a_flags)
	{
		if (m_impl->windowState == a_flags)
		{
			return;
		}

		Vector2i desiredSize;

		// get resolution of monitor
		const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());

		if (a_flags & (WS_Fullscreen | WS_Borderless))
		{
			// backup window position and window size
			glfwGetWindowPos(m_impl->glfwWindow, &m_impl->position.x, &m_impl->position.y);
			glfwGetWindowSize(m_impl->glfwWindow, &m_impl->size.y, &m_impl->size.y);

			// switch to full screen
			glfwSetWindowMonitor(
				m_impl->glfwWindow,
				glfwGetWindowMonitor(m_impl->glfwWindow),
				0,
				0,
				mode->width,
				mode->height,
				mode->refreshRate
			);

			desiredSize = { mode->width, mode->height };
		} else if (a_flags ^ (WS_Fullscreen | WS_Borderless))
		{
			glfwSetWindowMonitor(
				m_impl->glfwWindow,
				nullptr,
				m_impl->position.x,
				m_impl->position.y,
				m_impl->size.x,
				m_impl->size.y,
				mode->refreshRate
			);
			desiredSize = m_impl->size;
		}

		// Set Context Size
		OYL_UNUSED(desiredSize);
		m_impl->windowState = a_flags;
	}

	CursorState
	Window::GetCursorStateFlags() const
	{
		return m_impl->cursorState;
	}

	void
	Window::SetCursorStateFlags(CursorState a_state)
	{
		if (m_impl->cursorState == a_state)
		{
			return;
		}

		int table[CS_Last];
		table[CS_Normal] = GLFW_CURSOR_NORMAL;
		table[CS_Hidden] = GLFW_CURSOR_HIDDEN;
		table[CS_Disabled] = GLFW_CURSOR_DISABLED;
		table[CS_Locked] = GLFW_CURSOR_CAPTURED;

		uint glfwState = table[a_state];
		glfwSetInputMode(m_impl->glfwWindow, GLFW_CURSOR, glfwState);

		m_impl->cursorState = a_state;
	}

	void*
	Window::GetNativeWindowHandle() const
	{
		return m_impl->glfwWindow;
	}
}
