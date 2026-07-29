#pragma once

#include <Core/EnumFlags.h>
#include <Core/UniqueHandle.h>

#include "Fence.h"
#include "Semaphore.h"

namespace Oyl::Rendering
{
	class ICommandBuffer;
	class ISwapChain;

	class ICommandQueue : public IUniqueHandle
	{
	protected:
		ICommandQueue() = default;

		DEFAULT_MOVE(ICommandQueue);

	public:
		NO_COPY(ICommandQueue);

		virtual
		~ICommandQueue() = default;

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
