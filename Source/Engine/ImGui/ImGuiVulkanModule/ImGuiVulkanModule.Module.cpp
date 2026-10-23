#include <Core/Modules/ModuleManager.h>

#include <Rendering/Rendering.h>

#include "ImGuiVulkanModule/ImGuiVulkanRenderer.h"

namespace Oyl
{
	class ImGuiVulkanModule final
		: public IImGuiRendererFactory,
		  public IModuleInterface
	{
	public:
		virtual
		~ImGuiVulkanModule() = default;

		void
		OnStartModule() override
		{
			auto renderingModule = ModuleManager::GetModulePointer<IRenderingModule>("Rendering");
			if (renderingModule)
				renderingModule->RegisterImGuiRendererFactory(*this);
		}

		void
		OnStopModule() override
		{
			auto renderingModule = ModuleManager::GetModulePointer<IRenderingModule>("Rendering");
			if (renderingModule)
				renderingModule->UnregisterImGuiRendererFactory(*this);
		}

		bool
		SupportsApi(RenderingApi a_api) override
		{
			return a_api == RenderingApi::Vulkan;
		}

		UniquePtr<IImGuiRenderer>
		CreateImGuiRenderer() override
		{
			return MakeUniquePtr<ImGuiVulkanRenderer>();
		}
	};

	OYL_MODULE_DECLARE(ImGuiVulkanModule, ImGuiVulkanModule)
}
