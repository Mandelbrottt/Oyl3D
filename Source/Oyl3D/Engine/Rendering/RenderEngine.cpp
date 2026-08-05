#include "RenderEngine.h"

#include <Core/Resources/ResourceManager.h>

#include "Rendering/RenderContext.h"

#include "Vulkan/VulkanRenderEngine.h"

namespace Oyl
{
	Internal::RenderEngineInstance* RenderEngine::s_instance = nullptr;

	namespace Internal
	{
		RenderEngineInstance::RenderEngineInstance() {}

		RenderEngineInstance::RenderEngineInstance(CreateParams a_params)
			: m_resourceManager(std::make_unique<ResourceManager>()),
			  m_shaderCompiler(std::move(a_params.shaderCompiler)),
			  m_renderContext(std::move(a_params.renderContext)) {}

		RenderEngineInstance::~RenderEngineInstance() {}

		ResourceManager*
		RenderEngineInstance::GetResourceManager() const
		{
			return m_resourceManager.get();
		}

		const Rendering::ShaderCompiler*
		RenderEngineInstance::GetShaderCompiler() const
		{
			return m_shaderCompiler.get();
		}

		const Rendering::Device*
		RenderEngineInstance::GetCurrentDevice() const
		{
			const Rendering::Device* result = nullptr;
			if (m_renderContext)
				result = m_renderContext->GetDevice();
			return result;
		}

		Vector2u
		RenderEngineInstance::GetCurrentViewPortSize() const
		{
			return m_renderContext->GetSwapChain()->GetSize();
		}

		Rendering::RenderContext*
		RenderEngineInstance::GetRenderContext()
		{
			return m_renderContext.get();
		}

		const Rendering::RenderContext*
		RenderEngineInstance::GetRenderContext() const
		{
			return m_renderContext.get();
		}
	}

	UniquePtr<Internal::RenderEngineInstance>
	RenderEngine::CreateInstance(Rendering::GraphicsApi a_api)
	{
		switch (a_api)
		{
			case Rendering::GraphicsApi::Vulkan:
				return UniquePtr<Internal::VulkanRenderEngineInstance>::Create(
					Internal::VulkanRenderEngineInstance::CreateParams {
						.window = nullptr
					});
			case Rendering::GraphicsApi::None:
				throw "Graphics API must not be None!";
			default:
				throw "Graphics API not implemented!";
		}
	}
}
