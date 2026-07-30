#pragma once

#include <Core/PImpl.h>
#include <Core/UniqueHandle.h>

#include "CommandQueue.h"
#include "Device.h"

namespace Oyl
{
	class IWindow;
}

namespace Oyl::Rendering
{
	class DeviceImpl : public IUniqueHandle
	{
	protected:
		DeviceImpl() = default;

	public:
		NO_MOVE(DeviceImpl);
		NO_COPY(DeviceImpl);

		virtual
		~DeviceImpl() = default;

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

		virtual
		void
		WaitUntilIdle() const = 0;
	};

	using Device = PImpl<DeviceImpl>;
}
