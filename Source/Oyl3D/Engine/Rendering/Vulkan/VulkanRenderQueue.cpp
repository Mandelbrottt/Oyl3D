#include "VulkanRenderQueue.h"

#include <vulkan/vulkan_raii.hpp>

#include "VulkanCommandBuffer.h"
#include "VulkanDevice.h"
#include "VulkanFence.h"
#include "VulkanSemaphore.h"
#include "VulkanSwapChain.h"

namespace Oyl::Rendering::Vulkan
{
	struct RenderQueue::Impl
	{
		vk::raii::Queue queue = nullptr;
	};

	RenderQueue::RenderQueue()
		: m_impl(nullptr) {}

	RenderQueue::RenderQueue(const CreateParams& a_params)
		: m_impl(std::make_unique<Impl>())
	{
		auto& vkDevice = a_params.device.GetVkDevice();
		auto queueFamilyIndex = a_params.queueFamilyIndex;
		m_impl->queue = vk::raii::Queue(vkDevice, queueFamilyIndex, 0);
	}

	RenderQueue::RenderQueue(RenderQueue&& a_other) noexcept
	{
		*this = std::move(a_other);
	}

	RenderQueue&
	RenderQueue::operator=(RenderQueue&& a_other) noexcept
	{
		if (this != &a_other)
		{
			m_impl = std::move(a_other.m_impl);
		}
		return *this;
	}

	RenderQueue::~RenderQueue()
	{
		RenderQueue::Destroy();
	}

	void
	RenderQueue::Destroy()
	{
		if (!IsValid())
			return;

		m_impl->queue.clear();
	}

	bool
	RenderQueue::IsValid() const
	{
		return m_impl
		       && *m_impl->queue;
	}

	const vk::raii::Queue&
	RenderQueue::GetVkQueue() const
	{
		return m_impl->queue;
	}

	void
	RenderQueue::Submit(const ICommandQueue::SubmitParams& a_params)
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
	RenderQueue::Submit(const SubmitParams& a_params) const
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
	RenderQueue::Present(const ICommandQueue::PresentParams& a_params)
	{
		Present(
			PresentParams {
				.waitSemaphore = a_params.waitSemaphore,
				.swapChain = dynamic_cast<const SwapChain&>(a_params.swapChain),
			}
		);
	}

	bool
	RenderQueue::Present(const PresentParams& a_params) const
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
