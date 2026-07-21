#include "RenderEngineInstance.h"

#include "RenderContext.h"

#include "Rendering/ShaderCompiler.h"

namespace Oyl::Rendering
{
	RenderEngineInstance::RenderEngineInstance(
		Internal::ResourceManager* a_resourceManager,
		const RenderContext* a_renderContext
	)
		: m_resourceManager(a_resourceManager),
		  m_renderContext(a_renderContext)
	{
		m_device = m_renderContext->GetDevice();
	}

	RenderEngineInstance::~RenderEngineInstance()
	{
		m_resourceManager = nullptr;
		m_device = nullptr;
	}

	const ShaderCompiler*
	RenderEngineInstance::GetShaderCompiler() const
	{
		return m_shaderCompiler.get();
	}

	const Device*
	RenderEngineInstance::GetCurrentDevice() const
	{
		return m_device;
	}

	void
	RenderEngineInstance::SetCurrentDevice(const Device* a_device)
	{
		m_device = a_device;
	}

	void
	RenderEngineInstance::SetShaderCompiler(std::unique_ptr<ShaderCompiler>&& a_shaderCompiler)
	{
		m_shaderCompiler = std::move(a_shaderCompiler);
	}
}
