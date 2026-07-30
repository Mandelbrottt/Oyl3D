#include "VulkanRenderContext.h"

#include <vulkan/vulkan_raii.hpp>

#include "VulkanDevice.h"
#include "VulkanImage.h"
#include "VulkanCommandQueue.h"
#include "VulkanShader.h"
#include "VulkanSwapChain.h"
#include "VulkanVertexBuffer.h"

#include "Rendering/RenderEngine.h"
#include "Rendering/RenderTarget.h"
#include "Rendering/Glfw/GlfwWindow.h"
#include "Rendering/Vulkan/VulkanCommandBuffer.h"
#include "Rendering/Vulkan/VulkanCommandPool.h"

static const std::vector REQUIRED_DEVICE_EXTENSION {
	vk::KHRSwapchainExtensionName,
};

constexpr int MAX_FRAMES_IN_FLIGHT = 2;

namespace Oyl::Rendering::Vulkan
{
	struct RenderContext::Impl
	{
		const IWindow* window;

		Device device = nullptr;
		SwapChain swapChain;

		Image image;
		RenderTarget renderTarget;

		// TEMPORARY:
		Shader shader;

		// TEMPORARY:
		VertexBuffer vertexBuffer;

		CommandPool commandPool;
		std::vector<CommandBuffer> commandBuffers;

		std::vector<Semaphore> presentCompleteSemaphores;
		std::vector<Semaphore> renderFinishedSemaphores;
		std::vector<Fence> inFlightFences;

		uint32 frameIndex = 0;

		void
		CreateSyncObjects();
		void
		RecordCommandBuffer();
		void
		DrawFrame();
		void
		RecreateSwapChain();
	};

	RenderContext::RenderContext() noexcept
		: m_impl(nullptr) {}

	RenderContext::RenderContext(const CreateParams& a_params) noexcept
		: m_impl(nullptr)
	{
		Init(a_params);
	}

	RenderContext::RenderContext(RenderContext&& a_other) noexcept
		: m_impl(nullptr)
	{
		*this = std::move(a_other);
	}

	RenderContext&
	RenderContext::operator=(RenderContext&& a_other) noexcept
	{
		if (this != &a_other)
		{
			m_impl = std::move(a_other.m_impl);
		}
		return *this;
	}

	RenderContext::~RenderContext()
	{
		RenderContext::Destroy();
	}

	void
	RenderContext::Init(const CreateParams& a_params)
	{
		OYL_PROFILE_FUNCTION();

		if (!m_impl)
			m_impl = std::make_unique<Impl>();

		m_impl->window = a_params.window;

		m_impl->device = DeviceImpl::Create(
			{
				.window = *m_impl->window,
				.commandQueueFlags = CommandQueueFlagBits::Graphics | CommandQueueFlagBits::Transfer,
				.ppRequiredDeviceExtensionsData = REQUIRED_DEVICE_EXTENSION.data(),
				.requiredDeviceExtensionsLength = REQUIRED_DEVICE_EXTENSION.size()
			}
		);

		//m_impl->graphicsQueue = CommandQueue(
		//	{
		//		.device = m_impl->device,
		//		.queueFamilyIndex = m_impl->device.GetVkGraphicsQueueFamilyIndex()
		//	}
		//);

		m_impl->swapChain = SwapChain(
			*m_impl->device,
			{
				.window = *m_impl->window,
			}
		);

		const std::vector vertices {
			Vertex { Vector2f { -0.5f, -0.5f }, Vector3f { 1.0f, 0.0f, 0.0f } },
			Vertex { Vector2f { 0.5f, -0.5f }, Vector3f { 0.0f, 1.0f, 0.0f } },
			Vertex { Vector2f { 0.5f, 0.5f }, Vector3f { 0.0f, 0.0f, 1.0f } },
			Vertex { Vector2f { -0.5f, 0.5f }, Vector3f { 1.0f, 1.0f, 1.0f } },
		};

		const std::vector<uint16> indices { 0, 1, 2, 2, 3, 0 };

		auto verticesBuffer = reinterpret_cast<const byte*>(vertices.data());
		auto indicesBuffer = reinterpret_cast<const byte*>(indices.data());

		m_impl->vertexBuffer = VertexBuffer(
			*m_impl->device,
			{
				.vertexData = verticesBuffer,
				.vertexLength = (uint32) (vertices.size() * sizeof(decltype(vertices)::value_type)),
				.vertexStride = sizeof(decltype(vertices)::value_type),
				.indexData = indicesBuffer,
				.indexLength = (uint32) (indices.size() * sizeof(decltype(indices)::value_type))
			}
		);

		m_impl->image = Image(
			*m_impl->device,
			{
				.size = m_impl->swapChain.GetSize(),
				.vkFormat = m_impl->swapChain.GetVkSurfaceFormat().format,
				.vkUsage = vk::ImageUsageFlagBits::eColorAttachment
				           | vk::ImageUsageFlagBits::eTransferSrc,
				.vkProperties = vk::MemoryPropertyFlagBits::eDeviceLocal,
				.vkLayout = vk::ImageLayout::eColorAttachmentOptimal
			}
		);

		const Rendering::Image* image = &m_impl->image;
		m_impl->renderTarget = RenderTarget(
			{
				.pColorImages = &image,
				.colorImageLength = 1,
			}
		);

		m_impl->commandPool = CommandPool(*m_impl->device, { .commandQueueFlags = CommandQueueFlagBits::Graphics });

		m_impl->commandBuffers.reserve(MAX_FRAMES_IN_FLIGHT);
		for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
		{
			m_impl->commandBuffers.emplace_back(
				*m_impl->device,
				CommandBuffer::CreateParams {
					.commandPool = m_impl->commandPool,
				}
			);
		}

		m_impl->CreateSyncObjects();
	}

