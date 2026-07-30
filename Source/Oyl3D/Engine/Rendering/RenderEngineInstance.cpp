#include "RenderEngineInstance.h"

#include <Core/Resources/ResourceManager.h>

#include "RenderContext.h"

#include "Rendering/Device.h"
#include "Rendering/ShaderCompiler.h"

namespace Oyl::Rendering::Internal
{
	RenderEngineInstance::RenderEngineInstance() {}

	RenderEngineInstance::RenderEngineInstance(CreateParams a_params)
		: m_resourceManager(std::make_unique<Oyl::Internal::ResourceManager>()),
		  m_shaderCompiler(std::move(a_params.shaderCompiler)),
		  m_renderContext(std::move(a_params.renderContext)) {}

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

	const DeviceImpl*
	RenderEngineInstance::GetCurrentDevice() const
	{
		const DeviceImpl* result = nullptr;
		if (m_renderContext)
			result = m_renderContext->GetDevice();
		return result;
	}

	IRenderContext*
	RenderEngineInstance::GetRenderContext() const
	{
		return m_renderContext.get();
	}
}
