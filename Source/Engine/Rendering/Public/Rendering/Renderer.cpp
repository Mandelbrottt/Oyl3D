#include "Renderer.h"

#include <Core/Logging/Logging.h>
#include <Core/Modules/ModuleManager.h>
#include <Core/Profiling/Profiling.h>

#include "RenderEngine.h"
#include "Rendering.h"

#include "RenderPass/ColorOutputRenderPass.h"

namespace Oyl
{
	static constexpr uint32 MAX_FRAMES_IN_FLIGHT = 2;

	Renderer::Renderer(UniquePtr<IRenderContext>&& a_renderer)
		: m_renderContext(std::move(a_renderer))
	{
		OYL_ASSERT(m_renderContext);
	}

	Renderer::~Renderer()
	{
		Shutdown();
	}

	void
	Renderer::Init(IApplicationWindow* a_mainWindow)
	{
		OYL_PROFILE_FUNCTION();

		if (!IsValid())
			return;

		m_renderContext->Init();

		PresentTargetHandle presentTarget = nullptr;
		if (a_mainWindow)
		{
			RendererWindowData& data = m_windowDataMap[a_mainWindow];
			data.presentTarget = m_renderContext->CreatePresentTarget(*a_mainWindow);
			presentTarget = data.presentTarget;
		}

		LogTrace("Creating Device");
		m_device = m_renderContext->CreateDevice(presentTarget.Get());
		RenderEngine::Get().RegisterDevice(m_device);

		LogTrace("Creating ShaderCompiler");
		m_shaderCompiler = m_renderContext->CreateShaderCompiler();
		RenderEngine::Get().RegisterShaderCompiler(m_shaderCompiler);

		if (a_mainWindow)
		{
			// Register the main window to create its render primitives
			RegisterApplicationWindow(*a_mainWindow);

			// Only init ImGuiRenderer if we have a window for it to render to
			// Init ImGuiRenderer after setting up the RenderEngine
			auto renderingModule = ModuleManager::GetModulePointer<IRenderingModule>("Rendering");
			if (renderingModule)
				m_imGuiRenderer = renderingModule->CreateImGuiRenderer();

			if (m_imGuiRenderer)
				m_imGuiRenderer->Init(*this, *a_mainWindow);
		}
	}

	void
	Renderer::Update()
	{
		OYL_PROFILE_FUNCTION();

		if (!IsValid())
			return;

		// Update renderer internal variables
		m_renderContext->Update();

		// Update ImGuiRenderer before we render to windows
		if (m_imGuiRenderer)
			m_imGuiRenderer->Update();

		// Render to each window
		for (auto& [window, windowData] : m_windowDataMap)
		{
			RenderToWindow(windowData);
		}
	}

	void
	Renderer::Shutdown()
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
			m_imGuiRenderer->Shutdown();

		m_renderContext->Shutdown();

		m_windowDataMap.Clear();

		RenderEngine::Get().UnregisterDevice(m_device);
		m_device.Reset();

		RenderEngine::Get().UnregisterShaderCompiler(m_shaderCompiler);
		m_shaderCompiler.Reset();

