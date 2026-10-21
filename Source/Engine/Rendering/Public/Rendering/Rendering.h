#pragma once

#include <Core/Modules/ModuleInterface.h>

#include "Renderer.h"

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

		bool
		SupportsHotReload() override
		{
			return false;
		}
	};

	inline
	IRenderingModule::~IRenderingModule() {}
}
