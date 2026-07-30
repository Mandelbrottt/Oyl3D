#pragma once

#include "VulkanFence.h"
#include "VulkanSemaphore.h"

#include "Rendering/CommandQueue.h"

namespace vk::raii
{
	class Queue;
}

namespace Oyl::Rendering::Vulkan
{
	class Device;
	class CommandBuffer;
	class SwapChain;

	class OYL_RENDERING_API CommandQueue : public ICommandQueue
	{
	public:
		CommandQueue();

		struct CreateParams
		{
			uint32 queueFamilyIndex;
		};

		explicit
		CommandQueue(const Device& a_device, const CreateParams& a_params);

		NO_COPY(CommandQueue);

		CommandQueue(CommandQueue&& a_other) noexcept;
		CommandQueue&
		operator =(CommandQueue&& a_other) noexcept;

		virtual
		~CommandQueue();

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

		struct SubmitParams
		{
			const CommandBuffer& commandBuffer;

			SemaphoreHandle waitSemaphore = {};
			SemaphoreHandle signalSemaphore = {};
			FenceHandle fence = {};

			vk::PipelineStageFlags waitDestinationStageMask = {};
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

	inline
	vk::QueueFlags
	ToVkQueueFlags(CommandQueueFlags a_flags)
	{
		vk::QueueFlags result {};

		auto checkFlagBit = [&result, &a_flags](vk::QueueFlagBits a_vkBit, CommandQueueFlagBits a_oylBit)
		{
			if (a_flags & a_oylBit)
			{
				result |= a_vkBit;

				// If in non distribution, remove a bit from a_in for a later assert
			#if !defined(OYL_DISTRIBUTION)
				a_flags &= ~((CommandQueueFlags) a_oylBit);
			#endif
			}
		};

		checkFlagBit(vk::QueueFlagBits::eGraphics, CommandQueueFlagBits::Graphics);
		checkFlagBit(vk::QueueFlagBits::eCompute, CommandQueueFlagBits::Compute);
		checkFlagBit(vk::QueueFlagBits::eTransfer, CommandQueueFlagBits::Transfer);

		OYL_ASSERT(!a_flags, "Missing CommandQueueFlagBits check!");
		return result;
	}

	inline
	CommandQueueFlags
	ToCommandQueueFlags(vk::QueueFlags a_flags)
	{
		CommandQueueFlags result;

		auto checkFlagBit = [&](auto a_oylBit, auto a_vkBit)
		{
			if (a_flags & a_vkBit)
			{
				result |= a_oylBit;
			}
		};

		checkFlagBit(CommandQueueFlagBits::Graphics, vk::QueueFlagBits::eGraphics);
		checkFlagBit(CommandQueueFlagBits::Compute, vk::QueueFlagBits::eCompute);
		checkFlagBit(CommandQueueFlagBits::Transfer, vk::QueueFlagBits::eTransfer);

		return result;
	}
}
