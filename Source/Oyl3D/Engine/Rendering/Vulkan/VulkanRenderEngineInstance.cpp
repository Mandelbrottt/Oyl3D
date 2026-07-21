#include "VulkanRenderEngineInstance.h"

#include "VulkanRenderContext.h"
#include "VulkanShader.h"
#include "VulkanShaderCompiler.h"
#include "VulkanVertexBuffer.h"

#include "Core/Resources/ResourceManager.h"

namespace Oyl::Rendering::Vulkan
{
	RenderEngineInstance::RenderEngineInstance(
		Internal::ResourceManager* a_resourceManager,
		const RenderContext* a_renderContext
	)
		: Rendering::RenderEngineInstance(a_resourceManager, a_renderContext)
	{
		RenderEngineInstance::SetShaderCompiler(std::make_unique<ShaderCompiler>());
	}

	Rendering::Shader
	RenderEngineInstance::CreateShader(const ShaderOptions& a_options)
	{
		return GetResourceManager()->Load<Shader>(a_options);
	}

	Rendering::VertexBuffer
	RenderEngineInstance::CreateVertexBuffer(const VertexBufferOptions& a_options)
	{
		return GetResourceManager()->Load<VertexBuffer>(a_options);
	}
}
