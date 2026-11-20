#pragma once

#include <vulkan/vulkan_raii.hpp>

#include <cstddef>

#include <Rendering/Hardware/RdoCommandQueue.h>

#include "VulkanCommandBuffer.h"
#include "VulkanSwapChain.h"

namespace Oyl
{
	class VulkanDevice;

	class RENDERING_VULKAN_EXPORT VulkanCommandQueue : public IRdoCommandQueue
	{
	public:
		VulkanCommandQueue() = default;
		VulkanCommandQueue(nullptr_t) {}

		VulkanCommandQueue(const VulkanDevice& a_device, const RdoCommandQueueCreateParams& a_params);

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
		Submit(const RdoCommandQueueSubmitParams& a_params) const override;

		bool
		Present(const RdoCommandQueuePresentParams& a_params) const override;

		const vk::raii::Queue&
		GetVkQueue() const { return m_vkQueue; }

		uint32
		GetVkQueueFamilyIndex() const { return m_queueFamilyIndex; }

	private:
		vk::raii::Queue m_vkQueue = nullptr;
		uint32 m_queueFamilyIndex = 0;
	};
}
