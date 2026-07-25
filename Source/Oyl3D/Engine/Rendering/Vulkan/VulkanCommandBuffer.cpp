#include "VulkanCommandBuffer.h"

#include <vulkan/vulkan_raii.hpp>

#include "VulkanCommandPool.h"
#include "VulkanDevice.h"
#include "VulkanSwapChain.h"

namespace Oyl::Rendering::Vulkan
{
	struct CommandBuffer::Impl
	{
		vk::raii::CommandBuffer commandBuffer = nullptr;

		void
		TransitionImageLayout(
			vk::Image a_image,
			vk::ImageLayout a_oldLayout,
			vk::ImageLayout a_new_layout,
			vk::AccessFlags2 a_srcAccessMask,
			vk::AccessFlags2 a_dstAccessMask,
			vk::PipelineStageFlags2 a_srcStageMask,
			vk::PipelineStageFlags2 a_dstStageMask
		) const noexcept;
	};

	CommandBuffer::CommandBuffer() noexcept
		: Rendering::CommandBuffer(),
		  m_impl(nullptr) {}

	CommandBuffer::CommandBuffer(const CreateParams& a_params) noexcept
		: Rendering::CommandBuffer(&a_params.commandPool),
		  m_impl(std::make_unique<Impl>())
	{
		OYL_PROFILE_FUNCTION();

		const auto& vkCommandPool = a_params.commandPool.GetVkCommandPool();
		const auto& vkDevice = a_params.device.GetVkDevice();

		vk::CommandBufferAllocateInfo allocInfo {
			.commandPool = vkCommandPool,
			.level = vk::CommandBufferLevel::ePrimary,
			.commandBufferCount = 1
		};

		// We can only create command buffers through CommandBuffers vector constructor
		m_impl->commandBuffer = std::move(vk::raii::CommandBuffers(vkDevice, allocInfo).front());
	}

	CommandBuffer::CommandBuffer(CommandBuffer&& a_other) noexcept
		: Rendering::CommandBuffer(std::move(a_other))
	{
		*this = std::move(a_other);
	}

	CommandBuffer&
	CommandBuffer::operator=(CommandBuffer&& a_other) noexcept
	{
		if (this != &a_other)
		{
			Rendering::CommandBuffer::operator=(std::move(a_other));
			std::swap(m_impl, a_other.m_impl);
		}
		return *this;
	}

	CommandBuffer::~CommandBuffer() noexcept
	{
		CommandBuffer::Destroy();
	}

	void
	CommandBuffer::Destroy() noexcept
	{
		m_impl->commandBuffer.clear();

		Rendering::CommandBuffer::Destroy();
	}

	bool
	CommandBuffer::IsValid() const noexcept
	{
		return m_impl
		       && *m_impl->commandBuffer
		       && Rendering::CommandBuffer::IsValid();
	}

	const CommandPool*
	CommandBuffer::GetCommandPool() const noexcept
	{
		return static_cast<const CommandPool*>(Rendering::CommandBuffer::GetCommandPool());
	}

	const vk::raii::CommandBuffer&
	CommandBuffer::GetVkCommandBuffer() const noexcept
	{
		return m_impl->commandBuffer;
	}

	void
	CommandBuffer::Begin() const noexcept
	{
		m_impl->commandBuffer.begin({});
	}

	void
	CommandBuffer::BeginRendering(const SwapChain& a_swapChain) const noexcept
	{
		OYL_PROFILE_FUNCTION();

		auto currentVkImage = a_swapChain.GetCurrentVkImage();
		m_impl->TransitionImageLayout(
			currentVkImage,
			vk::ImageLayout::eUndefined,
			vk::ImageLayout::eColorAttachmentOptimal,
			{},
			vk::AccessFlagBits2::eColorAttachmentWrite,
			vk::PipelineStageFlagBits2::eColorAttachmentOutput,
			vk::PipelineStageFlagBits2::eColorAttachmentOutput
		);

		const auto& currentVkImageView = a_swapChain.GetCurrentVkImageView();
		vk::ClearValue clearColor = vk::ClearColorValue(0.0f, 0.0f, 0.0f, 1.0f);
		vk::RenderingAttachmentInfo attachmentInfo = {
			.imageView = currentVkImageView,
			.imageLayout = vk::ImageLayout::eColorAttachmentOptimal,
			.loadOp = vk::AttachmentLoadOp::eClear,
			.storeOp = vk::AttachmentStoreOp::eStore,
			.clearValue = clearColor
		};

		const auto& swapChainExtent = a_swapChain.GetVkExtent();

		vk::RenderingInfo renderingInfo = {
			.renderArea = { .offset = { 0, 0 }, .extent = swapChainExtent },
			.layerCount = 1,
			.colorAttachmentCount = 1,
			.pColorAttachments = &attachmentInfo
		};

		m_impl->commandBuffer.beginRendering(renderingInfo);
	}

