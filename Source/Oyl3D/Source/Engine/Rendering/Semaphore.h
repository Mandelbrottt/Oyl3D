#pragma once

#include <Core/Types/Primitives.h>

#include "IDeviceObject.h"

namespace Oyl::Rendering
{
	struct SemaphoreId : IDeviceObjectId<SemaphoreId> {};

	class Semaphore : public IDeviceObject<SemaphoreId>
	{
	protected:
		Semaphore() = default;

		DEFAULT_MOVE(Semaphore);

	public:
		NO_COPY(Semaphore);

		virtual
		~Semaphore() = 0;
	};

	inline
	Semaphore::~Semaphore() {}
}
