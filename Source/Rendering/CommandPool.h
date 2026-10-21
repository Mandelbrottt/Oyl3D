#pragma once

#include <Core/PImpl.h>
#include <Core/UniqueHandle.h>

#include "CommandQueue.h"

namespace Oyl::Rendering
{
	class CommandPool : public IUniqueHandle
	{
	protected:
		CommandPool() = default;

	public:
		struct CreateParams
		{
			CommandQueueFlagBits commandQueueFlags;
		};

		NO_MOVE(CommandPool);
		NO_COPY(CommandPool);

		virtual
		~CommandPool() = default;
	};

	using CommandPoolHandle = PImpl<CommandPool>;
}
