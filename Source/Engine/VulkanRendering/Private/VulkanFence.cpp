#include "VulkanFence.h"

#include "VulkanDevice.h"

namespace Oyl
{
	VulkanFence::VulkanFence(const VulkanDevice& a_device)
		: m_device(&a_device),
		  m_fence(
			  vk::raii::Fence(
				  a_device.GetVkDevice(),
				  vk::FenceCreateInfo {
					  .flags = vk::FenceCreateFlagBits::eSignaled
				  }
			  )
		  ) {}

	VulkanFence::~VulkanFence()
	{
		VulkanFence::Destroy();
	}

	void
	VulkanFence::Destroy()
	{
		if (!IsValid())
			return;

		m_fence.clear();
	}

	bool
	VulkanFence::IsValid() const
	{
		return m_device && *m_fence;
	}

	void
	VulkanFence::Reset()
	{
		const auto& vkFence = GetVkFence();
		const auto& vkDevice = m_device->GetVkDevice();

		vkDevice.resetFences(*vkFence);
	}

	bool
	VulkanFence::Wait(uint64 a_timeout)
	{
		OYL_PROFILE_FUNCTION();

		const auto& vkFence = GetVkFence();
		const auto& vkDevice = m_device->GetVkDevice();

		auto result = vkDevice.waitForFences(*vkFence, vk::True, a_timeout);
		return result != vk::Result::eSuccess;
	}
}
