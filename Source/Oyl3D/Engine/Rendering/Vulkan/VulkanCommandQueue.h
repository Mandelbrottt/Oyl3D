#pragma once

#include "VulkanFence.h"
#include "VulkanSemaphore.h"

#include "Rendering/CommandQueue.h"

namespace vk::raii
{
	class Queue;
}

namespace Oyl::Rendering
{
	class VulkanDevice;
	class VulkanCommandBuffer;
	class VulkanSwapChain;

	class OYL_RENDERING_API VulkanCommandQueue : public CommandQueue
	{
	public:
		VulkanCommandQueue(nullptr_t);

		VulkanCommandQueue(const VulkanDevice& a_device, const CreateParams& a_params);

		NO_COPY(VulkanCommandQueue);

		VulkanCommandQueue(VulkanCommandQueue&& a_other) noexcept;
		VulkanCommandQueue&
		operator =(VulkanCommandQueue&& a_other) noexcept;

		virtual
		~VulkanCommandQueue();

		void
		Destroy() override;

		bool
		IsValid() const override;

		const vk::raii::Queue&
		GetVkQueue() const;

		uint32
		GetVkQueueFamilyIndex() const;

		void
		WaitUntilIdle() const override;

		bool
		Submit(const SubmitParams& a_params) const override;

		bool
		Present(const PresentParams& a_params) const override;

	private:
		struct Impl;
		std::unique_ptr<Impl> m_impl;
	};

	using VulkanCommandQueueHandle = PImpl<VulkanCommandQueue>;
}
