#pragma once

#include <Application/ApplicationWindow.h>

#include <GLFW/glfw3.h>

namespace Oyl
{
	class GlfwApplicationWindow : public IApplicationWindow
	{
	public:
		explicit
		GlfwApplicationWindow(const ApplicationWindowCreateParams& a_params);

		NO_COPY(GlfwApplicationWindow);
		NO_MOVE(GlfwApplicationWindow);

		virtual
		~GlfwApplicationWindow();

		void
		Close() override;

		bool
		IsValid() const override;

		void
		Update() override;

		Vector2u
		GetSize() const override;

		void
		SetSize(Vector2u a_size) override;

		Vector2u
		GetContentSize() const override;

		Vector2i
		GetPosition() const override;

		void
		SetPosition(Vector2i a_position) override;

		StringView
		GetTitle() const override;

		void
		SetTitle(StringView a_title) override;

		WindowStateFlags
		GetWindowState() const override;

		void
		SetWindowState(WindowStateFlags a_flags) override;

		CursorState
		GetCursorState() const override;

		void
		SetCursorState(CursorState a_state) override;

		void*
		GetNativeWindowHandle() const override;

	private:
		void
		CreateGlfwWindow();

		void
		SetupGlfwWindowCallbacks();

	public:
		void
		RegisterWindowResizeCallback(Delegate<void(const WindowResizeEvent&)> a_callback) override
		{
			m_windowResizeCallback += a_callback;
		}

		void
		UnregisterWindowResizeCallback(Delegate<void(const WindowResizeEvent&)> a_callback) override
		{
			m_windowResizeCallback -= a_callback;
		}

		void
		RegisterWindowMaximizeCallback(Delegate<void(const WindowMaximizeEvent&)> a_callback) override
		{
			m_windowMaximizeCallback += a_callback;
		}

		void
		UnregisterWindowMaximizeCallback(Delegate<void(const WindowMaximizeEvent&)> a_callback) override
		{
			m_windowMaximizeCallback -= a_callback;
		}

		void
		RegisterWindowMinimizeCallback(Delegate<void(const WindowMinimizeEvent&)> a_callback) override
		{
			m_windowMinimizeCallback += a_callback;
		}

		void
		UnregisterWindowMinimizeCallback(Delegate<void(const WindowMinimizeEvent&)> a_callback) override
		{
			m_windowMinimizeCallback -= a_callback;
		}

		void
		RegisterWindowMoveCallback(Delegate<void(const WindowMoveEvent&)> a_callback) override
		{
			m_windowMoveCallback += a_callback;
		}

		void
		UnregisterWindowMoveCallback(Delegate<void(const WindowMoveEvent&)> a_callback) override
		{
			m_windowMoveCallback -= a_callback;
		}

		void
		RegisterWindowClosedCallback(Delegate<void(const WindowClosedEvent&)> a_callback) override
		{
			m_windowClosedCallback += a_callback;
		}

		void
		UnregisterWindowClosedCallback(Delegate<void(const WindowClosedEvent&)> a_callback) override
		{
			m_windowClosedCallback -= a_callback;
		}

		void
		RegisterWindowCloseRequestCallback(Delegate<void(const WindowCloseRequestEvent&)> a_callback) override
		{
			m_windowCloseRequestCallback += a_callback;
		}

		void
		UnregisterWindowCloseRequestCallback(Delegate<void(const WindowCloseRequestEvent&)> a_callback) override
		{
			m_windowCloseRequestCallback -= a_callback;
		}

		void
		RegisterWindowFocusCallback(Delegate<void(const WindowFocusEvent&)> a_callback) override
		{
			m_windowFocusCallback += a_callback;
		}

		void
		UnregisterWindowFocusCallback(Delegate<void(const WindowFocusEvent&)> a_callback) override
		{
			m_windowFocusCallback -= a_callback;
		}

		void
		RegisterKeyPressCallback(Delegate<void(const WindowKeyPressEvent&)> a_callback) override
		{
			m_keyPressCallback += a_callback;
		}

		void
		UnregisterKeyPressCallback(Delegate<void(const WindowKeyPressEvent&)> a_callback) override
		{
			m_keyPressCallback -= a_callback;
		}

		void
		RegisterKeyReleaseCallback(Delegate<void(const WindowKeyReleaseEvent&)> a_callback) override
		{
			m_keyReleaseCallback += a_callback;
		}

		void
		UnregisterKeyReleaseCallback(Delegate<void(const WindowKeyReleaseEvent&)> a_callback) override
		{
			m_keyReleaseCallback -= a_callback;
		}

		void
		RegisterMousePressCallback(Delegate<void(const WindowMousePressEvent&)> a_callback) override
		{
			m_mousePressCallback += a_callback;
		}

		void
		UnregisterMousePressCallback(Delegate<void(const WindowMousePressEvent&)> a_callback) override
		{
			m_mousePressCallback -= a_callback;
		}

		void
		RegisterMouseReleaseCallback(Delegate<void(const WindowMouseReleaseEvent&)> a_callback) override
		{
			m_mouseReleaseCallback += a_callback;
		}

		void
		UnregisterMouseReleaseCallback(Delegate<void(const WindowMouseReleaseEvent&)> a_callback) override
		{
			m_mouseReleaseCallback -= a_callback;
		}

		void
		RegisterMouseScrollCallback(Delegate<void(const WindowMouseScrollEvent&)> a_callback) override
		{
			m_mouseScrollCallback += a_callback;
		}

		void
		UnregisterMouseScrollCallback(Delegate<void(const WindowMouseScrollEvent&)> a_callback) override
		{
			m_mouseScrollCallback -= a_callback;
		}

		void
		RegisterCursorMoveCallback(Delegate<void(const WindowCursorMoveEvent&)> a_callback) override
		{
			m_cursorMoveCallback += a_callback;
		}

		void
		UnregisterCursorMoveCallback(Delegate<void(const WindowCursorMoveEvent&)> a_callback) override
		{
			m_cursorMoveCallback -= a_callback;
		}

	private:
		String m_title;

		Vector2i m_position;
		Vector2u m_size;

		WindowStateFlags m_windowState;
		CursorState m_cursorState;

		GLFWwindow* m_glfwWindow = nullptr;

		MulticastDelegate<void(const WindowResizeEvent&)> m_windowResizeCallback;
		MulticastDelegate<void(const WindowMinimizeEvent&)> m_windowMinimizeCallback;
		MulticastDelegate<void(const WindowMaximizeEvent&)> m_windowMaximizeCallback;
		MulticastDelegate<void(const WindowMoveEvent&)> m_windowMoveCallback;
		MulticastDelegate<void(const WindowClosedEvent&)> m_windowClosedCallback;
		MulticastDelegate<void(const WindowCloseRequestEvent&)> m_windowCloseRequestCallback;
		MulticastDelegate<void(const WindowFocusEvent&)> m_windowFocusCallback;
		MulticastDelegate<void(const WindowKeyPressEvent&)> m_keyPressCallback;
		MulticastDelegate<void(const WindowKeyReleaseEvent&)> m_keyReleaseCallback;
		MulticastDelegate<void(const WindowMousePressEvent&)> m_mousePressCallback;
		MulticastDelegate<void(const WindowMouseReleaseEvent&)> m_mouseReleaseCallback;
		MulticastDelegate<void(const WindowMouseScrollEvent&)> m_mouseScrollCallback;
		MulticastDelegate<void(const WindowCursorMoveEvent&)> m_cursorMoveCallback;
	};
}