	void
	RenderContext::Update()
	{
		OYL_PROFILE_FUNCTION();

		if (!m_impl->window || !m_impl->window->IsValid())
			return;

		if (!m_impl->shader)
		{
			auto* compiler = RenderEngine::GetShaderCompiler();
			ShaderCompileResult result;
			compiler->CompileHlslFromFile("G:/dev/Oyl3D/Oyl3D/Source/Oyl3D/Engine/Rendering/Shaders/shader.hlsl", &result);

			m_impl->shader = Shader(
				{
					.device = *m_impl->device,
					.format = m_impl->swapChain.GetVkSurfaceFormat().format,
					.compileResult = result
				}
			);
		}

		m_impl->DrawFrame();
	}

	void
	RenderContext::Destroy()
	{
		OYL_PROFILE_FUNCTION();

		if (!m_impl)
			return;

		m_impl->device->WaitUntilIdle();

		m_impl->renderFinishedSemaphores.clear();
		m_impl->presentCompleteSemaphores.clear();
		m_impl->inFlightFences.clear();

		m_impl->commandBuffers.clear();
		m_impl->commandPool.Destroy();

		m_impl->renderTarget.Destroy();
		m_impl->image.Destroy();

		m_impl->vertexBuffer.Destroy();
		m_impl->shader.Destroy();

		m_impl->swapChain.Destroy();
		m_impl->device->Destroy();

		*m_impl = {};
	}

	void
	RenderContext::Resize(Vector2i /*a_size*/)
	{
		// No need to pass in a_size - we get the size from the window directly
		m_impl->RecreateSwapChain();
	}

	const DeviceImpl*
	RenderContext::GetDevice() const
	{
		return m_impl->device.Get();
	}

	const SwapChain*
	RenderContext::GetSwapChain() const
	{
		return &m_impl->swapChain;
	}

	void
	RenderContext::Impl::CreateSyncObjects()
	{
		OYL_PROFILE_FUNCTION();

		OYL_ASSERT(renderFinishedSemaphores.empty() && presentCompleteSemaphores.empty() && inFlightFences.empty());

		for (size_t i = 0; i < swapChain.GetVkImages().size(); i++)
		{
			renderFinishedSemaphores.emplace_back(*device);
		}

		for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
		{
			presentCompleteSemaphores.emplace_back(*device);
			inFlightFences.emplace_back(*device);
		}
	}

