#pragma once

#include <Core/UniqueHandle.h>

#include "Enums.h"
#include "Fence.h"
#include "Semaphore.h"

namespace Oyl::Rendering
{
	class CommandBuffer;
	class SwapChain;

	class CommandQueue : public IUniqueHandle
	{
	protected:
		CommandQueue() = default;

	public:
		struct CreateParams
		{
			uint32 queueFamilyIndex;
		};

		NO_MOVE(CommandQueue);
		NO_COPY(CommandQueue);

		virtual
		~CommandQueue() = default;

		virtual
		void
		WaitUntilIdle() const = 0;

		struct SubmitParams
		{
			const CommandBuffer& commandBuffer;
			SemaphoreId waitSemaphore = {};
			SemaphoreId signalSemaphore = {};
			FenceId fence = {};
			PipelineStageFlags waitDestinationStageMask = {};
		};

		virtual
		bool
		Submit(const SubmitParams& a_params) const = 0;

		struct PresentParams
		{
			const SwapChain& swapChain;
			SemaphoreId waitSemaphore = {};
		};

		virtual
		bool
		Present(const PresentParams& a_params) const = 0;
	};

	using CommandQueueHandle = PImpl<CommandQueue>;
}
