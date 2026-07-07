#include "MainWindowModule.h"

#include <GLFW/glfw3.h>

#include "Core/Application/ApplicationEvents.h"
#include "Core/Application/GlfwWindow.h"
#include "Core/Logging/Logging.h"

namespace Oyl
{
	static void GlfwErrorCallback(int a_error, const char* a_description)
	{
		OYL_LOG_ERROR("GLFW ERROR ({}): {}", a_error, a_description);
	}

	void Test(const Event&) {}

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
		//params.postEventCallback = Delegate<void(const Event&)>::Create<&MainWindowModule::PostEvent>(this);
		//params.postEventCallback = Delegate(&Test);
		//params.postEventCallback = CreateDelegate<&Module::PostEvent>(this);
		//params.postEventCallback = { this, &Module::PostEvent };
		//params.postEventCallback = CreateDelegate(this, &MainWindowModule::PostEvent);
		params.postEventCallback = PostEventDelegate::Create(this, &Module::PostEvent);

		m_window = new GlfwWindow(params);

		RegisterEventListener(&MainWindowModule::OnWindowResizeEvent);
		RegisterEventListener(&MainWindowModule::OnWindowMoveEvent);
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
	MainWindowModule::OnUpdate()
	{
		OYL_PROFILE_FUNCTION();

		if (m_window->IsValid())
		{
			m_window->Update();
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
	MainWindowModule::OnWindowCloseRequestEvent(const WindowCloseRequestEvent& a_event)
	{
		if (!m_window)
			return;

		if (a_event.nativeWindow != m_window->GetNativeWindowHandle())
			return;

		PostEvent(ApplicationCloseRequestEvent());
	}

	void
	MainWindowModule::OnWindowFocusEvent(const WindowFocusEvent& a_event)
	{
		OYL_LOG("Window Focus: {}", a_event.focused);
	}

	void
	MainWindowModule::OnWindowKeyPressEvent(const WindowKeyPressEvent& a_event)
	{
		OYL_LOG("Window Key Press: {}", a_event.key);
	}

	void
	MainWindowModule::OnWindowKeyReleaseEvent(const WindowKeyReleaseEvent& a_event)
	{
		OYL_LOG("Window Key Release: {}", a_event.key);
	}

	void
	MainWindowModule::OnWindowMousePressEvent(const WindowMousePressEvent& a_event)
	{
		OYL_LOG("Window Mouse Button Press: {}", a_event.button);
	}

	void
	MainWindowModule::OnWindowMouseReleaseEvent(const WindowMouseReleaseEvent& a_event)
	{
		OYL_LOG("Window Mouse Button Release: {}", a_event.button);
	}

	void
	MainWindowModule::OnWindowMouseScrollEvent(const WindowMouseScrollEvent& a_event)
	{
		OYL_LOG("Window Mouse Scroll: ({}, {})", a_event.scroll.x, a_event.scroll.y);
	}

	void
	MainWindowModule::OnWindowCursorMoveEvent(const WindowCursorMoveEvent& a_event)
	{
		OYL_LOG("Window Cursor Moved: ({}, {})", a_event.position.x, a_event.position.y);
	}
}
