#include "VulkanCommandPool.h"

#include <vulkan/vulkan_raii.hpp>

#include "VulkanDevice.h"

namespace Oyl::Rendering::Vulkan
{
	struct CommandPool::Impl
	{
		vk::raii::CommandPool commandPool = nullptr;
	};

	CommandPool::CommandPool()
		: m_impl(nullptr) {}

	CommandPool::CommandPool(const CreateParams& a_params)
		: m_impl(std::make_unique<Impl>())
	{
		OYL_PROFILE_FUNCTION();

		const auto& device = a_params.device;

		vk::CommandPoolCreateInfo poolInfo {
			.flags = vk::CommandPoolCreateFlagBits::eResetCommandBuffer,
			.queueFamilyIndex = device.GetVkGraphicsQueueIndex(),
		};
		m_impl->commandPool = vk::raii::CommandPool(device.GetVkDevice(), poolInfo);
	}

	CommandPool::CommandPool(CommandPool&& a_other) noexcept
	{
		*this = std::move(a_other);
	}

	CommandPool&
	CommandPool::operator=(CommandPool&& a_other) noexcept
	{
		if (this != &a_other)
		{
			Rendering::CommandPool::operator=(std::move(a_other));
			m_impl = std::move(a_other.m_impl);
		}
		return *this;
	}

	CommandPool::~CommandPool()
	{
		CommandPool::Destroy();
	}

	void
	CommandPool::Destroy()
	{
		if (!IsValid())
			return;

		m_impl->commandPool.clear();
	}

	bool
	CommandPool::IsValid() const
	{
		return m_impl
		       && *m_impl->commandPool;
	}

	const vk::raii::CommandPool&
	CommandPool::GetVkCommandPool() const
	{
		return m_impl->commandPool;
	}
}
