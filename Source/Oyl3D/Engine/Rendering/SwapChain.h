#pragma once

#include <Core/UniqueHandle.h>
#include <Core/Math/Vector2.h>

#include "Fence.h"
#include "Semaphore.h"

namespace Oyl::Rendering
{
	class SwapChain : public IUniqueHandle
	{
	protected:
		SwapChain() = default;

	public:
		NO_MOVE(SwapChain);
		NO_COPY(SwapChain);

		virtual
		~SwapChain() = default;

		virtual
		Vector2u
		GetSize() const = 0;

		virtual
		void
		Recreate() = 0;

		virtual
		bool
		AcquireNextImage(SemaphoreId a_semaphore, FenceId a_fence) = 0;

		bool
		AcquireNextImage(const SemaphoreHandle& a_semaphore, const FenceHandle& a_fence)
		{
			SemaphoreId semaphoreId = {};
			if (a_semaphore)
				semaphoreId = a_semaphore->GetId();

			FenceId fenceId = {};
			if (a_fence)
				fenceId = a_fence->GetId();

			return AcquireNextImage(semaphoreId, fenceId);
		}

		virtual
		uint32
		GetImageCount() const = 0;

		virtual
		uint32
		GetCurrentImageIndex() const = 0;

		virtual
		ImageId
		GetImageId(uint32 a_index) const = 0;

		virtual
		ImageId
		GetCurrentImageId() const = 0;
	};

	using SwapChainHandle = PImpl<SwapChain>;
}
