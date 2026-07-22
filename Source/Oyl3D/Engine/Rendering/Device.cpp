#include "Device.h"

namespace Oyl::Rendering
{
	Device::Device(const Window* a_window)
		: m_window(a_window) {}

	Device::~Device()
	{
		m_window = nullptr;
	}
}
