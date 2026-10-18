#pragma once

#include <Application/ApplicationWindow.h>

#include <Core/Interfaces/IDestroyable.h>
#include <Core/Math/Vector.h>
#include <Core/Preprocessor/Common.h>

namespace Oyl
{
	class ISwapChain : public IDestroyable
	{
	protected:
		ISwapChain() = default;
		DEFAULT_MOVE(ISwapChain);

	public:
		NO_COPY(ISwapChain);

		virtual
		~ISwapChain() = 0;

		virtual
		Vector2u
		GetSize() const = 0;

		virtual
		void
		Recreate() = 0;

		//virtual
		//bool
		//AcquireNextImage(SemaphoreId a_semaphore, FenceId a_fence) = 0;

		//bool
		//AcquireNextImage(const SemaphoreHandle& a_semaphore, const FenceHandle& a_fence)
		//{
		//	SemaphoreId semaphoreId = {};
		//	if (a_semaphore)
		//		semaphoreId = a_semaphore->GetId();

		//	FenceId fenceId = {};
		//	if (a_fence)
		//		fenceId = a_fence->GetId();

		//	return AcquireNextImage(semaphoreId, fenceId);
		//}

		//virtual
		//uint32
		//GetImageCount() const = 0;

		//virtual
		//uint32
		//GetCurrentImageIndex() const = 0;

		//virtual
		//ImageId
		//GetImageId(uint32 a_index) const = 0;

		//virtual
		//ImageId
		//GetCurrentImageId() const = 0;
	};

	inline
	ISwapChain::~ISwapChain() {}
}
