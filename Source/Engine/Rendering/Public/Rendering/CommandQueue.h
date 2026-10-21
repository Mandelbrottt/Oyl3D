#pragma once

#include <Core/Interfaces/IDestroyable.h>
#include <Core/Preprocessor/Common.h>
#include <Core/Types/Primitives.h>

#include "CommandBuffer.h"
#include "Fence.h"
#include "Semaphore.h"
#include "SwapChain.h"

namespace Oyl
{
	struct CommandQueueCreateParams
	{
		uint32 queueFamilyIndex;
	};

	struct CommandQueueSubmitParams
	{
		const ICommandBuffer& commandBuffer;
		SemaphoreId waitSemaphore = {};
		SemaphoreId signalSemaphore = {};
		FenceId fence = {};
		PipelineStageFlags waitDestinationStageMask = {};
	};

	struct CommandQueuePresentParams
	{
		const ISwapChain& swapChain;
		SemaphoreId waitSemaphore = {};
	};

	class ICommandQueue : public IDestroyable
	{
	protected:
		ICommandQueue() = default;
		DEFAULT_MOVE(ICommandQueue);

	public:
		NO_COPY(ICommandQueue);

		virtual
		~ICommandQueue() = 0;

		virtual
		void
		WaitUntilIdle() const = 0;

		virtual
		bool
		Submit(const CommandQueueSubmitParams& a_params) const = 0;

		virtual
		bool
		Present(const CommandQueuePresentParams& a_params) const = 0;
	};

	inline
	ICommandQueue::~ICommandQueue() {}

	using CommandQueueHandle = IObjectHandle<ICommandQueue>;
}