	static
	void
	VkTransitionImageLayout(
		const CommandBuffer& a_commandBuffer,
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
	RenderContext::Impl::RecordCommandBuffer()
	{
		OYL_PROFILE_FUNCTION();

		auto& commandBuffer = commandBuffers[frameIndex];

		commandBuffer.Begin();

		Vector2u extent = Vector2u(
			swapChain.GetVkExtent().width,
			swapChain.GetVkExtent().height
		);
		commandBuffer.SetViewport(Vector2i::Zero(), extent);
		commandBuffer.SetScissor(Vector2i::Zero(), extent);

		image.VkTransitionImageLayout(
			commandBuffer,
			vk::ImageLayout::eUndefined,
			vk::ImageLayout::eColorAttachmentOptimal,
			{},
			vk::AccessFlagBits2::eColorAttachmentWrite,
			vk::PipelineStageFlagBits2::eColorAttachmentOutput,
			vk::PipelineStageFlagBits2::eColorAttachmentOutput
		);

		commandBuffer.BeginRendering(renderTarget);

		commandBuffer.BindShader(shader);
		commandBuffer.DrawVertexBuffer(vertexBuffer);

		commandBuffer.EndRendering();

		image.VkTransitionImageLayout(
			commandBuffer,
			vk::ImageLayout::eColorAttachmentOptimal,
			vk::ImageLayout::eTransferSrcOptimal,
			vk::AccessFlagBits2::eColorAttachmentWrite,
			{},
			vk::PipelineStageFlagBits2::eColorAttachmentOutput,
			vk::PipelineStageFlagBits2::eBottomOfPipe
		);

		VkTransitionImageLayout(
			commandBuffer,
			swapChain.GetCurrentVkImage(),
			vk::ImageLayout::eUndefined,
			vk::ImageLayout::eTransferDstOptimal,
			vk::AccessFlagBits2::eTransferWrite,
			{},
			vk::PipelineStageFlagBits2::eTransfer,
			vk::PipelineStageFlagBits2::eBlit
		);
		{
			OYL_PROFILE_SCOPE("BlitImage");

			commandBuffer.GetVkCommandBuffer().blitImage(
				image.GetVkImage(),
				vk::ImageLayout::eTransferSrcOptimal,
				swapChain.GetCurrentVkImage(),
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
			commandBuffer,
			swapChain.GetCurrentVkImage(),
			vk::ImageLayout::eTransferDstOptimal,
			vk::ImageLayout::ePresentSrcKHR,
			{},
			vk::AccessFlagBits2::eTransferRead,
			vk::PipelineStageFlagBits2::eBlit,
			vk::PipelineStageFlagBits2::eBlit
		);

		commandBuffer.End();
	}

	void
	RenderContext::Impl::DrawFrame()
	{
		OYL_PROFILE_FUNCTION();

		if (!swapChain)
			return;

		auto& drawFence = inFlightFences[frameIndex];
		auto& presentCompleteSemaphore = presentCompleteSemaphores[frameIndex];

		drawFence.Wait();

		bool success = swapChain.AcquireNextImage(presentCompleteSemaphore, nullptr);
		if (!success)
		{
			RecreateSwapChain();
			return;
		}

		auto imageIndex = swapChain.GetCurrentImageIndex();

		// Only reset fences if we are going to submit work to the GPU
		drawFence.Reset();

		RecordCommandBuffer();

		auto& renderFinishedSemaphore = renderFinishedSemaphores[imageIndex];

		auto& commandBuffer = commandBuffers[frameIndex];

		const auto& graphicsQueue = *device->GetCommandQueue(CommandQueueFlagBits::Graphics);
		graphicsQueue.Submit(
			CommandQueue::SubmitParams {
				.commandBuffer = commandBuffer,
				.waitSemaphore = presentCompleteSemaphore,
				.signalSemaphore = renderFinishedSemaphore,
				.fence = drawFence,
				.waitDestinationStageMask = vk::PipelineStageFlagBits::eColorAttachmentOutput
			}
		);

		bool result = graphicsQueue.Present(
			CommandQueue::PresentParams {
				.waitSemaphore = renderFinishedSemaphore,
				.swapChain = swapChain
			}
		);

		if (!result)
			RecreateSwapChain();

		frameIndex = (frameIndex + 1) % MAX_FRAMES_IN_FLIGHT;
	}

	void
	RenderContext::Impl::RecreateSwapChain()
	{
		OYL_PROFILE_FUNCTION();

		swapChain.Recreate();
		image = Image(
			*device,
			{
				.size = swapChain.GetSize(),
				.vkFormat = swapChain.GetVkSurfaceFormat().format,
				.vkUsage = vk::ImageUsageFlagBits::eColorAttachment
				           | vk::ImageUsageFlagBits::eTransferSrc,
				.vkProperties = vk::MemoryPropertyFlagBits::eDeviceLocal,
				.vkLayout = vk::ImageLayout::eColorAttachmentOptimal
			}
		);
	}
}
