#include "RenderEngine.h"

#include <Core/Logging/Logging.h>

namespace Oyl
{
	struct RenderEngine::Impl
	{
		DeviceHandle device = nullptr;
	};

	RenderEngine::RenderEngine()
		: m_impl(new Impl) {}

	RenderEngine::~RenderEngine()
	{
		delete m_impl;
	}

	RenderEngine&
	RenderEngine::Get()
	{
		static RenderEngine instance;
		return instance;
	}

	DeviceHandle
	RenderEngine::CurrentDevice()
	{
		return m_impl->device;
	}

	void
	RenderEngine::RegisterDevice(const DeviceHandle& a_device)
	{
		if (!a_device)
			return;

		if (!m_impl->device)
		{
			LogTrace("RenderEngine::RegisterDevice({})", (void*) a_device.Get());
			m_impl->device = a_device;
		} else
			LogError("Trying to register Device [{}] when Device [{}] has already been registered!",
			         (void*) a_device.Get(),
			         (void*) m_impl->device.Get());
	}

	void
	RenderEngine::UnregisterDevice(const DeviceHandle& a_device)
	{
		if (!a_device)
			return;

		if (!m_impl->device)
			return;

		if (m_impl->device == a_device)
		{
			LogTrace("RenderEngine::UnregisterDevice({})", (void*) a_device.Get());
			m_impl->device = nullptr;
		} else
			LogError("Trying to unregister Device [{}] when Device [{}] has already been registered!",
			         (void*) a_device.Get(),
			         (void*) m_impl->device.Get());
	}
}
