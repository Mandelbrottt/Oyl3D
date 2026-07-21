#include "RenderEngineInstance.h"

#include "Rendering/ShaderCompiler.h"

namespace Oyl::Rendering
{
	RenderEngineInstance::RenderEngineInstance(Internal::ResourceManager* a_resourceManager)
		: m_resourceManager(a_resourceManager) {}

	RenderEngineInstance::~RenderEngineInstance() {}

	void
	RenderEngineInstance::SetShaderCompiler(std::unique_ptr<ShaderCompiler>&& a_shaderCompiler)
	{
		m_shaderCompiler = std::move(a_shaderCompiler);
	}
}
