#pragma once

#include <Application/ApplicationWindow.h>

#include <Core/Events/Event.h>
#include <Core/Interfaces/IDestroyable.h>
#include <Core/Math/Vector.h>
#include <Core/Preprocessor/Common.h>

#include "Fence.h"
#include "IDeviceObject.h"
#include "Image.h"
#include "Semaphore.h"

namespace Oyl
{
	class ISwapChain : public IDestroyable
	{
	protected:
		ISwapChain() = default;
		DEFAULT_MOVE(ISwapChain);

	public:
		NO_COPY(ISwapChain);

		using OnRecreateDelegate = Delegate<void(const ISwapChain&)>;

		virtual
		~ISwapChain() = 0;

		virtual
		void
		Recreate() = 0;

		virtual
		void
		AddOnRecreateCallback(OnRecreateDelegate a_callback) = 0;

		virtual
		void
		RemoveOnRecreateCallback(OnRecreateDelegate a_callback) = 0;

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
		Vector2u
		GetSize() const = 0;

		virtual
		uint32
		GetMinImageCount() const = 0;

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

	inline
	ISwapChain::~ISwapChain() {}

	using SwapChainHandle = IObjectHandle<ISwapChain>;
}
