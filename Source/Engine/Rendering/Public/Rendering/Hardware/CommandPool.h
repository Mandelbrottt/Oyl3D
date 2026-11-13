#pragma once
#pragma once

#include <Core/Interfaces/IDestroyable.h>

#include "RenderDeviceObject.h"

#include "Rendering/RenderingEnums.h"

namespace Oyl
{
	struct RdoCommandPoolCreateParams
	{
		CommandQueueFlagBits commandQueueFlags;
	};

	class IRdoCommandPool : public IDestroyable
	{
	protected:
		IRdoCommandPool() = default;
		DEFAULT_MOVE(IRdoCommandPool);

	public:
		NO_COPY(IRdoCommandPool);

		virtual
		~IRdoCommandPool() = 0;
	};

	inline
	IRdoCommandPool::~IRdoCommandPool() {}

	using RdoCommandPoolHandle = IRenderDeviceObjectHandle<IRdoCommandPool>;
}
