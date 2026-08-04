#include "VulkanCommandPool.h"

#include <vulkan/vulkan_raii.hpp>

#include "VulkanDevice.h"

namespace Oyl::Rendering::Vulkan
{
	struct CommandPoolImpl::Impl
	{
		vk::raii::CommandPool commandPool = nullptr;
	};

	CommandPoolImpl::CommandPoolImpl(nullptr_t)
		: m_impl(nullptr) {}

	CommandPoolImpl::CommandPoolImpl(const DeviceImpl& a_device, const CreateParams& a_params)
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

	CommandPoolImpl::CommandPoolImpl(CommandPoolImpl&& a_other) noexcept
	{
		*this = std::move(a_other);
	}

	CommandPoolImpl&
	CommandPoolImpl::operator=(CommandPoolImpl&& a_other) noexcept
	{
		if (this != &a_other)
		{
			m_impl = std::move(a_other.m_impl);
		}
		return *this;
	}

	CommandPoolImpl::~CommandPoolImpl()
	{
		CommandPoolImpl::Destroy();
	}

	void
	CommandPoolImpl::Destroy()
	{
		if (!IsValid())
			return;

		m_impl->commandPool.clear();
	}

	bool
	CommandPoolImpl::IsValid() const
	{
		return m_impl
		       && *m_impl->commandPool;
	}

	const vk::raii::CommandPool&
	CommandPoolImpl::GetVkCommandPool() const
	{
		return m_impl->commandPool;
	}
}
