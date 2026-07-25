#include "VulkanSemaphore.h"

#include <vulkan/vulkan_raii.hpp>

#include "VulkanDevice.h"

namespace Oyl::Rendering::Vulkan
{
	Semaphore::Semaphore(const Device& a_device)
		: m_semaphore(
			vk::raii::Semaphore(a_device.GetVkDevice(), vk::SemaphoreCreateInfo {})
		) {}

	Semaphore::Semaphore(Semaphore&& a_other) noexcept
	{
		*this = std::move(a_other);
	}

	Semaphore&
	Semaphore::operator=(Semaphore&& a_other) noexcept
	{
		if (this != &a_other)
		{
			m_semaphore = std::move(a_other.m_semaphore);
		}
		return *this;
	}

	void
	Semaphore::Destroy()
	{
		if (!IsValid())
			return;

		m_semaphore.clear();
	}
}
