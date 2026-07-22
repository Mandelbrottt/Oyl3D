#include "RenderEngineInstance.h"

#include <Core/Resources/ResourceManager.h>

#include "RenderContext.h"

#include "Rendering/Device.h"
#include "Rendering/ShaderCompiler.h"

namespace Oyl::Rendering::Internal
{
	RenderEngineInstance::RenderEngineInstance() {}

	RenderEngineInstance::RenderEngineInstance(Params a_params)
		: m_resourceManager(std::move(a_params.resourceManager)),
		  m_shaderCompiler(std::move(a_params.shaderCompiler)),
		  m_device(std::move(a_params.device)),
		  m_renderContext(std::move(a_params.renderContext))
	{
		if (!m_resourceManager)
			m_resourceManager = std::make_unique<Oyl::Internal::ResourceManager>();
	}

	RenderEngineInstance::~RenderEngineInstance() {}

	Oyl::Internal::ResourceManager*
	RenderEngineInstance::GetResourceManager() const
	{
		return m_resourceManager.get();
	}

	const ShaderCompiler*
	RenderEngineInstance::GetShaderCompiler() const
	{
		return m_shaderCompiler.get();
	}

	const Device*
	RenderEngineInstance::GetCurrentDevice() const
	{
		return m_device.get();
	}

	RenderContext*
	RenderEngineInstance::GetRenderContext() const
	{
		return m_renderContext.get();
	}
}
