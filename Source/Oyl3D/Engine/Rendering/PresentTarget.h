#pragma once

#include <Core/UniquePtr.h>

#include "DeviceObject.h"

namespace Oyl
{
	class IWindow;
}

namespace Oyl::Rendering
{
	struct PresentTargetId : OpaqueHandle<PresentTargetId> {};

	class PresentTarget : public IDeviceObject<PresentTargetId>
	{
	protected:
		PresentTarget() = default;

		DEFAULT_MOVE(PresentTarget);

	public:
		struct CreateParams
		{
			const IWindow& window;
		};

		NO_COPY(PresentTarget);

		virtual
		~PresentTarget() = default;
	};

	using PresentTargetHandle = UniquePtrImplicitConvertible<PresentTarget>;
}
