#pragma once

#include <Core/Typedefs.h>

#include "DeviceObject.h"

namespace Oyl::Rendering
{
	struct FenceHandle : OpaqueHandle<FenceHandle> {};

	class IFence : public IDeviceObject<FenceHandle>
	{
	protected:
		IFence() = default;

		DEFAULT_MOVE(IFence);

	public:
		NO_COPY(IFence);

		virtual
		~IFence() = default;

		void
		virtual Reset() = 0;

		bool
		virtual Wait(uint64 a_timeout = std::numeric_limits<uint64>::max()) = 0;
	};
}
