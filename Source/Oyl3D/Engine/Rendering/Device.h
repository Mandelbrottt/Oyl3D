#pragma once

#include "Rendering/Common.h"

namespace Oyl::Rendering
{
	class OYL_RENDERING_API Device
	{
	protected:
		Device();

	public:
		Device(const Device& a_other) = delete;
		virtual
		Device&
		operator =(const Device& a_other) = delete;

		Device(Device&& a_other) = default;
		virtual
		Device&
		operator =(Device&& a_other) = default;

		virtual
		~Device();

		virtual
		bool
		Destroy()
		{
			return true;
		}

		virtual
		bool
		IsValid() const = 0;

		explicit
		operator bool() const
		{
			return IsValid();
		}
	};
}
