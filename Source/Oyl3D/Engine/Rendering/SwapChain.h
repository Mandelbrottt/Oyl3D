#pragma once

#include "Fence.h"
#include "Semaphore.h"

#include "Core/UniqueHandle.h"

namespace Oyl::Rendering
{
	class ISwapChain : public IUniqueHandle
	{
	protected:
		ISwapChain() = default;

		DEFAULT_MOVE(ISwapChain);

	public:
		NO_COPY(ISwapChain);

		virtual
		~ISwapChain() = default;

		virtual
		void
		Recreate() = 0;

		virtual
		bool
		AcquireNextImage(SemaphoreHandle a_semaphore, FenceHandle a_fence) = 0;

		bool
		AcquireNextImage(const ISemaphore& a_semaphore, const IFence& a_fence)
		{
			return AcquireNextImage(a_semaphore.GetHandle(), a_fence.GetHandle());
		}

		virtual
		uint32
		GetCurrentImageIndex() const = 0;
	};
}
