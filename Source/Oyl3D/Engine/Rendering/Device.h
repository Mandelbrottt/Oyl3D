#pragma once

#include "Window.h"

#include "Rendering/Common.h"

namespace Oyl::Rendering
{
	class OYL_RENDERING_API Device
	{
	protected:
		explicit
		Device(const Window* a_window);

		Device(Device&& a_other) = default;
		Device&
		operator =(Device&& a_other) = default;

	public:
		Device(const Device& a_other) = delete;
		Device&
		operator =(const Device& a_other) = delete;

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

		const Window*
		GetWindow() const
		{
			return m_window;
		}

	private:
		const Window* m_window;
	};
}
