#pragma once

#include <Core/Typedefs.h>
#include <Core/UniqueHandle.h>

namespace Oyl::Rendering
{
	struct SemaphoreHandle : OpaqueHandle<SemaphoreHandle> {};

	class ISemaphore : public IUniqueHandle
	{
	protected:
		ISemaphore() = default;

		DEFAULT_MOVE(ISemaphore);

	public:
		NO_COPY(ISemaphore);

		virtual
		~ISemaphore() = default;

		SemaphoreHandle
		GetHandle() const
		{
			return GetHandleImpl();
		}

		operator SemaphoreHandle() const
		{
			return GetHandle();
		}

	protected:
		virtual
		SemaphoreHandle
		GetHandleImpl() const = 0;
	};
}
