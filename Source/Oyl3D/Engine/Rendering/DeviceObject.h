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
		GetId() const
		{
			return GetIdImpl();
		}

		operator THandle()
		{
			return GetId();
		}

	protected:
		virtual
		THandle
		GetIdImpl() const = 0;
	};
}
