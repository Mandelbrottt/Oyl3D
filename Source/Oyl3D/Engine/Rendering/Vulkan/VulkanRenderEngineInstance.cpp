#include "VulkanRenderEngineInstance.h"

#include "Core/Resources/ResourceManager.h"

#include "VulkanVertexBuffer.h"
#include "VulkanShader.h"
#include "VulkanShaderCompiler.h"

namespace Oyl::Rendering::Vulkan
{
	RenderEngineInstance::RenderEngineInstance(Internal::ResourceManager* a_resourceManager)
		: Rendering::RenderEngineInstance(a_resourceManager)
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
