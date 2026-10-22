#pragma once

#include <Application/ApplicationWindow.h>

#include <Core/Containers/ArrayMap.h>
#include <Core/Containers/UniquePtr.h>

#include "Device.h"
#include "PresentTarget.h"
#include "RenderGraph.h"

#include "ImGui/ImGuiRenderer.h"

#include "Rendering/Renderer.h"

namespace Oyl
{
	struct RendererWindowData
	{
		PresentTargetHandle presentTarget = nullptr;
		SwapChainHandle swapChain = nullptr;

		CommandPoolHandle commandPool = nullptr;
		Array<CommandBufferHandle> commandBuffers;

		Array<SemaphoreHandle> presentCompleteSemaphores;
		Array<SemaphoreHandle> renderFinishedSemaphores;
		Array<FenceHandle> inFlightFences;

		uint32 frameIndex = 0;
	};

	class RENDERING_EXPORT RendererProcessor final
	{
	public:
		RendererProcessor() = default;

		explicit
		RendererProcessor(UniquePtr<IRenderer>&& a_renderer);

		NO_COPY(RendererProcessor);
		DEFAULT_MOVE(RendererProcessor);

		~RendererProcessor();

		void
		Init(IApplicationWindow* a_mainWindow);

		void
		Update();

		void
		Shutdown();

		bool
		IsValid() const noexcept { return m_renderer; }

		void
		RegisterApplicationWindow(IApplicationWindow& a_window);

		void
		UnregisterApplicationWindow(IApplicationWindow& a_window);

		RenderGraph&
		GetRenderGraph() { return m_renderGraph; }

		const RenderGraph&
		GetRenderGraph() const { return const_cast<RendererProcessor*>(this)->GetRenderGraph(); }

	private:
		void
		CreateSwapChain(IApplicationWindow& a_window, RendererWindowData& a_windowData);

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

	private:
		UniquePtr<IRenderer> m_renderer;
		UniquePtr<IImGuiRenderer> m_imGuiRenderer;
		DeviceHandle m_device = nullptr;

		SharedPtr<IShaderCompiler> m_shaderCompiler;

		ArrayMap<IApplicationWindow*, RendererWindowData> m_windowDataMap;
		Array<IApplicationWindow*> m_windowDataPendingInitList;

		RenderGraph m_renderGraph;
	};
}