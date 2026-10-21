#pragma once

#include <Core/Types/Primitives.h>

#include "IDeviceObject.h"

namespace Oyl
{
	struct SemaphoreId : IDeviceObjectId<SemaphoreId> {};

	class ISemaphore : public IDeviceObject<SemaphoreId>
	{
	protected:
		ISemaphore() = default;

		DEFAULT_MOVE(ISemaphore);

	public:
		NO_COPY(ISemaphore);

		virtual
		~ISemaphore() = 0;
	};

	inline
	ISemaphore::~ISemaphore() {}

	using SemaphoreHandle = IObjectHandle<ISemaphore>;
}
