#pragma once

#include <Core/UniqueHandle.h>

namespace Oyl::Rendering
{
	class ICommandPool : public IUniqueHandle
	{
	protected:
		ICommandPool() = default;

		DEFAULT_MOVE(ICommandPool);

	public:
		NO_COPY(ICommandPool);

		virtual
		~ICommandPool() = default;
	};
}
