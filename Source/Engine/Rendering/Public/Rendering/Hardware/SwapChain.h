#pragma once

#include <Application/ApplicationWindow.h>

#include <Core/Events/Event.h>
#include <Core/Interfaces/IDestroyable.h>
#include <Core/Math/Vector.h>
#include <Core/Preprocessor/Common.h>

#include "Fence.h"
#include "RenderDeviceObject.h"
#include "Image.h"
#include "Semaphore.h"

namespace Oyl
{
	class IRdoSwapChain : public IDestroyable
	{
	protected:
		IRdoSwapChain() = default;
		DEFAULT_MOVE(IRdoSwapChain);

	public:
		NO_COPY(IRdoSwapChain);

		using OnRecreateDelegate = Delegate<void(const IRdoSwapChain&)>;

		virtual
		~IRdoSwapChain() = 0;

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
		AcquireNextImage(RdoSemaphoreId a_semaphore, RdoFenceId a_fence) = 0;

		bool
		AcquireNextImage(const RdoSemaphoreHandle& a_semaphore, const RdoFenceHandle& a_fence)
		{
			RdoSemaphoreId semaphoreId = {};
			if (a_semaphore)
				semaphoreId = a_semaphore->GetId();

			RdoFenceId fenceId = {};
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
		RdoImageId
		GetImageId(uint32 a_index) const = 0;

		virtual
		RdoImageId
		GetCurrentImageId() const = 0;
	};

	inline
	IRdoSwapChain::~IRdoSwapChain() {}

	using RdoSwapChainHandle = IRenderDeviceObjectHandle<IRdoSwapChain>;
}
