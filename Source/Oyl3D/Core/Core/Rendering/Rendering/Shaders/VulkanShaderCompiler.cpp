#include "VulkanShaderCompiler.h"

#if defined(OYL_WINDOWS)
#include <atlcomcli.h>
#include <Windows.h>
#endif

#include <Core/Logging/Logging.h>

#include <dxc/dxcapi.h>
#include <vulkan/vulkan_raii.hpp>

namespace Oyl::Rendering::Internal
{
	struct VulkanShaderCompiler::Impl {};

	VulkanShaderCompiler::VulkanShaderCompiler()
		: m_impl(nullptr) {}

	VulkanShaderCompiler::~VulkanShaderCompiler() {}

	VulkanShader*
	VulkanShaderCompiler::Compile()
	{
		return nullptr;
	}
}
