#pragma once

#include <vulkan/vulkan_raii.hpp>

#include <cstddef>

#include <Rendering/CommandQueue.h>

namespace vk::raii
{
	class Queue;
}

namespace Oyl
{
	class VulkanDevice;
	class VulkanCommandBuffer;
	class VulkanSwapChain;

	class VulkanCommandQueue : public ICommandQueue
	{
	public:
		VulkanCommandQueue() = default;
		VulkanCommandQueue(nullptr_t) {}

		VulkanCommandQueue(const VulkanDevice& a_device, const CommandQueueCreateParams& a_params);

		DEFAULT_MOVE(VulkanCommandQueue);
		NO_COPY(VulkanCommandQueue);

		virtual
		~VulkanCommandQueue();

		void
		Destroy() override;

		bool
		IsValid() const override { return *m_vkQueue; }

		void
		WaitUntilIdle() const override;

		bool
		Submit(const CommandQueueSubmitParams& a_params) const override;

		bool
		Present(const CommandQueuePresentParams& a_params) const override;

		const vk::raii::Queue&
		GetVkQueue() const { return m_vkQueue; }

		uint32
		GetVkQueueFamilyIndex() const { return m_queueFamilyIndex; }

	private:
		vk::raii::Queue m_vkQueue = nullptr;
		uint32 m_queueFamilyIndex = 0;
	};
}
