#include <Application/Application.h>

#include <Core/Logging/Logging.h>
#include <Core/Modules/ModuleManager.h>

#include "Rendering/Rendering.h"

namespace Oyl
{
	class RenderingModule final : public IRenderingModule
	{
	public:
		virtual
		~RenderingModule() = default;

		void
		RegisterRenderContextFactory(IRenderContextFactory& a_factory) override
		{
			auto iter = std::ranges::find(m_rendererFactories, &a_factory);
			if (iter == m_rendererFactories.end())
				m_rendererFactories.Add(&a_factory);
		}

		void
		UnregisterRenderContextFactory(IRenderContextFactory& a_factory) override
		{
			auto iter = std::ranges::find(m_rendererFactories, &a_factory);
			if (iter != m_rendererFactories.end())
				m_rendererFactories.erase(iter);
		}

		UniquePtr<IRenderContext>
		CreateRenderContext(RenderingApi a_api) override
		{
			for (auto& rendererFactory : m_rendererFactories)
				if (rendererFactory->SupportsApi(a_api))
					return rendererFactory->CreateRenderContext();
			return nullptr;
		}

		void
		RegisterImGuiRendererFactory(IImGuiRendererFactory& a_factory) override
		{
			auto iter = std::ranges::find(m_imGuiRendererFactories, &a_factory);
			if (iter == m_imGuiRendererFactories.end())
				m_imGuiRendererFactories.Add(&a_factory);
		}

		void
		UnregisterImGuiRendererFactory(IImGuiRendererFactory& a_factory) override
		{
			auto iter = std::ranges::find(m_imGuiRendererFactories, &a_factory);
			if (iter != m_imGuiRendererFactories.end())
				m_imGuiRendererFactories.erase(iter);
		}

		UniquePtr<IImGuiRenderer>
		CreateImGuiRenderer(RenderingApi a_api) override
		{
			for (auto& rendererFactory : m_imGuiRendererFactories)
				if (rendererFactory->SupportsApi(a_api))
					return rendererFactory->CreateImGuiRenderer();
			return nullptr;
		}

	private:
		Array<IRenderContextFactory*> m_rendererFactories;
		Array<IImGuiRendererFactory*> m_imGuiRendererFactories;
	};

	OYL_MODULE_DECLARE(Rendering, RenderingModule)
}
