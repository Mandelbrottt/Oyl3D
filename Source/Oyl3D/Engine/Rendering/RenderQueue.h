#pragma once

#include <Core/UniqueHandle.h>

#include "Fence.h"
#include "Semaphore.h"

namespace Oyl::Rendering
{
	class ICommandBuffer;
	class ISwapChain;

	class IRenderQueue : public IUniqueHandle
	{
	protected:
		IRenderQueue() = default;

		DEFAULT_MOVE(IRenderQueue);

	public:
		NO_COPY(IRenderQueue);

		virtual
		~IRenderQueue() = default;

		struct SubmitParams
		{
			const ICommandBuffer& commandBuffer;
			SemaphoreHandle waitSemaphore;
			SemaphoreHandle signalSemaphore;
			FenceHandle fence;
		};

		virtual
		void
		Submit(const SubmitParams& a_params) = 0;

		struct PresentParams
		{
			SemaphoreHandle waitSemaphore;
			const ISwapChain& swapChain;
		};

		virtual
		void
		Present(const PresentParams& a_params) = 0;
	};
}
