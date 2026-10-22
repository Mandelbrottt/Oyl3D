#include "RendererProcessor.h"

#include <Core/Logging/Logging.h>
#include <Core/Modules/ModuleManager.h>
#include <Core/Profiling/Profiling.h>

#include "RenderEngine.h"
#include "Rendering.h"

namespace Oyl
{
	static constexpr uint32 MAX_FRAMES_IN_FLIGHT = 2;

	RendererProcessor::RendererProcessor(UniquePtr<IRenderer>&& a_renderer)
		: m_renderer(std::move(a_renderer))
	{
		OYL_ASSERT(m_renderer);

		auto renderingModule = ModuleManager::GetModulePointer<IRenderingModule>("Rendering");
		if (renderingModule)
			m_imGuiRenderer = renderingModule->CreateImGuiRenderer(m_renderer->GetRenderingApi());
	}

	RendererProcessor::~RendererProcessor()
	{
		Shutdown();
	}

	void
	RendererProcessor::Init(IApplicationWindow* a_mainWindow)
	{
		OYL_PROFILE_FUNCTION();

		if (!IsValid())
			return;

		m_renderer->Init();

		PresentTargetHandle presentTarget = nullptr;
		if (a_mainWindow)
		{
			RegisterApplicationWindow(*a_mainWindow);
			presentTarget = m_windowDataMap[a_mainWindow].presentTarget;
		}

		LogTrace("Creating Device");
		m_device = m_renderer->CreateDevice(presentTarget.Get());
		RenderEngine::Get().RegisterDevice(m_device);

		LogTrace("Creating ShaderCompiler");
		m_shaderCompiler = m_renderer->CreateShaderCompiler();
		RenderEngine::Get().RegisterShaderCompiler(m_shaderCompiler);

		// Init ImGuiRenderer after setting up the RenderEngine
		m_imGuiRenderer->Init(*m_renderer);
	}

	void
	RendererProcessor::Update()
	{
		OYL_PROFILE_FUNCTION();

		if (!IsValid())
			return;

		// Update renderer internal variables
		m_renderer->Update();

		// Update ImGuiRenderer before we render to windows
		if (m_imGuiRenderer)
			m_imGuiRenderer->Update(*m_renderer);

		// If any windows need to be registered with the renderer, do it now
		if (!m_windowDataPendingInitList.Empty())
		{
			for (auto* windowToInit : m_windowDataPendingInitList)
			{
				LogTrace("Creating RendererWindowData for window [{} \"{}\"]", windowToInit->GetNativeWindowHandle(), windowToInit->GetTitle());
				auto& windowDataToInit = m_windowDataMap[windowToInit];
				CreateSwapChain(*windowToInit, windowDataToInit);
				CreateCommandBuffers(windowDataToInit);
				CreateSyncObjects(windowDataToInit);
			}
			m_windowDataPendingInitList.Clear();
		}

		// Render to each window
		for (auto& [window, windowData] : m_windowDataMap)
		{
			RenderToWindow(windowData);
		}
	}

	void
	RendererProcessor::Shutdown()
	{
		OYL_PROFILE_FUNCTION();

		if (!IsValid())
			return;

		m_device->WaitUntilIdle();

		for (auto& [window, info] : m_windowDataMap)
		{
			UnregisterApplicationWindow(*window);
		}

		// Shutdown ImGuiRenderer before Renderer shuts down
		if (m_imGuiRenderer)
			m_imGuiRenderer->Shutdown(*m_renderer);

		m_renderer->Shutdown();

		m_windowDataMap.Clear();

		RenderEngine::Get().UnregisterDevice(m_device);
		m_device.Reset();

		RenderEngine::Get().UnregisterShaderCompiler(m_shaderCompiler);
		m_shaderCompiler.Reset();

		m_imGuiRenderer.Reset();
		m_renderer.Reset();
	}

	void
	RendererProcessor::RegisterApplicationWindow(IApplicationWindow& a_window)
	{
		OYL_PROFILE_FUNCTION();

		if (!IsValid())
			return;

		if (!a_window.IsValid())
			return;

		if (m_windowDataMap.Contains(&a_window))
			return;

		LogDebug("Registering window [{} \"{}\"] with RendererProcessor", a_window.GetNativeWindowHandle(), a_window.GetTitle());

		RendererWindowData info;
		LogTrace("Creating PresentTarget for window [{} \"{}\"]", a_window.GetNativeWindowHandle(), a_window.GetTitle());
		info.presentTarget = m_renderer->CreatePresentTarget(a_window);

		m_windowDataMap.Emplace(&a_window, std::move(info));
		m_windowDataPendingInitList.Add(&a_window);
	}

