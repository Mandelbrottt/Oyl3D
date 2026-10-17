#pragma once

#include <Core/Modules/ModuleInterface.h>

#include "Renderer.h"

namespace Oyl
{
	enum class RenderingApi
	{
		None = 0,
		OpenGL,
		WebGL,
		DirectX11,
		DirectX12,
		Vulkan,
		Metal,
	};

	class IRendererFactory
	{
	public:
		virtual
		~IRendererFactory() = 0;

		virtual
		bool
		SupportsApi(RenderingApi a_api) = 0;

		virtual
		IRenderer*
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
		IRenderer*
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
