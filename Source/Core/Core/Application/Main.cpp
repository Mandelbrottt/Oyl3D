#include "pch.h"
#include "Main.h"
#include "Logging.h"

#include <GLFW/glfw3.h>

namespace Rearm
{	
	void
	Init()
	{
		Logging::Init();
		
		Logging::GetCoreLogger().info("Core Logger Initialized");

		glfwInit();
		Logging::GetCoreLogger().info("GLFW Initialized");

		glfwTerminate();
		Logging::GetCoreLogger().info("GLFW Terminated");
	}

	void
	Shutdown()
	{
		Logging::GetCoreLogger().info("Shutting Down");
	}
}
