#pragma once

#include "DeviceObject.h"

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
		AcquireNextImage() = 0;

		virtual
		uint32
		GetCurrentImageIndex() const = 0;
	};
}
