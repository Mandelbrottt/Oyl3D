#pragma once

#include <Core/PImpl.h>
#include <Core/Typedefs.h>

#include "DeviceObject.h"

namespace Oyl::Rendering
{
	struct SemaphoreId : OpaqueHandle<SemaphoreId> {};

	class Semaphore : public IDeviceObject<SemaphoreId>
	{
	protected:
		Semaphore() = default;

		DEFAULT_MOVE(Semaphore);

	public:
		NO_COPY(Semaphore);

		virtual
		~Semaphore() = default;
	};

	using SemaphoreHandle = PImpl<Semaphore>;
}
