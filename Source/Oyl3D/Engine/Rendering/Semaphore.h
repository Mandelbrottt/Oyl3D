#pragma once

#include "DeviceObject.h"

namespace Oyl::Rendering
{
	//struct SemaphoreHandle
	//{
	//	SemaphoreHandle() = default;

	//	SemaphoreHandle(const SemaphoreHandle& a_other) = default;
	//	SemaphoreHandle&
	//	operator =(const SemaphoreHandle& a_other) = default;

	//	SemaphoreHandle(SemaphoreHandle&& a_other) = default;
	//	SemaphoreHandle&
	//	operator =(SemaphoreHandle&& a_other) noexcept = default;

	//	SemaphoreHandle(std::nullptr_t) {}

	//protected:
	//	using Handle = struct _Handle*;
	//	Handle m_handle = nullptr;
	//};

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

		virtual
		const SemaphoreHandle&
		GetHandle() const = 0;
	};
}
