#include "VulkanSemaphore.h"

#include <vulkan/vulkan_raii.hpp>

#include "VulkanDevice.h"

namespace Oyl::Rendering
{
	VulkanSemaphore::VulkanSemaphore(const VulkanDevice& a_device)
		: m_device(&a_device),
		  m_semaphore(
			  vk::raii::Semaphore(a_device.GetVkDevice(), vk::SemaphoreCreateInfo {})
		  ) {}

	VulkanSemaphore::VulkanSemaphore(VulkanSemaphore&& a_other) noexcept
	{
		*this = std::move(a_other);
	}

	VulkanSemaphore&
	VulkanSemaphore::operator=(VulkanSemaphore&& a_other) noexcept
	{
		if (this != &a_other)
		{
			std::swap(m_device, a_other.m_device);
			std::swap(m_semaphore, a_other.m_semaphore);
		}
		return *this;
	}

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
