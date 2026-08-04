#include "Renderer.h"

#include <vulkan/vulkan_raii.hpp>

#include "CommandBuffer.h"
#include "CommandPool.h"
#include "Device.h"
#include "Fence.h"
#include "RenderContext.h"
#include "RenderEngine.h"
#include "Semaphore.h"
#include "Shader.h"
#include "ShaderCompiler.h"
#include "TestRenderPass.h"

#include "Vulkan/VulkanCommandBuffer.h"
#include "Vulkan/VulkanImage.h"
#include "Vulkan/VulkanSwapChain.h"

static constexpr uint32 MAX_FRAMES_IN_FLIGHT = 2;

namespace Oyl::Rendering
{
	struct Renderer::Impl
	{
		RenderContext* renderContext = nullptr;
		RenderGraph renderGraph;

		CommandPool commandPool = nullptr;
		Array<CommandBuffer> commandBuffers;

		Array<Semaphore> presentCompleteSemaphores;
		Array<Semaphore> renderFinishedSemaphores;
		Array<Fence> inFlightFences;

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
			CommandQueueImpl::SubmitParams {
				.commandBuffer = *m_impl->commandBuffers[m_impl->frameIndex],
				.waitSemaphore = *presentCompleteSemaphore,
				.signalSemaphore = *renderFinishedSemaphore,
				.fence = *drawFence,
				.waitDestinationStageMask = PipelineStageFlagBits::ColorAttachmentOutput
			}
		);

		bool result = graphicsQueue.Present(
			CommandQueueImpl::PresentParams {
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

	static
	void
	VkTransitionImageLayout(
		const Vulkan::CommandBufferImpl& a_commandBuffer,
		vk::Image a_image,
		vk::ImageLayout a_oldLayout,
		vk::ImageLayout a_newLayout,
		vk::AccessFlags2 a_srcAccessMask,
		vk::AccessFlags2 a_dstAccessMask,
		vk::PipelineStageFlags2 a_srcStageMask,
		vk::PipelineStageFlags2 a_dstStageMask
	)
	{
		OYL_PROFILE_FUNCTION();

		vk::ImageMemoryBarrier2 barrier = {
			.srcStageMask = a_srcStageMask,
			.srcAccessMask = a_srcAccessMask,
			.dstStageMask = a_dstStageMask,
			.dstAccessMask = a_dstAccessMask,
			.oldLayout = a_oldLayout,
			.newLayout = a_newLayout,
			.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
			.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
			.image = a_image,
			.subresourceRange = {
				.aspectMask = vk::ImageAspectFlagBits::eColor,
				.baseMipLevel = 0,
				.levelCount = 1,
				.baseArrayLayer = 0,
				.layerCount = 1
			}
		};
		vk::DependencyInfo dependency_info = {
			.dependencyFlags = {},
			.imageMemoryBarrierCount = 1,
			.pImageMemoryBarriers = &barrier
		};
		const auto& vkCommandBuffer = a_commandBuffer.GetVkCommandBuffer();
		vkCommandBuffer.pipelineBarrier2(dependency_info);
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

		auto& vulkanCommandBuffer = dynamic_cast<Vulkan::CommandBufferImpl&>(*commandBuffer);
		auto& vulkanSwapChain = dynamic_cast<Vulkan::SwapChainImpl&>(swapChain);
		VkTransitionImageLayout(
			vulkanCommandBuffer,
			vulkanSwapChain.GetCurrentVkImage(),
			vk::ImageLayout::eUndefined,
			vk::ImageLayout::eTransferDstOptimal,
			vk::AccessFlagBits2::eTransferWrite,
			{},
			vk::PipelineStageFlagBits2::eTransfer,
			vk::PipelineStageFlagBits2::eBlit
		);
		{
			OYL_PROFILE_SCOPE("BlitImage");

			auto& image = *m_impl->renderGraph.GetSortedRenderPasses().Back()->GetRenderTarget()->GetColorAttachment(0);
			auto& vulkanImage = dynamic_cast<const Vulkan::ImageImpl&>(image);
			vulkanCommandBuffer.GetVkCommandBuffer().blitImage(
				vulkanImage.GetVkImage(),
				vk::ImageLayout::eTransferSrcOptimal,
				vulkanSwapChain.GetCurrentVkImage(),
				vk::ImageLayout::eTransferDstOptimal,
				vk::ImageBlit {
					.srcSubresource = { .aspectMask = vk::ImageAspectFlagBits::eColor, .mipLevel = 0, .baseArrayLayer = 0, .layerCount = 1 },
					.srcOffsets = std::array { vk::Offset3D { 0, 0, 0 }, vk::Offset3D { (int) image.GetSize().x, (int) image.GetSize().y, 1 } },
					.dstSubresource = { .aspectMask = vk::ImageAspectFlagBits::eColor, .mipLevel = 0, .baseArrayLayer = 0, .layerCount = 1 },
					.dstOffsets = std::array { vk::Offset3D { 0, 0, 0 }, vk::Offset3D { (int) swapChain.GetSize().x, (int) swapChain.GetSize().y, 1 } }
				},
				vk::Filter::eNearest
			);
		}

		VkTransitionImageLayout(
			vulkanCommandBuffer,
			vulkanSwapChain.GetCurrentVkImage(),
			vk::ImageLayout::eTransferDstOptimal,
			vk::ImageLayout::ePresentSrcKHR,
			{},
			vk::AccessFlagBits2::eTransferRead,
			vk::PipelineStageFlagBits2::eBlit,
			vk::PipelineStageFlagBits2::eBlit
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
