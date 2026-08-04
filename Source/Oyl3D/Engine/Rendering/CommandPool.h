#pragma once

#include <Core/PImpl.h>
#include <Core/UniqueHandle.h>

#include "CommandQueue.h"

namespace Oyl::Rendering
{
	class CommandPoolImpl : public IUniqueHandle
	{
	protected:
		CommandPoolImpl() = default;

	public:
		struct CreateParams
		{
			CommandQueueFlagBits commandQueueFlags;
		};

		NO_MOVE(CommandPoolImpl);
		NO_COPY(CommandPoolImpl);

		virtual
		~CommandPoolImpl() = default;
	};

	using CommandPool = PImpl<CommandPoolImpl>;
}
