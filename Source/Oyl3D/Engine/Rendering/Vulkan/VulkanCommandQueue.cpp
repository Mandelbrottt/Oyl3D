#include "VulkanCommandQueue.h"

#include <vulkan/vulkan_raii.hpp>

#include "VulkanCommandBuffer.h"
#include "VulkanDevice.h"
#include "VulkanEnums.h"
#include "VulkanSwapChain.h"

namespace Oyl::Rendering
{
	struct VulkanCommandQueue::Impl
	{
		vk::raii::Queue queue = nullptr;
		uint32 queueFamilyIndex;
	};

	VulkanCommandQueue::VulkanCommandQueue(nullptr_t)
		: m_impl(nullptr) {}

	VulkanCommandQueue::VulkanCommandQueue(const VulkanDevice& a_device, const CreateParams& a_params)
		: m_impl(std::make_unique<Impl>())
	{
		auto& vkDevice = a_device.GetVkDevice();
		m_impl->queueFamilyIndex = a_params.queueFamilyIndex;
		m_impl->queue = vk::raii::Queue(vkDevice, m_impl->queueFamilyIndex, 0);
	}

	VulkanCommandQueue::VulkanCommandQueue(VulkanCommandQueue&& a_other) noexcept
	{
		*this = std::move(a_other);
	}

	VulkanCommandQueue&
	VulkanCommandQueue::operator=(VulkanCommandQueue&& a_other) noexcept
	{
		if (this != &a_other)
		{
			m_impl = std::move(a_other.m_impl);
		}
		return *this;
	}

	VulkanCommandQueue::~VulkanCommandQueue()
	{
		VulkanCommandQueue::Destroy();
	}

	void
	VulkanCommandQueue::Destroy()
	{
		if (!IsValid())
			return;

		m_impl->queue.clear();
	}

	bool
	VulkanCommandQueue::IsValid() const
	{
		return m_impl
		       && *m_impl->queue;
	}

	const vk::raii::Queue&
	VulkanCommandQueue::GetVkQueue() const
	{
		return m_impl->queue;
	}

	uint32
	VulkanCommandQueue::GetVkQueueFamilyIndex() const
	{
		return m_impl->queueFamilyIndex;
	}

	void
	VulkanCommandQueue::WaitUntilIdle() const
	{
		OYL_PROFILE_FUNCTION();

		m_impl->queue.waitIdle();
	}

	bool
	VulkanCommandQueue::Submit(const SubmitParams& a_params) const
	{
		OYL_PROFILE_FUNCTION();

		auto& vkCommandBuffer = dynamic_cast<const VulkanCommandBuffer&>(a_params.commandBuffer).GetVkCommandBuffer();
		vk::Semaphore vkWaitSemaphore = static_cast<VulkanSemaphoreId>(a_params.waitSemaphore);
		vk::Semaphore vkSignalSemaphore = static_cast<VulkanSemaphoreId>(a_params.signalSemaphore);
		vk::Fence vkFence = static_cast<VulkanFenceId>(a_params.fence);

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
	VulkanCommandQueue::Present(const PresentParams& a_params) const
	{
		OYL_PROFILE_FUNCTION();

		vk::Semaphore vkWaitSemaphore = static_cast<VulkanSemaphoreId>(a_params.waitSemaphore);
		auto& vkSwapChain = dynamic_cast<const VulkanSwapChain&>(a_params.swapChain).GetVkSwapChain();
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
