#include "RenderEngine.h"

#include <Core/Logging/Logging.h>

namespace Oyl
{
	struct RenderEngine::Impl
	{
		DeviceHandle device = nullptr;
		SharedPtr<IShaderCompiler> shaderCompiler;
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

	IDevice&
	RenderEngine::GetDevice() const
	{
		OYL_ASSERT(m_impl->device);
		return *m_impl->device;
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

	IShaderCompiler&
	RenderEngine::GetShaderCompiler() const
	{
		return *m_impl->shaderCompiler;
	}

	void
	RenderEngine::RegisterShaderCompiler(const SharedPtr<IShaderCompiler>& a_shaderCompiler)
	{
		if (!a_shaderCompiler)
			return;

		if (!m_impl->shaderCompiler)
		{
			LogTrace("RenderEngine::RegisterShadeCompiler({})", (void*) a_shaderCompiler.Get());
			m_impl->shaderCompiler = a_shaderCompiler;
		} else
			LogError("Trying to register ShaderCompiler [{}] when ShaderCompiler [{}] has already been registered!",
			         (void*) a_shaderCompiler.Get(),
			         (void*) m_impl->shaderCompiler.Get());
	}

	void
	RenderEngine::UnregisterShaderCompiler(const SharedPtr<IShaderCompiler>& a_shaderCompiler)
	{
		if (!a_shaderCompiler)
			return;

		if (!m_impl->shaderCompiler)
			return;

		if (m_impl->shaderCompiler == a_shaderCompiler)
		{
			LogTrace("RenderEngine::UnregisterShaderCompiler({})", (void*) a_shaderCompiler.Get());
			m_impl->shaderCompiler = nullptr;
		} else
			LogError("Trying to unregister ShaderCompiler [{}] when ShaderCompiler [{}] has already been registered!",
			         (void*) a_shaderCompiler.Get(),
			         (void*) m_impl->shaderCompiler.Get());
	}
}
