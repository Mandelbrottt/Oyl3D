#pragma once

#include <Core/Types/Primitives.h>

#include "IDeviceObject.h"

namespace Oyl
{
	struct FenceId : IDeviceObjectId<FenceId> {};

	class IFence : public IDeviceObject<FenceId>
	{
	protected:
		IFence() = default;

		DEFAULT_MOVE(IFence);

	public:
		NO_COPY(IFence);

		virtual
		~IFence() = 0;

		void
		virtual
		Reset() = 0;

		bool
		virtual
		Wait(uint64 a_timeout = INT64_MAX) = 0;
	};

	inline
	IFence::~IFence() {}
}
