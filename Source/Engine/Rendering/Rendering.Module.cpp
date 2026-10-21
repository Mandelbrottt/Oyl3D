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
		RegisterRendererFactory(IRendererFactory& a_factory) override
		{
			auto iter = std::ranges::find(m_rendererFactories, &a_factory);
			if (iter == m_rendererFactories.end())
				m_rendererFactories.Add(&a_factory);
		}

		void
		UnregisterRendererFactory(IRendererFactory& a_factory) override
		{
			auto iter = std::ranges::find(m_rendererFactories, &a_factory);
			if (iter != m_rendererFactories.end())
				m_rendererFactories.erase(iter);
		}

		UniquePtr<IRenderer>
		CreateRenderer(RenderingApi a_api) override
		{
			for (auto& rendererFactory : m_rendererFactories)
				if (rendererFactory->SupportsApi(a_api))
					return rendererFactory->CreateRenderer();
			return nullptr;
		}

	private:
		Array<IRendererFactory*> m_rendererFactories;
	};

	OYL_MODULE_DECLARE(Rendering, RenderingModule)
}
