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

	private:
		String m_title;

		Vector2i m_position;
		Vector2u m_size;

		WindowStateFlags m_windowState;
		CursorState m_cursorState;

		PostEventDelegate m_postEventCallback;

		GLFWwindow* m_glfwWindow = nullptr;
	};
}
