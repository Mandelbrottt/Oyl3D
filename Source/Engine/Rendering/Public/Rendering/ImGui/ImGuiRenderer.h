#pragma once

#include <Application/ApplicationWindow.h>

namespace Oyl
{
	class Renderer;
	class ICommandBuffer;

	class IImGuiRenderer
	{
	public:
		virtual
		~IImGuiRenderer() = 0;

		virtual
		void
		Init(Renderer& a_renderer, IApplicationWindow& a_mainWindow) = 0;

		virtual
		void
		Update(Renderer& a_renderer) = 0;

		virtual
		void
		Render(Renderer& a_renderer, ICommandBuffer& a_commandBuffer) = 0;

		virtual
		void
		Shutdown(Renderer& a_renderer) = 0;

		virtual
		void
		SetOnUpdateCallback(Delegate<void()> a_callback) = 0;
	};

	inline
	IImGuiRenderer::~IImGuiRenderer() {}
}
