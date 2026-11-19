#include <Application/ApplicationModule.h>

#include <Core/Modules/ModuleManager.h>

#include <EditorClient/EditorClientApplication.h>

#include "RecompilableRenderPass.h"

namespace Oyl
{
	class TestRecompilableModule final : public IModuleInterface
	{
	public:
		virtual ~TestRecompilableModule() = default;

		void OnStartModule() override
		{
			m_renderPass = MakeUniquePtr<RecompilableRenderPass>(Vector2u(1280, 720));

			auto app = ModuleManager::GetModulePointer<IApplicationModule>("Application")->CurrentApplication();
			auto ecApp = dynamic_cast<EditorClientApplication*>(app);

			m_renderPass->AddDependency(nameof(TestRenderPass));
			ecApp->GetRenderer().GetRenderGraph().AddRenderPass(m_renderPass.Get());
		}

		void OnStopModule() override
		{
			auto app = ModuleManager::GetModulePointer<IApplicationModule>("Application")->CurrentApplication();
			auto ecApp = dynamic_cast<EditorClientApplication*>(app);

			ecApp->GetRenderer().GetRenderGraph().RemoveRenderPass(m_renderPass.Get());
		}

		bool ShouldAutoload() override
		{
			return false;
		}

	private:
		UniquePtr<RecompilableRenderPass> m_renderPass;
	};

	OYL_MODULE_DECLARE(TestRecompilableModule)
}
