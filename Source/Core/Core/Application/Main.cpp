#include "pch.h"
#include "Main.h"
#include "Logging.h"

#include <GLFW/glfw3.h>

#include <vulkan/vulkan.hpp>

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

		uint32_t extensionCount = 0;
		vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);

		Logging::GetCoreLogger().info("{} extensions supported", extensionCount);
	}

	void
	Shutdown()
	{
		Logging::GetCoreLogger().info("Shutting Down");
	}
}
