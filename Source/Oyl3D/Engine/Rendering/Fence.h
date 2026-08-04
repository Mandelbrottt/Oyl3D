#pragma once

#include <Core/PImpl.h>
#include <Core/Typedefs.h>

#include "DeviceObject.h"

namespace Oyl::Rendering
{
	struct FenceHandle : OpaqueHandle<FenceHandle> {};

	class FenceImpl : public IDeviceObject<FenceHandle>
	{
	protected:
		FenceImpl() = default;

		DEFAULT_MOVE(FenceImpl);

	public:
		NO_COPY(FenceImpl);

		virtual
		~FenceImpl() = default;

		void
		virtual Reset() = 0;

		bool
		virtual Wait(uint64 a_timeout = std::numeric_limits<uint64>::max()) = 0;
	};

	using Fence = PImpl<FenceImpl>;
}
