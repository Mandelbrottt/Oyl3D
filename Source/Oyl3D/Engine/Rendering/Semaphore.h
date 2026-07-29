#pragma once

#include <Core/Typedefs.h>

#include "DeviceObject.h"

namespace Oyl::Rendering
{
	struct SemaphoreHandle : OpaqueHandle<SemaphoreHandle> {};

	class ISemaphore : public IDeviceObject<SemaphoreHandle>
	{
	protected:
		ISemaphore() = default;

		DEFAULT_MOVE(ISemaphore);

	public:
		NO_COPY(ISemaphore);

		virtual
		~ISemaphore() = default;
	};
}
