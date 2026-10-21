#include "VulkanCommandQueue.h"

#include "VulkanDevice.h"

namespace Oyl
{
	VulkanCommandQueue::VulkanCommandQueue(const VulkanDevice& a_device, const CommandQueueCreateParams& a_params)
	{
		auto& vkDevice = a_device.GetVkDevice();
		m_queueFamilyIndex = a_params.queueFamilyIndex;
		m_vkQueue = vk::raii::Queue(vkDevice, m_queueFamilyIndex, 0);
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

		m_vkQueue.clear();
	}

	void
	VulkanCommandQueue::WaitUntilIdle() const
	{
		OYL_PROFILE_FUNCTION();

		m_vkQueue.waitIdle();
	}

	bool
	VulkanCommandQueue::Submit(const CommandQueueSubmitParams& a_params) const
	{
		OYL_PROFILE_FUNCTION();

		auto& vkCommandBuffer = static_cast<const VulkanCommandBuffer&>(a_params.commandBuffer).GetVkCommandBuffer();
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

		m_vkQueue.submit(submitInfo, vkFence);
		return true;
	}

	bool
	VulkanCommandQueue::Present(const CommandQueuePresentParams& a_params) const
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

		auto result = m_vkQueue.presentKHR(presentInfoKHR);
		if (result == vk::Result::eSuboptimalKHR || result == vk::Result::eErrorOutOfDateKHR)
			return false;

		return true;
	}
}
