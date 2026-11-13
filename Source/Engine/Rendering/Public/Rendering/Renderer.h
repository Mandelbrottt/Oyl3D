#pragma once

#include <Application/ApplicationWindow.h>

#include <Core/Containers/ArrayMap.h>
#include <Core/Containers/UniquePtr.h>

#include "RenderGraph.h"

#include "ImGui/IImGuiRenderer.h"

#include "Rendering/RenderContext.h"
#include "Rendering/Hardware/PresentTarget.h"
#include "Rendering/Hardware/RenderDevice.h"

namespace Oyl
{
	struct RendererWindowData
	{
		IApplicationWindow* window;

		HardwarePresentTargetHandle presentTarget = nullptr;
		RdoSwapChainHandle swapChain = nullptr;

		RdoCommandPoolHandle commandPool = nullptr;
		Array<RdoCommandBufferHandle> commandBuffers;

		Array<RdoSemaphoreHandle> presentCompleteSemaphores;
		Array<RdoSemaphoreHandle> renderFinishedSemaphores;
		Array<RdoFenceHandle> inFlightFences;

		uint32 frameIndex = 0;
	};

	class RENDERING_EXPORT Renderer final
	{
	public:
		Renderer() = default;

		explicit
		Renderer(UniquePtr<IRenderContext>&& a_renderer);

		NO_COPY(Renderer);
		DEFAULT_MOVE(Renderer);

		~Renderer();

		void
		Init(IApplicationWindow* a_mainWindow);

		void
		Update();

		void
		Shutdown();

		bool
		IsValid() const noexcept { return m_renderContext; }

		IRenderContext&
		GetRenderContext() const { return *m_renderContext; }

		void
		RegisterApplicationWindow(IApplicationWindow& a_window);

		void
		UnregisterApplicationWindow(IApplicationWindow& a_window);

		const RendererWindowData*
		GetWindowData(IApplicationWindow& a_window) const;

		RenderGraph&
		GetRenderGraph() { return m_renderGraph; }

		const RenderGraph&
		GetRenderGraph() const { return const_cast<Renderer*>(this)->GetRenderGraph(); }

		const MulticastDelegate<void()>&
		OnDebugGuiCallback() const
		{
			return m_imGuiRenderer->OnDebugGuiCallback();
		}

	private:
		void
		CreateSwapChain(RendererWindowData& a_windowData);

		void
		CreateCommandBuffers(RendererWindowData& a_windowData);

		void
		CreateSyncObjects(RendererWindowData& a_windowData);

		void
		RenderToWindow(RendererWindowData& a_windowData);

		void
		RecordWindowCommandBuffer(RendererWindowData& a_windowData);

		void
		RecreateSwapChain(RendererWindowData& a_windowData);

		void
		ResizeRenderGraphViewPort(const IRdoSwapChain& a_swapChain);

	private:
		UniquePtr<IRenderContext> m_renderContext;
		UniquePtr<IImGuiRenderer> m_imGuiRenderer;
		RenderDeviceHandle m_device = nullptr;

		SharedPtr<IShaderCompiler> m_shaderCompiler;

		ArrayMap<IApplicationWindow*, RendererWindowData> m_windowDataMap;

		RenderGraph m_renderGraph;
	};
}
