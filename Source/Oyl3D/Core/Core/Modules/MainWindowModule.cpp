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
}
