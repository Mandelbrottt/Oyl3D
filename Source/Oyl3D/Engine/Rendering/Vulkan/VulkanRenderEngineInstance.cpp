#include "VulkanRenderEngineInstance.h"

#include "VulkanRenderContext.h"
#include "VulkanRenderContext.h"
#include "VulkanShader.h"
#include "VulkanShaderCompiler.h"
#include "VulkanVertexBuffer.h"

#include "Core/Resources/ResourceManager.h"

namespace Oyl::Rendering::Vulkan
{
	RenderEngineInstance::RenderEngineInstance(CreateParams a_params)
		: Internal::RenderEngineInstance(
			{
				.shaderCompiler = std::make_unique<ShaderCompiler>(),
				.renderContext = std::make_unique<RenderContext>(RenderContext::CreateParams {
					.window = a_params.window
				})
			}
		) {}

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
