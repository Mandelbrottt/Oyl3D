#pragma once

#include "DeviceObject.h"

#include "Rendering/Window.h"

namespace Oyl::Rendering
{
	class OYL_RENDERING_API Device : public Internal::IDeviceObject
	{
	protected:
		explicit
		Device(const Window* a_window)
			: m_window(a_window) {}

		Device(Device&& a_other) noexcept
		{
			*this = std::move(a_other);
		}

		Device&
		operator =(Device&& a_other) noexcept
		{
			if (this != &a_other)
			{
				IDeviceObject::operator=(std::move(a_other));
				std::swap(m_window, a_other.m_window);
			}
			return *this;
		}

	public:
		virtual
		~Device()
		{
			Device::Destroy();
		};

		void
		Destroy() override
		{
			m_window = nullptr;
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
