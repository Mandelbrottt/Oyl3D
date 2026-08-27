#include "RenderEngine.h"

#include <Core/Resources/ResourceManager.h>

#include "Vulkan/VulkanRenderEngine.h"

namespace Oyl
{
	Internal::RenderEngineInstance* RenderEngine::s_instance = nullptr;

	namespace Internal
	{
		RenderEngineInstance::RenderEngineInstance()
			: m_resourceManager(std::make_unique<ResourceManager>()) {}

		RenderEngineInstance::~RenderEngineInstance() {}

		ResourceManager&
		RenderEngineInstance::GetResourceManager()
		{
			return *m_resourceManager;
		}

		Vector2u
		RenderEngineInstance::GetCurrentViewPortSize() const
		{
			return GetCurrentSwapChain().GetSize();
		}
	}

	UniquePtr<Internal::RenderEngineInstance>
	RenderEngine::CreateInstance(Rendering::GraphicsApi a_api, const CreateParams& a_params)
	{
		switch (a_api)
		{
			case Rendering::GraphicsApi::Vulkan:
				return UniquePtr<Internal::VulkanRenderEngineInstance>::Create(
					Internal::VulkanRenderEngineInstance::CreateParams {
						.window = a_params.window
					});
			case Rendering::GraphicsApi::None:
				throw "Graphics API must not be None!";
			default:
				throw "Graphics API not implemented!";
		}
	}
}
