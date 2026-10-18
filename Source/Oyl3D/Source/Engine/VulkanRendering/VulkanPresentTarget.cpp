#include "VulkanPresentTarget.h"

#include <Application/ApplicationWindow.h>

#include <Core/Modules/ModuleManager.h>
#include <Core/Profiling/Profiling.h>

#include <GLFW/glfw3.h>

namespace Oyl
{
	VulkanPresentTarget::VulkanPresentTarget(const VulkanPresentTargetCreateParams& a_params)
	{
		OYL_PROFILE_FUNCTION();

		CreateVkSurface(a_params.vkInstance, a_params.window);
	}

	VulkanPresentTarget::~VulkanPresentTarget()
	{
		VulkanPresentTarget::Destroy();
	}

	void
	VulkanPresentTarget::Destroy()
	{
		OYL_PROFILE_FUNCTION();

		if (!IsValid())
			return;

		m_vkSurface.clear();
	}

	void
	VulkanPresentTarget::CreateVkSurface(const vk::raii::Instance& a_vkInstance, const IApplicationWindow& a_window)
	{
		OYL_PROFILE_FUNCTION();

		VkSurfaceKHR cSurface;
		if (ModuleManager::GetModulePointer("GlfwModule"))
		{
			auto glfwWindow = static_cast<GLFWwindow*>(a_window.GetNativeWindowHandle());
			if (glfwCreateWindowSurface(*a_vkInstance, glfwWindow, nullptr, &cSurface) != VK_SUCCESS)
			{
				throw std::runtime_error("failed to create window surface!");
			}
			m_vkSurface = vk::raii::SurfaceKHR(a_vkInstance, cSurface);
		}
	}
}
