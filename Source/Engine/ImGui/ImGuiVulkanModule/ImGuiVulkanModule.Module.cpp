#include <Core/Modules/ModuleManager.h>

#include <ImGuiModule/IImGuiImpl.h>
#include <ImGuiModule/ImGuiModule.h>

#include "ImGuiVulkanModule/ImGuiVulkanImpl.h"

namespace Oyl
{
	class ImGuiVulkanModule final
		: public IImGuiImplFactory,
		  public IModuleInterface
	{
	public:
		virtual
		~ImGuiVulkanModule() = default;

		void
		OnStartModule() override
		{
			auto renderingModule = ModuleManager::GetModulePointer<IImGuiModule>("ImGuiModule");
			if (renderingModule)
				renderingModule->RegisterImGuiImplFactory(*this);
		}

		void
		OnStopModule() override
		{
			auto renderingModule = ModuleManager::GetModulePointer<IImGuiModule>("ImGuiModule");
			if (renderingModule)
				renderingModule->UnregisterImGuiImplFactory(*this);
		}

		bool
		SupportsApi(RenderingApi a_api) override
		{
			return a_api == RenderingApi::Vulkan;
		}

		UniquePtr<IImGuiImpl>
		CreateImGuiImpl() override
		{
			return MakeUniquePtr<ImGuiVulkanImpl>();
		}
	};

	OYL_MODULE_DECLARE(ImGuiVulkanModule, ImGuiVulkanModule)
}