		m_renderContext.Reset();
	}

	void
	Renderer::RegisterApplicationWindow(IApplicationWindow& a_window)
	{
		OYL_PROFILE_FUNCTION();

		if (!IsValid())
			return;

		if (!a_window.IsValid())
			return;

		RendererWindowData& windowData = m_windowDataMap[&a_window];
		if (windowData.swapChain)
			return;

		LogDebug("Registering window [{} \"{}\"] with RendererProcessor", a_window.GetNativeWindowHandle(), a_window.GetTitle());

		windowData.window = &a_window;

		if (!windowData.presentTarget)
			windowData.presentTarget = m_renderContext->CreatePresentTarget(a_window);

		CreateSwapChain(windowData);
		CreateCommandBuffers(windowData);
		CreateSyncObjects(windowData);
	}

	void
	Renderer::UnregisterApplicationWindow(IApplicationWindow& a_window)
	{
		OYL_PROFILE_FUNCTION();

		if (!IsValid())
			return;

		if (!m_windowDataMap.ContainsKey(&a_window))
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

	const RendererWindowData*
	Renderer::GetWindowData(IApplicationWindow& a_window) const
	{
		if (!m_windowDataMap.ContainsKey(&a_window))
			return nullptr;

		return &m_windowDataMap.At(&a_window);
	}

	void
	Renderer::RenderToWindow(RendererWindowData& a_windowData)
	{
		OYL_PROFILE_FUNCTION();

		auto& drawFence = *a_windowData.inFlightFences[a_windowData.frameIndex];
		auto& presentCompleteSemaphore = *a_windowData.presentCompleteSemaphores[a_windowData.frameIndex];

		drawFence.Wait();

		auto& swapChain = *a_windowData.swapChain;
		bool success = swapChain.AcquireNextImage(*presentCompleteSemaphore, {});
		if (!success)
		{
			return;
		}

		auto imageIndex = swapChain.GetCurrentImageIndex();

		// Only reset fences if we are going to submit work to the GPU
		drawFence.Reset();

		RecordWindowCommandBuffer(a_windowData);

		auto& renderFinishedSemaphore = *a_windowData.renderFinishedSemaphores[imageIndex];

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
	Renderer::RecordWindowCommandBuffer(RendererWindowData& a_windowData)
	{
		OYL_PROFILE_FUNCTION();

		auto& commandBuffer = a_windowData.commandBuffers[a_windowData.frameIndex];

		commandBuffer->Begin();

		m_renderGraph.Execute(*commandBuffer);

		ImageHandle finalColorOutput = nullptr;

		auto& sortedPasses = m_renderGraph.GetSortedRenderPasses();
		if (!sortedPasses.Empty())
		{
			auto* finalRenderPass = dynamic_cast<IColorOutputRenderPass*>(sortedPasses.back());
			if (finalRenderPass == nullptr)
				LogError("Final RenderPass doesn't provide a Color Output, it needs to derive from \"{}\"!",
				         nameof(IColorOutputRenderPass));
			else
				finalColorOutput = finalRenderPass->GetColorAttachment();
		}

		if (m_imGuiRenderer)
		{
			m_imGuiRenderer->SetGameViewPortImageHandle(finalColorOutput);
			m_imGuiRenderer->Render(commandBuffer);
			finalColorOutput = m_imGuiRenderer->GetRenderedImageHandle();
		}

		if (finalColorOutput)
		{
			auto& swapChain = *a_windowData.swapChain;

			commandBuffer->TransitionImageLayout(
				finalColorOutput,
				ImageLayout::TransferSource
			);

			commandBuffer->TransitionImageLayout(
				swapChain.GetCurrentImageId(),
				ImageLayout::None,
				ImageLayout::TransferDest
			);

			commandBuffer->BlitImage(
				**finalColorOutput,
				ImageLayout::TransferSource,
				Rect2D(Vector2i::Zero, finalColorOutput->GetSize()),
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
		} else
		{
			LogError("No image is being rendered to swap chain for window [{}]!",
			         a_windowData.window->GetNativeWindowHandle());
		}

		commandBuffer->End();
	}

	void
	Renderer::RecreateSwapChain(RendererWindowData& a_windowData)
	{
		a_windowData.swapChain->Recreate();
	}

	void
	Renderer::OnRecreateSwapChain(const ISwapChain& a_swapChain)
	{
		m_renderGraph.ResizeViewPort(a_swapChain.GetSize());
	}

	void
	Renderer::CreateSwapChain(RendererWindowData& a_windowData)
	{
		OYL_PROFILE_FUNCTION();
		a_windowData.swapChain = m_renderContext->CreateSwapChain(m_device, *a_windowData.window, a_windowData.presentTarget);
		a_windowData.swapChain->AddOnRecreateCallback({ this, &Renderer::OnRecreateSwapChain });
	}

	void
	Renderer::CreateCommandBuffers(RendererWindowData& a_windowData)
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
	Renderer::CreateSyncObjects(RendererWindowData& a_windowData)
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
