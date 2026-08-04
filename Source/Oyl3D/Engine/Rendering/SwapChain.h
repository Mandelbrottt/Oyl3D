#pragma once

#include <Core/UniqueHandle.h>
#include <Core/Math/Vector2.h>

#include "Fence.h"
#include "Semaphore.h"

namespace Oyl::Rendering
{
	class SwapChainImpl : public IUniqueHandle
	{
	protected:
		SwapChainImpl() = default;

	public:
		NO_MOVE(SwapChainImpl);
		NO_COPY(SwapChainImpl);

		virtual
		~SwapChainImpl() = default;

		virtual
		Vector2u
		GetSize() const = 0;

		virtual
		void
		Recreate() = 0;

		virtual
		bool
		AcquireNextImage(SemaphoreHandle a_semaphore, FenceHandle a_fence) = 0;

		bool
		AcquireNextImage(const Semaphore& a_semaphore, const Fence& a_fence)
		{
			SemaphoreHandle semaphoreHandle = {};
			if (a_semaphore)
				semaphoreHandle = a_semaphore->GetHandle();

			FenceHandle fenceHandle = {};
			if (a_fence)
				fenceHandle = a_fence->GetHandle();

			return AcquireNextImage(semaphoreHandle, fenceHandle);
		}

		virtual
		uint32
		GetImageCount() const = 0;

		virtual
		uint32
		GetCurrentImageIndex() const = 0;
	};

	using SwapChain = PImpl<SwapChainImpl>;
}
