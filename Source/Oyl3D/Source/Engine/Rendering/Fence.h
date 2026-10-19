#pragma once

#include <Core/Types/Primitives.h>

#include "IDeviceObject.h"

namespace Oyl
{
	struct FenceId : IDeviceObjectId<FenceId> {};

	class Fence : public IDeviceObject<FenceId>
	{
	protected:
		Fence() = default;

		DEFAULT_MOVE(Fence);

	public:
		NO_COPY(Fence);

		virtual
		~Fence() = 0;

		void
		virtual
		Reset() = 0;

		bool
		virtual
		Wait(uint64 a_timeout = INT64_MAX) = 0;
	};

	inline
	Fence::~Fence() {}
}