	void
	CommandBuffer::End() const noexcept
	{
		m_impl->commandBuffer.end();
	}

	void
	CommandBuffer::EndRendering(const SwapChain& a_swapChain) const noexcept
	{
		OYL_PROFILE_FUNCTION();

		m_impl->commandBuffer.endRendering();

		m_impl->TransitionImageLayout(
			a_swapChain.GetCurrentVkImage(),
			vk::ImageLayout::eColorAttachmentOptimal,
			vk::ImageLayout::ePresentSrcKHR,
			vk::AccessFlagBits2::eColorAttachmentWrite,
			{},
			vk::PipelineStageFlagBits2::eColorAttachmentOutput,
			vk::PipelineStageFlagBits2::eBottomOfPipe
		);
	}

	void
	CommandBuffer::SetViewport(Vector2i a_offset, Vector2u a_size) const noexcept
	{
		OYL_PROFILE_FUNCTION();

		m_impl->commandBuffer.setViewport(
			0,
			vk::Viewport {
				.x = static_cast<float>(a_offset.x),
				.y = static_cast<float>(a_offset.y),
				.width = static_cast<float>(a_size.x),
				.height = static_cast<float>(a_size.y),
				.minDepth = 0.f,
				.maxDepth = 1.f
			}
		);
	}

	void
	CommandBuffer::SetScissor(Vector2i a_offset, Vector2u a_size) const noexcept
	{
		OYL_PROFILE_FUNCTION();

		m_impl->commandBuffer.setScissor(
			0,
			vk::Rect2D {
				.offset = {
					.x = a_offset.x,
					.y = a_offset.y
				},
				.extent = {
					.width = a_size.x,
					.height = a_size.y
				}
			}
		);
	}

	void
	CommandBuffer::BindShader(const ShaderResource& a_shader) const noexcept
	{
		m_impl->commandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, a_shader.GetVkPipeline());
	}

	void
	CommandBuffer::BindVertexBuffer(const VertexBufferResource& a_vertexBuffer) const noexcept
	{
		const auto& vkCommandBuffer = m_impl->commandBuffer;

		const auto& vkVertexBuffer = a_vertexBuffer.GetVkBuffer();

		if (a_vertexBuffer.HasIndexData())
		{
			vkCommandBuffer.bindIndexBuffer(*vkVertexBuffer, 0, vk::IndexType::eUint16);
			vkCommandBuffer.bindVertexBuffers(0, *vkVertexBuffer, { a_vertexBuffer.GetVertexDataOffset() });
			vkCommandBuffer.drawIndexed(a_vertexBuffer.GetIndexCount(), 1, 0, 0, 0);
		} else
		{
			vkCommandBuffer.bindVertexBuffers(0, *vkVertexBuffer, { a_vertexBuffer.GetVertexDataOffset() });
			vkCommandBuffer.draw(a_vertexBuffer.GetVertexCount(), 1, 0, 0);
		}
	}

	void
	CommandBuffer::Impl::TransitionImageLayout(
		vk::Image a_image,
		vk::ImageLayout a_oldLayout,
		vk::ImageLayout a_new_layout,
		vk::AccessFlags2 a_srcAccessMask,
		vk::AccessFlags2 a_dstAccessMask,
		vk::PipelineStageFlags2 a_srcStageMask,
		vk::PipelineStageFlags2 a_dstStageMask
	) const noexcept
	{
		OYL_PROFILE_FUNCTION();

		vk::ImageMemoryBarrier2 barrier = {
			.srcStageMask = a_srcStageMask,
			.srcAccessMask = a_srcAccessMask,
			.dstStageMask = a_dstStageMask,
			.dstAccessMask = a_dstAccessMask,
			.oldLayout = a_oldLayout,
			.newLayout = a_new_layout,
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
		commandBuffer.pipelineBarrier2(dependency_info);
	}
}
