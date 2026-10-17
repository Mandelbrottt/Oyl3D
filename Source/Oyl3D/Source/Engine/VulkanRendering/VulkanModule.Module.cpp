#include <Application/Application.h>

#include <Core/Logging/Logging.h>
#include <Core/Modules/ModuleManager.h>

#include <Rendering/Rendering.h>

#include "VulkanRenderer.h"

namespace Oyl
{
	class VulkanModule final
		: public IRendererFactory,
		  public IModuleInterface
	{
	public:
		void
		OnStartModule() override
		{
			auto* renderingModule = ModuleManager::GetModulePointer<IRenderingModule>("Rendering");
			if (renderingModule)
				renderingModule->RegisterRendererFactory(*this);
		}

		void
		OnStopModule() override
		{
			auto* renderingModule = ModuleManager::GetModulePointer<IRenderingModule>("Rendering");
			if (renderingModule)
				renderingModule->UnregisterRendererFactory(*this);
		}

		bool
		SupportsApi(RenderingApi a_api) override
		{
			return a_api == RenderingApi::Vulkan;
		}

		IRenderer*
		CreateRenderer() override
		{
			return new VulkanRenderer;
		}

		bool SupportsHotReload() override
		{
			return false;
		}
	};

	OYL_MODULE_DECLARE(VulkanModule, VulkanModule)
}
