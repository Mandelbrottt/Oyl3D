#pragma once
#pragma once

#include <Core/Interfaces/IDestroyable.h>

#include "IDeviceObject.h"
#include "RenderingEnums.h"

namespace Oyl
{
	struct CommandPoolCreateParams
	{
		CommandQueueFlagBits commandQueueFlags;
	};

	class ICommandPool : public IDestroyable
	{
	protected:
		ICommandPool() = default;
		DEFAULT_MOVE(ICommandPool);

	public:
		NO_COPY(ICommandPool);

		virtual
		~ICommandPool() = 0;
	};

	inline
	ICommandPool::~ICommandPool() {}

	using CommandPoolHandle = IObjectHandle<ICommandPool>;
}
