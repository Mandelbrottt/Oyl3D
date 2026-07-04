#include "MainWindowModule.h"

#include <GLFW/glfw3.h>

#include "Core/Logging/Logging.h"

namespace Oyl
{
	static void GlfwErrorCallback(int a_error, const char* a_description)
	{
		OYL_LOG_ERROR("GLFW ERROR ({}): {}", a_error, a_description);
	}

	void
	MainWindowModule::OnInit()
	{
		OYL_PROFILE_FUNCTION();

		int initResult = glfwInit();
		if (!initResult)
		{
			const char* err;
			glfwGetError(&err);
			OYL_LOG_ERROR("Failed to initialize GLFW [{}]: {}", initResult, err);
			return;
		}

		glfwSetErrorCallback(GlfwErrorCallback);

		WindowParams params;
		params.size = { 1280, 720 };
		params.title = "Oyl3D - Now with more code!";
		params.cursorState = CS_Normal;
		params.windowState = WS_None;
		m_window = Window(params);

		EventDelegate delegate = EventDelegate::Create(this, &MainWindowModule::PostEvent);
		m_window.SetEventCallback(delegate);

		RegisterEventListener(&MainWindowModule::OnWindowResizeEvent);
		RegisterEventListener(&MainWindowModule::OnWindowMoveEvent);
		RegisterEventListener(&MainWindowModule::OnWindowCloseRequestEvent);
		RegisterEventListener(&MainWindowModule::OnWindowFocusEvent);
		RegisterEventListener(&MainWindowModule::OnWindowCursorMoveEvent);
	}

	void
	MainWindowModule::OnUpdate()
	{
		OYL_PROFILE_FUNCTION();

		if (m_window.IsValid())
		{
			m_window.Update();
		}
	}

	void
	MainWindowModule::OnShutdown()
	{
		OYL_PROFILE_FUNCTION();

		if (!glfwInit())
		{
			return;
		}

		glfwTerminate();
	}

	void
	MainWindowModule::OnWindowResizeEvent(const WindowResizeEvent& a_event)
	{
		OYL_LOG("Window Resized: ({}, {})", a_event.size.x, a_event.size.y);
	}

	void
	MainWindowModule::OnWindowMoveEvent(const WindowMoveEvent& a_event)
	{
		OYL_LOG("Window Moved: ({}, {})", a_event.position.x, a_event.position.y);
	}

	void
	MainWindowModule::OnWindowCloseRequestEvent(const WindowCloseRequestEvent&)
	{
		OYL_LOG("Window Close Request");
	}

	void
	MainWindowModule::OnWindowFocusEvent(const WindowFocusEvent& a_event)
	{
		OYL_LOG("Window Focus: {}", a_event.focused);
	}

	void
	MainWindowModule::OnWindowCursorMoveEvent(const WindowCursorMoveEvent& a_event)
	{
		OYL_LOG("Window Cursort Moved: ({}, {})", a_event.position.x, a_event.position.y);
	}
}
