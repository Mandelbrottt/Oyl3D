#include "VulkanCommandQueue.h"

#include <vulkan/vulkan_raii.hpp>

#include "VulkanCommandBuffer.h"
#include "VulkanDevice.h"
#include "VulkanSwapChain.h"

namespace Oyl::Rendering::Vulkan
{
	struct CommandQueue::Impl
	{
		vk::raii::Queue queue = nullptr;
		uint32 queueFamilyIndex;
	};

	CommandQueue::CommandQueue()
		: m_impl(nullptr) {}

	CommandQueue::CommandQueue(const Device& a_device, const CreateParams& a_params)
		: m_impl(std::make_unique<Impl>())
	{
		auto& vkDevice = a_device.GetVkDevice();
		m_impl->queueFamilyIndex = a_params.queueFamilyIndex;
		m_impl->queue = vk::raii::Queue(vkDevice, m_impl->queueFamilyIndex, 0);
	}

	CommandQueue::CommandQueue(CommandQueue&& a_other) noexcept
	{
		*this = std::move(a_other);
	}

	CommandQueue&
	CommandQueue::operator=(CommandQueue&& a_other) noexcept
	{
		if (this != &a_other)
		{
			m_impl = std::move(a_other.m_impl);
		}
		return *this;
	}

	CommandQueue::~CommandQueue()
	{
		CommandQueue::Destroy();
	}

	void
	CommandQueue::Destroy()
	{
		if (!IsValid())
			return;

		m_impl->queue.clear();
	}

	bool
	CommandQueue::IsValid() const
	{
		return m_impl
		       && *m_impl->queue;
	}

	const vk::raii::Queue&
	CommandQueue::GetVkQueue() const
	{
		return m_impl->queue;
	}

	uint32
	CommandQueue::GetVkQueueFamilyIndex() const
	{
		return m_impl->queueFamilyIndex;
	}

	void
	CommandQueue::Submit(const ICommandQueue::SubmitParams& a_params)
	{
		Submit(
			SubmitParams {
				.commandBuffer = dynamic_cast<const CommandBuffer&>(a_params.commandBuffer),
				.waitSemaphore = a_params.waitSemaphore,
				.signalSemaphore = a_params.signalSemaphore,
				.fence = a_params.fence
			}
		);
	}

	void
	CommandQueue::Submit(const SubmitParams& a_params) const
	{
		OYL_PROFILE_FUNCTION();

		auto& vkCommandBuffer = a_params.commandBuffer.GetVkCommandBuffer();
		vk::Semaphore vkWaitSemaphore = a_params.waitSemaphore;
		vk::Semaphore vkSignalSemaphore = a_params.signalSemaphore;
		vk::Fence vkFence = a_params.fence;

		vk::PipelineStageFlags waitDestinationStageMask = vk::PipelineStageFlagBits::eColorAttachmentOutput;
		const vk::SubmitInfo submitInfo {
			.waitSemaphoreCount = 1,
			.pWaitSemaphores = &vkWaitSemaphore,
			.pWaitDstStageMask = &waitDestinationStageMask,
			.commandBufferCount = 1,
			.pCommandBuffers = &*vkCommandBuffer,
			.signalSemaphoreCount = 1,
			.pSignalSemaphores = &vkSignalSemaphore
		};
		m_impl->queue.submit(submitInfo, vkFence);
	}

	void
	CommandQueue::Present(const ICommandQueue::PresentParams& a_params)
	{
		Present(
			PresentParams {
				.waitSemaphore = a_params.waitSemaphore,
				.swapChain = dynamic_cast<const SwapChain&>(a_params.swapChain),
			}
		);
	}

	bool
	CommandQueue::Present(const PresentParams& a_params) const
	{
		OYL_PROFILE_FUNCTION();

		vk::Semaphore vkWaitSemaphore = a_params.waitSemaphore;
		auto& vkSwapChain = a_params.swapChain.GetVkSwapChain();
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
