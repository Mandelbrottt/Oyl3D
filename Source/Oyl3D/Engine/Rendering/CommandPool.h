#pragma once

#include "DeviceObject.h"

namespace Oyl::Rendering
{
	class OYL_RENDERING_API CommandPool : public Internal::IDeviceObject
	{
	protected:
		CommandPool() = default;

		CommandPool(CommandPool&& a_other) noexcept = default;
		CommandPool&
		operator =(CommandPool&& a_other) noexcept = default;

	public:
		virtual
		~CommandPool() = default;
	};
}
