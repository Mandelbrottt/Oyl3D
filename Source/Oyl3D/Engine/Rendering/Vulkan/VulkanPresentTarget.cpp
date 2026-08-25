#include "VulkanPresentTarget.h"

#include <GLFW/glfw3.h>

#include "VulkanRenderEngine.h"

#include "Rendering/Window.h"

namespace Oyl::Rendering
{
	VulkanPresentTarget::VulkanPresentTarget(const CreateParams& a_params)
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
		if (!IsValid())
			return;

		m_vkSurface.clear();
	}

	bool
	VulkanPresentTarget::IsValid() const
	{
		return *m_vkSurface;
	}

	void
	VulkanPresentTarget::CreateVkSurface(const vk::raii::Instance& a_vkInstance, const IWindow& a_window)
	{
		VkSurfaceKHR cSurface;
		auto glfwWindow = static_cast<GLFWwindow*>(a_window.GetNativeWindowHandle());
		if (glfwCreateWindowSurface(*a_vkInstance, glfwWindow, nullptr, &cSurface) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create window surface!");
		}
		m_vkSurface = vk::raii::SurfaceKHR(a_vkInstance, cSurface);
	}
}
