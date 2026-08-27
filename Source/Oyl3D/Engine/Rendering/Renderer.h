#pragma once

#include "RenderGraph.h"

namespace Oyl::Rendering
{
	class Device;
	class RenderContext;
	class SwapChain;

	class OYL_RENDERING_API Renderer
	{
	public:
		explicit
		Renderer(Device& a_device, SwapChain& a_swapChain);

		NO_COPY(Renderer);
		NO_MOVE(Renderer);

		virtual
		~Renderer();

		void
		Destroy();

		void
		Render();

		RenderGraph&
		GetRenderGraph();

	private:
		void
		RecordCommandBuffer();

		void
		RecreateSwapChain();

	private:
		struct Impl;
		std::unique_ptr<Impl> m_impl;
	};
}
