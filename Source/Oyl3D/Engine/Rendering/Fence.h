#pragma once

#include <Core/PImpl.h>
#include <Core/Typedefs.h>

#include "DeviceObject.h"

namespace Oyl::Rendering
{
	struct FenceId : OpaqueHandle<FenceId> {};

	class Fence : public IDeviceObject<FenceId>
	{
	protected:
		Fence() = default;

		DEFAULT_MOVE(Fence);

	public:
		NO_COPY(Fence);

		virtual
		~Fence() = default;

		void
		virtual Reset() = 0;

		bool
		virtual Wait(uint64 a_timeout = std::numeric_limits<uint64>::max()) = 0;
	};

	using FenceHandle = PImpl<Fence>;
}
