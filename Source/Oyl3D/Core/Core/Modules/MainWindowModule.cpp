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

		RegisterEventListener(&MainWindowModule::OnWindowResizedEvent);
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
	MainWindowModule::OnWindowResizedEvent(const WindowResizedEvent& a_event)
	{
		OYL_LOG("Window Resized: ({}, {})", a_event.size.x, a_event.size.y);
	}
}
