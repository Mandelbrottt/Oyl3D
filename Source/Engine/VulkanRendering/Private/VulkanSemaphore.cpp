#include "VulkanSemaphore.h"

#include <vulkan/vulkan_raii.hpp>

#include "VulkanDevice.h"

namespace Oyl
{
	VulkanSemaphore::VulkanSemaphore(const VulkanDevice& a_device)
		: m_device(&a_device),
		  m_semaphore(
			  vk::raii::Semaphore(a_device.GetVkDevice(), vk::SemaphoreCreateInfo {})
		  ) {}

	void
	VulkanSemaphore::Destroy()
	{
		if (!IsValid())
			return;

		m_semaphore.clear();
	}

	bool
	VulkanSemaphore::IsValid() const
	{
		return m_device && *m_semaphore;
	}
}
