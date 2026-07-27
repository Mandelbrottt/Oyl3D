#pragma once

#include <Core/UniqueHandle.h>

namespace Oyl::Rendering
{
	template<typename THandle>
	class DeviceObject : public IUniqueHandle
	{
	protected:
		DeviceObject() = default;

		DEFAULT_MOVE(DeviceObject);

	public:
		NO_COPY(DeviceObject);

		virtual
		~DeviceObject() = default;

		THandle
		GetHandle() const
		{
			return GetHandleImpl();
		}

	protected:
		virtual
		THandle
		GetHandleImpl() const = 0;
	};
}
