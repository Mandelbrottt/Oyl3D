#pragma once
#pragma once

#include <Core/Interfaces/IDestroyable.h>

#include "RenderingEnums.h"

namespace Oyl
{
	struct CommandPoolCreateParams
	{
		CommandQueueFlagBits commandQueueFlags;
	};

	class CommandPool : public IDestroyable
	{
	protected:
		CommandPool() = default;
		DEFAULT_MOVE(CommandPool);

	public:
		NO_COPY(CommandPool);

		virtual
		~CommandPool() = 0;
	};

	inline
	CommandPool::~CommandPool() {}
}
