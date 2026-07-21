#include "MainWindowModule.h"

#include <GLFW/glfw3.h>

#include "Core/Application/ApplicationEvents.h"
#include "Core/Logging/Logging.h"

#include "Rendering/Glfw/GlfwWindow.h"

namespace Oyl
{
	static void GlfwErrorCallback(int a_error, const char* a_description)
	{
		OYL_LOG_ERROR("GLFW ERROR ({}): {}", a_error, a_description);
	}

	void
	MainWindowModule::Setup()
	{
		RegisterEventListener(&MainWindowModule::OnWindowResizeEvent);
		RegisterEventListener(&MainWindowModule::OnWindowMoveEvent);
		RegisterEventListener(&MainWindowModule::OnWindowClosedEvent);
		RegisterEventListener(&MainWindowModule::OnWindowCloseRequestEvent);
		RegisterEventListener(&MainWindowModule::OnWindowFocusEvent);
		RegisterEventListener(&MainWindowModule::OnWindowKeyPressEvent);
		RegisterEventListener(&MainWindowModule::OnWindowKeyReleaseEvent);
		RegisterEventListener(&MainWindowModule::OnWindowMousePressEvent);
		RegisterEventListener(&MainWindowModule::OnWindowMouseReleaseEvent);
		RegisterEventListener(&MainWindowModule::OnWindowMouseScrollEvent);
		RegisterEventListener(&MainWindowModule::OnWindowCursorMoveEvent);
	}

	void
	MainWindowModule::Init()
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

		WindowParams params {
			.size = { 1280, 720 },
			.title = "Oyl3D - Now with more code!",
			.windowState = WS_None,
			.cursorState = CS_Normal,
			.postEventCallback = PostEventDelegate::Create(this, &MainWindowModule::PostEvent),
		};
		m_window = new Glfw::Window(params);
	}

	void
	MainWindowModule::Update()
	{
		OYL_PROFILE_FUNCTION();

		if (m_window->IsValid())
		{
			m_window->Update();
		}
	}

	void
	MainWindowModule::Shutdown()
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
		OYL_UNUSED(a_event);
	}

	void
	MainWindowModule::OnWindowMoveEvent(const WindowMoveEvent& a_event)
	{
		OYL_UNUSED(a_event);
	}

	void
	MainWindowModule::OnWindowClosedEvent(const WindowClosedEvent& a_event)
	{
		if (!m_window)
			return;

		if (a_event.window != m_window)
			return;

		PostEvent(ApplicationCloseRequestEvent());
	}

	void
	MainWindowModule::OnWindowCloseRequestEvent(const WindowCloseRequestEvent& a_event)
	{
		if (m_window != a_event.window)
			return;

		m_window->Destroy();
	}

	void
	MainWindowModule::OnWindowFocusEvent(const WindowFocusEvent& a_event)
	{
		OYL_UNUSED(a_event);
	}

	void
	MainWindowModule::OnWindowKeyPressEvent(const WindowKeyPressEvent& a_event)
	{
		OYL_UNUSED(a_event);
	}

	void
	MainWindowModule::OnWindowKeyReleaseEvent(const WindowKeyReleaseEvent& a_event)
	{
		OYL_UNUSED(a_event);
	}

	void
	MainWindowModule::OnWindowMousePressEvent(const WindowMousePressEvent& a_event)
	{
		OYL_UNUSED(a_event);
	}

	void
	MainWindowModule::OnWindowMouseReleaseEvent(const WindowMouseReleaseEvent& a_event)
	{
		OYL_UNUSED(a_event);
	}

	void
	MainWindowModule::OnWindowMouseScrollEvent(const WindowMouseScrollEvent& a_event)
	{
		OYL_UNUSED(a_event);
	}

	void
	MainWindowModule::OnWindowCursorMoveEvent(const WindowCursorMoveEvent& a_event)
	{
		OYL_UNUSED(a_event);
	}
}
