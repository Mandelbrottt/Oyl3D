#pragma once

#include <Core/Interfaces/IDestroyable.h>
#include <Core/Preprocessor/Common.h>
#include <Core/Types/Primitives.h>

#include "RdoCommandBuffer.h"
#include "RdoFence.h"
#include "RdoSemaphore.h"
#include "RdoSwapChain.h"

namespace Oyl
{
	struct RdoCommandQueueCreateParams
	{
		uint32 queueFamilyIndex;
	};

	struct RdoCommandQueueSubmitParams
	{
		const IRdoCommandBuffer& commandBuffer;
		RdoSemaphoreId waitSemaphore = {};
		RdoSemaphoreId signalSemaphore = {};
		RdoFenceId fence = {};
		PipelineStageFlags waitDestinationStageMask = {};
	};

	struct RdoCommandQueuePresentParams
	{
		const IRdoSwapChain& swapChain;
		RdoSemaphoreId waitSemaphore = {};
	};

	class IRdoCommandQueue : public IDestroyable
	{
	protected:
		IRdoCommandQueue() = default;
		DEFAULT_MOVE(IRdoCommandQueue);

	public:
		NO_COPY(IRdoCommandQueue);

		virtual
		~IRdoCommandQueue() = 0;

		virtual
		void
		WaitUntilIdle() const = 0;

		virtual
		bool
		Submit(const RdoCommandQueueSubmitParams& a_params) const = 0;

		virtual
		bool
		Present(const RdoCommandQueuePresentParams& a_params) const = 0;
	};

	inline
	IRdoCommandQueue::~IRdoCommandQueue() {}

	using RdoCommandQueueHandle = IRenderDeviceObjectHandle<IRdoCommandQueue>;
}
