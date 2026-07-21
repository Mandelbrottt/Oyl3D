#pragma once

#include "Rendering/Common.h"

namespace Oyl::Rendering
{
	class OYL_RENDERING_API Device
	{
	protected:
		Device();

	public:
		virtual
		~Device();

		Device(const Device& a_other) = delete;
		virtual
		Device&
		operator =(const Device& a_other) = delete;

		Device(Device&& a_other) = default;
		virtual
		Device&
		operator =(Device&& a_other) = default;

		virtual
		bool
		Destroy()
		{
			return true;
		}
	};
}
