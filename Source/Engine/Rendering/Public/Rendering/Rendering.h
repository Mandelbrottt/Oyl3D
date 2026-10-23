#pragma once

#include <Core/Containers/UniquePtr.h>
#include <Core/Modules/ModuleInterface.h>

#include "RenderContext.h"

#include "ImGui/ImGuiRenderer.h"

#include "Rendering/RenderingEnums.h"

namespace Oyl
{
	class IRenderContextFactory
	{
	public:
		virtual
		~IRenderContextFactory() = 0;

		virtual
		bool
		SupportsApi(RenderingApi a_api) = 0;

		virtual
		UniquePtr<IRenderContext>
		CreateRenderContext() = 0;
	};

	inline
	IRenderContextFactory::~IRenderContextFactory() {}

	class IImGuiRendererFactory
	{
	public:
		virtual
		~IImGuiRendererFactory() = 0;

		virtual
		bool
		SupportsApi(RenderingApi a_api) = 0;

		virtual
		UniquePtr<IImGuiRenderer>
		CreateImGuiRenderer() = 0;
	};

	inline
	IImGuiRendererFactory::~IImGuiRendererFactory() {}

	class IRenderingModule : public IModuleInterface
	{
	public:
		virtual
		~IRenderingModule() = 0;

		virtual
		void
		RegisterRenderContextFactory(IRenderContextFactory& a_factory) = 0;

		virtual
		void
		UnregisterRenderContextFactory(IRenderContextFactory& a_factory) = 0;

		virtual
		UniquePtr<IRenderContext>
		CreateRenderContext(RenderingApi a_api) = 0;

		virtual
		void
		RegisterImGuiRendererFactory(IImGuiRendererFactory& a_factory) = 0;

		virtual
		void
		UnregisterImGuiRendererFactory(IImGuiRendererFactory& a_factory) = 0;

		virtual
		UniquePtr<IImGuiRenderer>
		CreateImGuiRenderer(RenderingApi a_api) = 0;

		bool
		SupportsHotReload() override
		{
			return false;
		}
	};

	inline
	IRenderingModule::~IRenderingModule() {}
}
