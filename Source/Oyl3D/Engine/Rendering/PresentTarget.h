#pragma once

#include <Core/PImpl.h>

#include "DeviceObject.h"

namespace Oyl::Rendering
{
	struct PresentTargetHandle : OpaqueHandle<PresentTargetHandle> {};

	class PresentTarget : public IDeviceObject<PresentTargetHandle>
	{
	protected:
		PresentTarget() = default;

		DEFAULT_MOVE(PresentTarget);

	public:
		NO_COPY(PresentTarget);

		virtual
		~PresentTarget() = default;
	};

	using PresentTargetPtr = PImpl<PresentTarget>;
	//using PresentTargetPtr = DeviceObjectPtr<PresentTarget>;
}
