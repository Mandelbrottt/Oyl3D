#pragma once

#include "VulkanFence.h"
#include "VulkanSemaphore.h"

#include "Rendering/RenderQueue.h"

namespace vk::raii
{
	class Queue;
}

namespace Oyl::Rendering::Vulkan
{
	class Device;
	class CommandBuffer;
	class SwapChain;

	class OYL_RENDERING_API RenderQueue : public ICommandQueue
	{
	public:
		RenderQueue();

		struct CreateParams
		{
			const Device& device;
			uint32 queueFamilyIndex;
		};

		explicit
		RenderQueue(const CreateParams& a_params);

		NO_COPY(RenderQueue);

		RenderQueue(RenderQueue&& a_other) noexcept;
		RenderQueue&
		operator =(RenderQueue&& a_other) noexcept;

		virtual
		~RenderQueue();

		void
		Destroy() override;

		bool
		IsValid() const override;

		const vk::raii::Queue&
		GetVkQueue() const;

		struct SubmitParams
		{
			const CommandBuffer& commandBuffer;
			SemaphoreHandle waitSemaphore;
			SemaphoreHandle signalSemaphore;
			FenceHandle fence;
		};

		void
		Submit(const SubmitParams& a_params) const;

		struct PresentParams
		{
			SemaphoreHandle waitSemaphore;
			const SwapChain& swapChain;
		};

		bool
		Present(const PresentParams& a_params) const;

	protected:
		void
		Submit(const ICommandQueue::SubmitParams& a_params) override;

		void
		Present(const ICommandQueue::PresentParams& a_params) override;

	private:
		struct Impl;
		std::unique_ptr<Impl> m_impl;
	};
}
