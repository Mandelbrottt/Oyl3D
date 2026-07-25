#pragma once

#include "DeviceObject.h"
#include "Fence.h"
#include "Semaphore.h"

namespace Oyl::Rendering
{
	class OYL_RENDERING_API SwapChain : public Internal::IDeviceObject
	{
	protected:
		SwapChain() = default;

		SwapChain(SwapChain&& a_other) noexcept = default;
		SwapChain&
		operator =(SwapChain&& a_other) noexcept = default;

	public:
		virtual
		~SwapChain() = default;

		virtual
		void
		Recreate() = 0;

		virtual
		bool
		AcquireNextImage(SemaphoreHandle a_semaphore, FenceHandle a_fenceHandle) = 0;

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
