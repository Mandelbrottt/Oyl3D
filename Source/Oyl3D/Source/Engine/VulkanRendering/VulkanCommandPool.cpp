#include "VulkanCommandPool.h"

#include <vulkan/vulkan_raii.hpp>

#include "VulkanDevice.h"

namespace Oyl
{
	VulkanCommandPool::VulkanCommandPool(const VulkanDevice& a_device, const CommandPoolCreateParams& a_params)
	{
		OYL_PROFILE_FUNCTION();

		auto& graphicsQueue = *a_device.GetCommandQueue(a_params.commandQueueFlags);

		vk::CommandPoolCreateInfo poolInfo {
			.flags = vk::CommandPoolCreateFlagBits::eResetCommandBuffer,
			.queueFamilyIndex = graphicsQueue.GetVkQueueFamilyIndex(),
		};
		m_commandPool = vk::raii::CommandPool(a_device.GetVkDevice(), poolInfo);
	}

	VulkanCommandPool::~VulkanCommandPool()
	{
		VulkanCommandPool::Destroy();
	}

	void
	VulkanCommandPool::Destroy()
	{
		if (!IsValid())
			return;

		m_commandPool.clear();
	}

	bool
	VulkanCommandPool::IsValid() const
	{
		return *m_commandPool;
	}

	const vk::raii::CommandPool&
	VulkanCommandPool::GetVkCommandPool() const
	{
		return m_commandPool;
	}
}
