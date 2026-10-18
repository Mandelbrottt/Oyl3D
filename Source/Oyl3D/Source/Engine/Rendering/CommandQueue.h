#pragma once

#include <Core/Interfaces/IDestroyable.h>
#include <Core/Preprocessor/Common.h>
#include <Core/Types/Primitives.h>

namespace Oyl
{
	class CommandBuffer;
	class ISwapChain;

	struct CommandQueueCreateParams
	{
		uint32 queueFamilyIndex;
	};

	struct CommandQueueSubmitParams
	{
		const CommandBuffer& commandBuffer;
		//SemaphoreId waitSemaphore = {};
		//SemaphoreId signalSemaphore = {};
		//FenceId fence = {};
		//PipelineStageFlags waitDestinationStageMask = {};
	};

	struct CommandQueuePresentParams
	{
		const ISwapChain& swapChain;
		//SemaphoreId waitSemaphore = {};
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
}
