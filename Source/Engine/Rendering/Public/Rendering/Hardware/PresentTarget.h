#pragma once

#include "RenderDeviceObject.h"

namespace Oyl
{
	class IApplicationWindow;

	struct HardwarePresentTargetId : IRenderDeviceObjectId<HardwarePresentTargetId> {};

	struct HardwarePresentTargetCreateParams
	{
		const IApplicationWindow& window;
	};

	class IHardwarePresentTarget
		: public IRenderDeviceObject<HardwarePresentTargetId>
	{
	protected:
		IHardwarePresentTarget() = default;
		DEFAULT_MOVE(IHardwarePresentTarget);

	public:
		NO_COPY(IHardwarePresentTarget);

		virtual
		~IHardwarePresentTarget() = 0;
	};

	inline
	IHardwarePresentTarget::~IHardwarePresentTarget() {}

	using HardwarePresentTargetHandle = IRenderDeviceObjectHandle<IHardwarePresentTarget>;
}
