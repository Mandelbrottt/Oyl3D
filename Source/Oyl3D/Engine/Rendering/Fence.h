#pragma once

#include "DeviceObject.h"

namespace Oyl::Rendering
{
	struct FenceHandle : OpaqueHandle<FenceHandle> {};

	class OYL_RENDERING_API IFence : Internal::IDeviceObject
	{
	protected:
		IFence() = default;

		IFence(IFence&& a_other) noexcept = default;
		IFence&
		operator =(IFence&& a_other) noexcept = default;

	public:
		IFence(const IFence& a_other) = delete;
		IFence&
		operator =(const IFence& a_other) = delete;

		virtual
		~IFence() = default;

		virtual
		const FenceHandle&
		GetHandle() const = 0;
	};
}
