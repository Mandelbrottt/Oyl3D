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
		const MulticastDelegate<void(const WindowResizeEvent&)>&
		OnResizeCallback() const override { return m_windowResizeCallback; };

		const MulticastDelegate<void(const WindowMaximizeEvent&)>&
		OnMaximizeCallback() const override { return m_windowMaximizeCallback; }

		const MulticastDelegate<void(const WindowMinimizeEvent&)>&
		OnMinimizeCallback() const override { return m_windowMinimizeCallback; }

		const MulticastDelegate<void(const WindowMoveEvent&)>&
		OnMoveCallback() const override { return m_windowMoveCallback; }

		const MulticastDelegate<void(const WindowCloseRequestEvent&)>&
		OnCloseRequestCallback() const override { return m_windowCloseRequestCallback; }

		const MulticastDelegate<void(const WindowCloseEvent&)>&
		OnCloseCallback() const override { return m_windowCloseCallback; }

		const MulticastDelegate<void(const WindowFocusEvent&)>&
		OnFocusCallback() const override { return m_windowFocusCallback; }

		const MulticastDelegate<void(const WindowKeyPressEvent&)>&
		OnKeyPressCallback() const override { return m_keyPressCallback; }

		const MulticastDelegate<void(const WindowKeyReleaseEvent&)>&
		OnKeyReleaseCallback() const override { return m_keyReleaseCallback; }

		const MulticastDelegate<void(const WindowMousePressEvent&)>&
		OnMousePressCallback() const override { return m_mousePressCallback; }

		const MulticastDelegate<void(const WindowMouseReleaseEvent&)>&
		OnMouseReleaseCallback() const override { return m_mouseReleaseCallback; }

		const MulticastDelegate<void(const WindowMouseScrollEvent&)>&
		OnMouseScrollCallback() const override { return m_mouseScrollCallback; }

		const MulticastDelegate<void(const WindowCursorMoveEvent&)>&
		OnCursorMoveCallback() const override { return m_cursorMoveCallback; }

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
		MulticastDelegate<void(const WindowCloseEvent&)> m_windowCloseCallback;
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
