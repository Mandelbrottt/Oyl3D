#include <Core/Containers/ArraySet.h>
#include <Core/Modules/ModuleManager.h>

#include <Rendering/Rendering.h>

#include "ImGuiRenderer.h"

#include "ImGuiModule/IImGuiImpl.h"
#include "ImGuiModule/ImGuiModule.h"

namespace Oyl
{
	class ImGuiModule final : public IImGuiModule
	{
	public:
		virtual ~ImGuiModule() = default;

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

		UniquePtr<IImGuiRenderer>
		CreateImGuiRenderer() override
		{
			return MakeUniquePtr<ImGuiRenderer>();
		}

		void
		RegisterImGuiImplFactory(IImGuiImplFactory& a_factory) override
		{
			if (!m_imGuiImplFactories.Contains(&a_factory))
				m_imGuiImplFactories.Add(&a_factory);
			else
				LogError("ImGuiImplFactory registered twice!");
		}

		void
		UnregisterImGuiImplFactory(IImGuiImplFactory& a_factory) override
		{
			if (m_imGuiImplFactories.Contains(&a_factory))
				m_imGuiImplFactories.Erase(&a_factory);
			else
				LogError("ImGuiImplFactory unregistered before being registered!");
		}

		UniquePtr<IImGuiImpl>
		CreateImGuiImpl(RenderingApi a_api) override
		{
			for (auto& factory : m_imGuiImplFactories)
				if (factory->SupportsApi(a_api))
					return factory->CreateImGuiImpl();
			return nullptr;
		}

	private:
		ArraySet<IImGuiImplFactory*> m_imGuiImplFactories;
	};

	OYL_MODULE_DECLARE(ImGuiModule, ImGuiModule)
}