	void
	RendererProcessor::RenderToWindow(RendererWindowData& a_windowData)
	{
		OYL_PROFILE_FUNCTION();

		auto& drawFence = a_windowData.inFlightFences[a_windowData.frameIndex];
		auto& presentCompleteSemaphore = a_windowData.presentCompleteSemaphores[a_windowData.frameIndex];

		drawFence->Wait();

		auto& swapChain = *a_windowData.swapChain;
		bool success = swapChain.AcquireNextImage(presentCompleteSemaphore, nullptr);
		if (!success)
		{
			return;
		}

		auto imageIndex = swapChain.GetCurrentImageIndex();

		// Only reset fences if we are going to submit work to the GPU
		drawFence->Reset();

		RecordWindowCommandBuffer(a_windowData);

		auto& renderFinishedSemaphore = a_windowData.renderFinishedSemaphores[imageIndex];

		const auto& graphicsQueue = *m_device->GetCommandQueue(CommandQueueFlagBits::Graphics);
		graphicsQueue.Submit(
			CommandQueueSubmitParams {
				.commandBuffer = *a_windowData.commandBuffers[a_windowData.frameIndex],
				.waitSemaphore = *presentCompleteSemaphore,
				.signalSemaphore = *renderFinishedSemaphore,
				.fence = *drawFence,
				.waitDestinationStageMask = PipelineStageFlagBits::ColorAttachmentOutput
			}
		);

		bool result = graphicsQueue.Present(
			CommandQueuePresentParams {
				.swapChain = swapChain,
				.waitSemaphore = *renderFinishedSemaphore
			}
		);

		if (!result)
			RecreateSwapChain(a_windowData);

		a_windowData.frameIndex = (a_windowData.frameIndex + 1) % MAX_FRAMES_IN_FLIGHT;
	}

	void
	RendererProcessor::RecordWindowCommandBuffer(RendererWindowData& a_windowData)
	{
		OYL_PROFILE_FUNCTION();

		auto& commandBuffer = a_windowData.commandBuffers[a_windowData.frameIndex];

		commandBuffer->Begin();

		m_renderGraph.Execute(*commandBuffer);

		auto& swapChain = *a_windowData.swapChain;

		commandBuffer->TransitionImageLayout(
			swapChain.GetCurrentImageId(),
			ImageLayout::None,
			ImageLayout::TransferDest
		);

		auto& image = *m_renderGraph.GetSortedRenderPasses().front()->GetRenderTarget()->GetColorAttachment(0);
		commandBuffer->BlitImage(
			image.GetId(),
			ImageLayout::TransferSource,
			Rect2D(Vector2i::Zero, image.GetSize()),
			swapChain.GetCurrentImageId(),
			ImageLayout::TransferDest,
			Rect2D(Vector2i::Zero, swapChain.GetSize()),
			ImageFilter::Nearest
		);

		commandBuffer->TransitionImageLayout(
			swapChain.GetCurrentImageId(),
			ImageLayout::TransferDest,
			ImageLayout::PresentSource
		);

		commandBuffer->End();
	}

	void
	RendererProcessor::RecreateSwapChain(RendererWindowData& a_windowData)
	{
		a_windowData.swapChain->Recreate();
	}

	void
	RendererProcessor::UnregisterApplicationWindow(IApplicationWindow& a_window)
	{
		OYL_PROFILE_FUNCTION();

		if (!IsValid())
			return;

		if (!m_windowDataMap.Contains(&a_window))
			return;

		LogDebug("Unregistering window [{} {}] with RendererProcessor", a_window.GetNativeWindowHandle(), a_window.GetTitle());

		m_device->WaitUntilIdle();

		auto& windowData = m_windowDataMap[&a_window];
		for (auto& fence : windowData.inFlightFences)
		{
			fence->Wait();
			fence->Destroy();
		}
		for (auto& semaphore : windowData.renderFinishedSemaphores)
			semaphore->Destroy();
		for (auto& semaphore : windowData.presentCompleteSemaphores)
			semaphore->Destroy();
		for (auto& commandBuffer : windowData.commandBuffers)
			commandBuffer->Destroy();
		windowData.commandPool->Destroy();
		windowData.swapChain->Destroy();
		windowData.presentTarget->Destroy();
		m_windowDataMap.Erase(&a_window);
	}

	void
	RendererProcessor::CreateSwapChain(IApplicationWindow& a_window, RendererWindowData& a_windowData)
	{
		OYL_PROFILE_FUNCTION();
		a_windowData.swapChain = m_renderer->CreateSwapChain(m_device, a_window, a_windowData.presentTarget);
	}

	void
	RendererProcessor::CreateCommandBuffers(RendererWindowData& a_windowData)
	{
		OYL_PROFILE_FUNCTION();

		a_windowData.commandPool = m_device->CreateCommandPool({ .commandQueueFlags = CommandQueueFlagBits::Graphics });

		a_windowData.commandBuffers.Reserve(MAX_FRAMES_IN_FLIGHT);
		for (uint32 i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
		{
			auto commandBuffer = m_device->CreateCommandBuffer({ .commandPool = *a_windowData.commandPool, });
			a_windowData.commandBuffers.Add(std::move(commandBuffer));
		}
	}

	void
	RendererProcessor::CreateSyncObjects(RendererWindowData& a_windowData)
	{
		OYL_PROFILE_FUNCTION();

		OYL_ASSERT(
			a_windowData.renderFinishedSemaphores.Empty()
			&& a_windowData.presentCompleteSemaphores.Empty()
			&& a_windowData.inFlightFences.Empty());

		for (uint32 i = 0; i < a_windowData.swapChain->GetImageCount(); i++)
		{
			a_windowData.renderFinishedSemaphores.Add(m_device->CreateSemaphore());
		}

		for (uint32 i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
		{
			a_windowData.presentCompleteSemaphores.Add(m_device->CreateSemaphore());
			a_windowData.inFlightFences.Add(m_device->CreateFence());
		}
	}
}
