#pragma once

#include <Core/Containers/UniquePtr.h>
#include <Core/Modules/ModuleInterface.h>

#include "Renderer.h"

#include "ImGui/ImGuiRenderer.h"

#include "Rendering/RenderingEnums.h"

namespace Oyl
{
	class IRendererFactory
	{
	public:
		virtual
		~IRendererFactory() = 0;

		virtual
		bool
		SupportsApi(RenderingApi a_api) = 0;

		virtual
		UniquePtr<IRenderer>
		CreateRenderer() = 0;
	};

	inline
	IRendererFactory::~IRendererFactory() {}

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
		RegisterRendererFactory(IRendererFactory& a_factory) = 0;

		virtual
		void
		UnregisterRendererFactory(IRendererFactory& a_factory) = 0;

		virtual
		UniquePtr<IRenderer>
		CreateRenderer(RenderingApi a_api) = 0;

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
