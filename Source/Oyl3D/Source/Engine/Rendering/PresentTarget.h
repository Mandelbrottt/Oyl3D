#pragma once

#include <Application/ApplicationWindow.h>

#include "IDeviceObject.h"

namespace Oyl
{
	struct PresentTargetId : IDeviceObjectId<PresentTargetId> {};

	struct PresentTargetCreateParams
	{
		const IApplicationWindow& window;
	};

	class IPresentTarget
		: public IDeviceObject<PresentTargetId>
	{
	protected:
		IPresentTarget() = default;
		DEFAULT_MOVE(IPresentTarget);

	public:
		NO_COPY(IPresentTarget);

		virtual
		~IPresentTarget() = 0;
	};

	inline
	IPresentTarget::~IPresentTarget() {}

	using PresentTargetHandle = IObjectHandle<IPresentTarget>;
}
