#pragma once

#include <Core/UniqueHandle.h>

namespace Oyl::Rendering
{
	template<typename THandle>
	class IDeviceObject : public IUniqueHandle
	{
	protected:
		IDeviceObject() = default;

		DEFAULT_MOVE(IDeviceObject);

	public:
		NO_COPY(IDeviceObject);

		virtual
		~IDeviceObject() = default;

		THandle
		GetHandle() const
		{
			return GetHandleImpl();
		}

		operator THandle()
		{
			return GetHandle();
		}

	protected:
		virtual
		THandle
		GetHandleImpl() const = 0;
	};
}
