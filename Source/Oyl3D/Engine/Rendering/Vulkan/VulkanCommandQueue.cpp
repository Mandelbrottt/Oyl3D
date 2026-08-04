#include "VulkanCommandQueue.h"

#include <vulkan/vulkan_raii.hpp>

#include "VulkanCommandBuffer.h"
#include "VulkanDevice.h"
#include "VulkanEnums.h"
#include "VulkanSwapChain.h"

namespace Oyl::Rendering::Vulkan
{
	struct CommandQueueImpl::Impl
	{
		vk::raii::Queue queue = nullptr;
		uint32 queueFamilyIndex;
	};

	CommandQueueImpl::CommandQueueImpl(nullptr_t)
		: m_impl(nullptr) {}

	CommandQueueImpl::CommandQueueImpl(const DeviceImpl& a_device, const CreateParams& a_params)
		: m_impl(std::make_unique<Impl>())
	{
		auto& vkDevice = a_device.GetVkDevice();
		m_impl->queueFamilyIndex = a_params.queueFamilyIndex;
		m_impl->queue = vk::raii::Queue(vkDevice, m_impl->queueFamilyIndex, 0);
	}

	CommandQueueImpl::CommandQueueImpl(CommandQueueImpl&& a_other) noexcept
	{
		*this = std::move(a_other);
	}

	CommandQueueImpl&
	CommandQueueImpl::operator=(CommandQueueImpl&& a_other) noexcept
	{
		if (this != &a_other)
		{
			m_impl = std::move(a_other.m_impl);
		}
		return *this;
	}

	CommandQueueImpl::~CommandQueueImpl()
	{
		CommandQueueImpl::Destroy();
	}

	void
	CommandQueueImpl::Destroy()
	{
		if (!IsValid())
			return;

		m_impl->queue.clear();
	}

	bool
	CommandQueueImpl::IsValid() const
	{
		return m_impl
		       && *m_impl->queue;
	}

	const vk::raii::Queue&
	CommandQueueImpl::GetVkQueue() const
	{
		return m_impl->queue;
	}

	uint32
	CommandQueueImpl::GetVkQueueFamilyIndex() const
	{
		return m_impl->queueFamilyIndex;
	}

	void
	CommandQueueImpl::WaitUntilIdle() const
	{
		OYL_PROFILE_FUNCTION();

		m_impl->queue.waitIdle();
	}

	bool
	CommandQueueImpl::Submit(const SubmitParams& a_params) const
	{
		OYL_PROFILE_FUNCTION();

		auto& vkCommandBuffer = dynamic_cast<const CommandBufferImpl&>(a_params.commandBuffer).GetVkCommandBuffer();
		vk::Semaphore vkWaitSemaphore = static_cast<SemaphoreHandle>(a_params.waitSemaphore);
		vk::Semaphore vkSignalSemaphore = static_cast<SemaphoreHandle>(a_params.signalSemaphore);
		vk::Fence vkFence = static_cast<FenceHandle>(a_params.fence);

		vk::PipelineStageFlags waitDestinationStageMask = ToVkEnum(a_params.waitDestinationStageMask);
		vk::SubmitInfo submitInfo;
		submitInfo.setCommandBufferCount(1)
		          .setPCommandBuffers(&*vkCommandBuffer);

		if (a_params.waitDestinationStageMask)
			submitInfo.setPWaitDstStageMask(&waitDestinationStageMask);

		if (vkWaitSemaphore)
			submitInfo.setWaitSemaphores(vkWaitSemaphore);

		if (vkSignalSemaphore)
			submitInfo.setSignalSemaphores(vkSignalSemaphore);

		m_impl->queue.submit(submitInfo, vkFence);
		return true;
	}

	bool
	CommandQueueImpl::Present(const PresentParams& a_params) const
	{
		OYL_PROFILE_FUNCTION();

		vk::Semaphore vkWaitSemaphore = static_cast<SemaphoreHandle>(a_params.waitSemaphore);
		auto& vkSwapChain = dynamic_cast<const SwapChainImpl&>(a_params.swapChain).GetVkSwapChain();
		auto swapChainImageIndex = a_params.swapChain.GetCurrentImageIndex();

		const vk::PresentInfoKHR presentInfoKHR {
			.waitSemaphoreCount = 1,
			.pWaitSemaphores = &vkWaitSemaphore,
			.swapchainCount = 1,
			.pSwapchains = &*vkSwapChain,
			.pImageIndices = &swapChainImageIndex
		};

		auto result = m_impl->queue.presentKHR(presentInfoKHR);
		if (result == vk::Result::eSuboptimalKHR || result == vk::Result::eErrorOutOfDateKHR)
			return false;

		return true;
	}
}
