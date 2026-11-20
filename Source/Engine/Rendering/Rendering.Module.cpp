#include <Application/Application.h>

#include <Core/Containers/ArraySet.h>
#include <Core/Containers/Optional.h>
#include <Core/Modules/ModuleManager.h>

#include "Rendering/Rendering.h"

#include <optional>

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
			m_renderContextFactories.Add(&a_factory);
		}

		void
		UnregisterRenderContextFactory(IRenderContextFactory& a_factory) override
		{
			m_renderContextFactories.Erase(&a_factory);
		}

		UniquePtr<IRenderContext>
		CreateRenderContext(RenderingApi a_api) override
		{
			for (auto& rendererFactory : m_renderContextFactories)
				if (rendererFactory->SupportsApi(a_api))
					return rendererFactory->CreateRenderContext();
			return nullptr;
		}

		void
		RegisterImGuiRendererFactory(IImGuiRendererFactory& a_factory) override
		{
			if (m_imGuiRendererFactory == nullptr)
				m_imGuiRendererFactory = &a_factory;
		}

		void
		UnregisterImGuiRendererFactory(IImGuiRendererFactory& a_factory) override
		{
			if (m_imGuiRendererFactory == &a_factory)
				m_imGuiRendererFactory = nullptr;
		}

		UniquePtr<IImGuiRenderer>
		CreateImGuiRenderer() override
		{
			if (m_imGuiRendererFactory)
				return m_imGuiRendererFactory->CreateImGuiRenderer();
			return nullptr;
		}

		void
		RegisterModelLoaderFactory(IModelLoaderFactory& a_factory) override
		{
			m_modelFactories.Add(&a_factory);
		}

		void
		UnregisterModelLoaderFactory(IModelLoaderFactory& a_factory) override
		{
			m_modelFactories.Erase(&a_factory);
		}

		ModelDesc
		CreateModel(StringView a_filename) override
		{
			auto extension = StringView(a_filename.substr(a_filename.rfind(".")));
			for (auto* factory : m_modelFactories)
			{
				if (!factory->SupportsExtension(extension))
					continue;

				ModelDesc modelDesc;
				auto loaded = factory->LoadModelFromFile(a_filename, &modelDesc);
				if (loaded)
					return modelDesc;
			}

			return {};
		}

	private:
		ArraySet<IRenderContextFactory*> m_renderContextFactories;
		IImGuiRendererFactory* m_imGuiRendererFactory = nullptr;

		ArraySet<IModelLoaderFactory*> m_modelFactories;
	};

	OYL_MODULE_DECLARE(RenderingModule)
}
