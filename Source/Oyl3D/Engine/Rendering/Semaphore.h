#pragma once

#include "DeviceObject.h"

#include "Core/Typedefs.h"

namespace Oyl::Rendering
{
	struct SemaphoreHandle : OpaqueHandle<SemaphoreHandle> {};

	class OYL_RENDERING_API ISemaphore : Internal::IDeviceObject
	{
	protected:
		ISemaphore() = default;

		ISemaphore(ISemaphore&& a_other) noexcept = default;
		ISemaphore&
		operator =(ISemaphore&& a_other) noexcept = default;

	public:
		ISemaphore(const ISemaphore& a_other) = delete;
		ISemaphore&
		operator =(const ISemaphore& a_other) = delete;

		virtual
		~ISemaphore() = default;

		SemaphoreHandle
		GetHandle() const
		{
			return GetHandleImpl();
		}

	protected:
		virtual
		SemaphoreHandle
		GetHandleImpl() const = 0;
	};
}
