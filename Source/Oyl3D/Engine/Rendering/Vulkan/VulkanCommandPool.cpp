#include "VulkanCommandPool.h"

#include <vulkan/vulkan_raii.hpp>

#include "VulkanDevice.h"

namespace Oyl::Rendering
{
	struct VulkanCommandPool::Impl
	{
		vk::raii::CommandPool commandPool = nullptr;
	};

	VulkanCommandPool::VulkanCommandPool(nullptr_t)
		: m_impl(nullptr) {}

	VulkanCommandPool::VulkanCommandPool(const VulkanDevice& a_device, const CreateParams& a_params)
		: m_impl(std::make_unique<Impl>())
	{
		OYL_PROFILE_FUNCTION();

		auto& graphicsQueue = *a_device.GetCommandQueue(a_params.commandQueueFlags);

		vk::CommandPoolCreateInfo poolInfo {
			.flags = vk::CommandPoolCreateFlagBits::eResetCommandBuffer,
			.queueFamilyIndex = graphicsQueue.GetVkQueueFamilyIndex(),
		};
		m_impl->commandPool = vk::raii::CommandPool(a_device.GetVkDevice(), poolInfo);
	}

	VulkanCommandPool::VulkanCommandPool(VulkanCommandPool&& a_other) noexcept
	{
		*this = std::move(a_other);
	}

	VulkanCommandPool&
	VulkanCommandPool::operator=(VulkanCommandPool&& a_other) noexcept
	{
		if (this != &a_other)
		{
			m_impl = std::move(a_other.m_impl);
		}
		return *this;
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

		m_impl->commandPool.clear();
	}

	bool
	VulkanCommandPool::IsValid() const
	{
		return m_impl
		       && *m_impl->commandPool;
	}

	const vk::raii::CommandPool&
	VulkanCommandPool::GetVkCommandPool() const
	{
		return m_impl->commandPool;
	}
}
