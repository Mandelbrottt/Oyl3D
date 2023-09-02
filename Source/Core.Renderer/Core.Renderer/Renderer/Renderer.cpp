#include "pch.h"
#include "Renderer.h"

#include <Core/Logging/Logging.h>
#include <vulkan/vulkan.hpp>

namespace Rearm::Renderer
{	
	void
	Init()
	{
		uint32_t extensionCount = 0;
		vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);

		Logging::GetCoreLogger().info("{} extensions supported", extensionCount);
	}

	void
	Shutdown() {}
}