#include <Core/Modules/ModuleManager.h>

#include <Rendering/Rendering.h>

#include "VulkanModule/VulkanRenderContext.h"

namespace Oyl
{
	class VulkanModule final
		: public IRenderContextFactory,
		  public IModuleInterface
	{
	public:
		void
		OnStartModule() override
		{
			auto* renderingModule = ModuleManager::GetModulePointer<IRenderingModule>("Rendering");
			if (renderingModule)
				renderingModule->RegisterRenderContextFactory(*this);
		}

		void
		OnStopModule() override
		{
			auto* renderingModule = ModuleManager::GetModulePointer<IRenderingModule>("Rendering");
			if (renderingModule)
				renderingModule->UnregisterRenderContextFactory(*this);
		}

		bool
		SupportsApi(RenderingApi a_api) override
		{
			return a_api == RenderingApi::Vulkan;
		}

		UniquePtr<IRenderContext>
		CreateRenderContext() override
		{
			return MakeUniquePtr<VulkanRenderContext>();
		}

		bool SupportsHotReload() override
		{
			return false;
		}
	};

	OYL_MODULE_DECLARE(VulkanModule, VulkanModule)
}
