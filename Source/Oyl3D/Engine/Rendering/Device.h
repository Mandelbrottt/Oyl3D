#pragma once

#include <Core/UniqueHandle.h>

#include "CommandQueue.h"

namespace Oyl
{
	class IWindow;
}

namespace Oyl::Rendering
{
	class IDevice : public IUniqueHandle
	{
	protected:
		IDevice() = default;

		DEFAULT_MOVE(IDevice);

	public:
		NO_COPY(IDevice);

		virtual
		~IDevice() = default;

		void
		Destroy() override = 0;

		bool
		IsValid() const override = 0;

		virtual
		const IWindow*
		GetWindow() const = 0;

		virtual
		const ICommandQueue*
		GetCommandQueue(CommandQueueFlagBits a_flag) const = 0;
	};
}
