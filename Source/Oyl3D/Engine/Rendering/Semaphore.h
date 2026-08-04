#pragma once

#include <Core/PImpl.h>
#include <Core/Typedefs.h>

#include "DeviceObject.h"

namespace Oyl::Rendering
{
	struct SemaphoreHandle : OpaqueHandle<SemaphoreHandle> {};

	class SemaphoreImpl : public IDeviceObject<SemaphoreHandle>
	{
	protected:
		SemaphoreImpl() = default;

		DEFAULT_MOVE(SemaphoreImpl);

	public:
		NO_COPY(SemaphoreImpl);

		virtual
		~SemaphoreImpl() = default;
	};

	using Semaphore = PImpl<SemaphoreImpl>;
}
