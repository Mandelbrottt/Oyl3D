#include "Renderer.h"

#include "CommandBuffer.h"
#include "CommandPool.h"
#include "Device.h"
#include "Fence.h"
#include "RenderContext.h"
#include "Semaphore.h"
#include "Shader.h"
#include "TestRenderPass.h"

static constexpr uint32 MAX_FRAMES_IN_FLIGHT = 2;

namespace Oyl::Rendering
{
	struct Renderer::Impl
	{
		RenderContext* renderContext = nullptr;
		RenderGraph renderGraph;

		CommandPoolHandle commandPool = nullptr;
		Array<CommandBufferHandle> commandBuffers;

		Array<SemaphoreHandle> presentCompleteSemaphores;
		Array<SemaphoreHandle> renderFinishedSemaphores;
		Array<FenceHandle> inFlightFences;

		uint32 frameIndex = 0;

		void
		CreateCommandBuffers();
		void
		CreateSyncObjects();
	};

	Renderer::Renderer(RenderContext& a_renderContext)
		: m_impl(std::make_unique<Impl>())
	{
		m_impl->renderContext = &a_renderContext;

		m_impl->CreateCommandBuffers();
		m_impl->CreateSyncObjects();
	}

	Renderer::~Renderer() {}

	void
	Renderer::Render()
	{
		OYL_PROFILE_FUNCTION();

		auto& drawFence = m_impl->inFlightFences[m_impl->frameIndex];
		auto& presentCompleteSemaphore = m_impl->presentCompleteSemaphores[m_impl->frameIndex];

		drawFence->Wait();

		auto& renderContext = *m_impl->renderContext;
		auto& swapChain = *renderContext.GetSwapChain();
		bool success = swapChain.AcquireNextImage(presentCompleteSemaphore, nullptr);
		if (!success)
		{
			return;
		}

		auto imageIndex = swapChain.GetCurrentImageIndex();

		// Only reset fences if we are going to submit work to the GPU
		drawFence->Reset();

		RecordCommandBuffer();

		auto& renderFinishedSemaphore = m_impl->renderFinishedSemaphores[imageIndex];

		auto& device = *renderContext.GetDevice();
		const auto& graphicsQueue = *device.GetCommandQueue(CommandQueueFlagBits::Graphics);
		graphicsQueue.Submit(
			CommandQueue::SubmitParams {
				.commandBuffer = *m_impl->commandBuffers[m_impl->frameIndex],
				.waitSemaphore = *presentCompleteSemaphore,
				.signalSemaphore = *renderFinishedSemaphore,
				.fence = *drawFence,
				.waitDestinationStageMask = PipelineStageFlagBits::ColorAttachmentOutput
			}
		);

		bool result = graphicsQueue.Present(
			CommandQueue::PresentParams {
				.swapChain = swapChain,
				.waitSemaphore = *renderFinishedSemaphore
			}
		);

		if (!result)
			RecreateSwapChain();

		m_impl->frameIndex = (m_impl->frameIndex + 1) % MAX_FRAMES_IN_FLIGHT;
	}

	RenderGraph&
	Renderer::GetRenderGraph()
	{
		return m_impl->renderGraph;
	}

	void
	Renderer::RecordCommandBuffer()
	{
		OYL_PROFILE_FUNCTION();

		auto& commandBuffer = m_impl->commandBuffers[m_impl->frameIndex];

		commandBuffer->Begin();

		m_impl->renderGraph.Execute(*commandBuffer);

		auto& renderContext = *m_impl->renderContext;
		auto& swapChain = *renderContext.GetSwapChain();

		commandBuffer->TransitionImageLayout(
			swapChain.GetCurrentImageId(),
			ImageLayout::None,
			ImageLayout::TransferDest
		);

		{
			auto& image = *m_impl->renderGraph.GetSortedRenderPasses().Back()->GetRenderTarget()->GetColorAttachment(0);
			commandBuffer->BlitImage(
				image.GetId(),
				ImageLayout::TransferSource,
				Rect2D(Vector2i::Zero(), image.GetSize()),
				swapChain.GetCurrentImageId(),
				ImageLayout::TransferDest,
				Rect2D(Vector2i::Zero(), swapChain.GetSize()),
				ImageFilter::Nearest
			);
		}

		commandBuffer->TransitionImageLayout(
			swapChain.GetCurrentImageId(),
			ImageLayout::TransferDest,
			ImageLayout::PresentSource
		);

		commandBuffer->End();
	}

	void
	Renderer::RecreateSwapChain()
	{
		m_impl->renderContext->Resize(m_impl->renderContext->GetSwapChain()->GetSize());
	}

	void
	Renderer::Impl::CreateCommandBuffers()
	{
		OYL_PROFILE_FUNCTION();

		auto& device = *renderContext->GetDevice();

		commandPool = device.CreateCommandPool({ .commandQueueFlags = CommandQueueFlagBits::Graphics });

		commandBuffers.Reserve(MAX_FRAMES_IN_FLIGHT);
		for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
		{
			auto commandBuffer = device.CreateCommandBuffer({ .commandPool = *commandPool, });
			commandBuffers.Add(std::move(commandBuffer));
		}
	}

	void
	Renderer::Impl::CreateSyncObjects()
	{
		OYL_PROFILE_FUNCTION();

		OYL_ASSERT(renderFinishedSemaphores.Empty() && presentCompleteSemaphores.Empty() && inFlightFences.Empty());

		auto& device = *renderContext->GetDevice();
		auto& swapChain = *renderContext->GetSwapChain();

		for (size_t i = 0; i < swapChain.GetImageCount(); i++)
		{
			renderFinishedSemaphores.Add(device.CreateSemaphore());
		}

		for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
		{
			presentCompleteSemaphores.Add(device.CreateSemaphore());
			inFlightFences.Add(device.CreateFence());
		}
	}
}
