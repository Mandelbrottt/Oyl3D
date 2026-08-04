#pragma once

#include <Core/UniqueHandle.h>

#include "Enums.h"
#include "Fence.h"
#include "Semaphore.h"

namespace Oyl::Rendering
{
	class CommandBufferImpl;
	class SwapChainImpl;

	class CommandQueueImpl : public IUniqueHandle
	{
	protected:
		CommandQueueImpl() = default;

	public:
		struct CreateParams
		{
			uint32 queueFamilyIndex;
		};

		NO_MOVE(CommandQueueImpl);
		NO_COPY(CommandQueueImpl);

		virtual
		~CommandQueueImpl() = default;

		virtual
		void
		WaitUntilIdle() const = 0;

		struct SubmitParams
		{
			const CommandBufferImpl& commandBuffer;
			SemaphoreHandle waitSemaphore = {};
			SemaphoreHandle signalSemaphore = {};
			FenceHandle fence = {};
			PipelineStageFlags waitDestinationStageMask = {};
		};

		virtual
		bool
		Submit(const SubmitParams& a_params) const = 0;

		struct PresentParams
		{
			const SwapChainImpl& swapChain;
			SemaphoreHandle waitSemaphore = {};
		};

		virtual
		bool
		Present(const PresentParams& a_params) const = 0;
	};

	using CommandQueue = PImpl<CommandQueueImpl>;
}
