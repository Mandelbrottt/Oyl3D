#pragma once

#include <Core/Types/Primitives.h>

#include "RenderDeviceObject.h"

namespace Oyl
{
	struct RdoFenceId : IRenderDeviceObjectId<RdoFenceId> {};

	class IRdoFence : public IRenderDeviceObject<RdoFenceId>
	{
	protected:
		IRdoFence() = default;

		DEFAULT_MOVE(IRdoFence);

	public:
		NO_COPY(IRdoFence);

		virtual
		~IRdoFence() = 0;

		void
		virtual
		Reset() = 0;

		bool
		virtual
		Wait(uint64 a_timeout = INT64_MAX) = 0;
	};

	inline
	IRdoFence::~IRdoFence() {}

	using RdoFenceHandle = IRenderDeviceObjectHandle<IRdoFence>;
}
