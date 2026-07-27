#pragma once

#include <Core/Typedefs.h>

#include "DeviceObject.h"

namespace Oyl::Rendering
{
	struct FenceHandle : OpaqueHandle<FenceHandle> {};

	class IFence : public DeviceObject<FenceHandle>
	{
	protected:
		IFence() = default;

		DEFAULT_MOVE(IFence);

	public:
		NO_COPY(IFence);

		virtual
		~IFence() = default;

		virtual
		void
		Reset() = 0;

		virtual
		bool
		Wait(uint64 a_timeout = std::numeric_limits<uint64>::max()) = 0;
	};
}
