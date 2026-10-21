#include "VulkanCommandBuffer.h"

#include "VulkanDevice.h"
#include "VulkanEnums.h"
#include "VulkanImage.h"
#include "VulkanShader.h"
#include "VulkanSwapChain.h"
#include "VulkanVertexBuffer.h"

#include "Rendering/RenderTarget.h"

namespace Oyl::Rendering
{
	struct VulkanCommandBuffer::Impl
	{
		const VulkanCommandPool* commandPool;

		vk::raii::CommandBuffer vkCommandBuffer = nullptr;
	};

	VulkanCommandBuffer::VulkanCommandBuffer(nullptr_t)
		: m_impl(nullptr) {}

	VulkanCommandBuffer::VulkanCommandBuffer(const VulkanDevice& a_device, const CreateParams& a_params) noexcept
		: m_impl(std::make_unique<Impl>())
	{
		OYL_PROFILE_FUNCTION();

		m_impl->commandPool = dynamic_cast<const VulkanCommandPool*>(&a_params.commandPool);

		const auto& vkCommandPool = m_impl->commandPool->GetVkCommandPool();
		const auto& vkDevice = a_device.GetVkDevice();

		vk::CommandBufferAllocateInfo allocInfo {
			.commandPool = vkCommandPool,
			.level = vk::CommandBufferLevel::ePrimary,
			.commandBufferCount = 1
		};

		// We can only create command buffers through CommandBuffers vector constructor
		m_impl->vkCommandBuffer = std::move(vk::raii::CommandBuffers(vkDevice, allocInfo).front());
	}

	VulkanCommandBuffer::VulkanCommandBuffer(VulkanCommandBuffer&& a_other) noexcept
	{
		*this = std::move(a_other);
	}

	VulkanCommandBuffer&
	VulkanCommandBuffer::operator=(VulkanCommandBuffer&& a_other) noexcept
	{
		if (this != &a_other)
		{
			m_impl = std::move(a_other.m_impl);
		}
		return *this;
	}

	VulkanCommandBuffer::~VulkanCommandBuffer() noexcept
	{
		VulkanCommandBuffer::Destroy();
	}

	void
	VulkanCommandBuffer::Destroy() noexcept
	{
		m_impl->vkCommandBuffer.clear();
	}

	bool
	VulkanCommandBuffer::IsValid() const noexcept
	{
		return m_impl
		       && *m_impl->vkCommandBuffer;
	}

	const VulkanCommandPool*
	VulkanCommandBuffer::GetCommandPool() const noexcept
	{
		return m_impl->commandPool;
	}

	const vk::raii::CommandBuffer&
	VulkanCommandBuffer::GetVkCommandBuffer() const noexcept
	{
		return m_impl->vkCommandBuffer;
	}

	void
	VulkanCommandBuffer::Begin() const noexcept
	{
		OYL_PROFILE_FUNCTION();

		m_impl->vkCommandBuffer.begin({});
	}

	void
	VulkanCommandBuffer::BeginRendering(const RenderTarget& a_renderTarget) const noexcept
	{
		OYL_PROFILE_FUNCTION();

		Vector2u size;

		std::vector<vk::RenderingAttachmentInfo> colorAttachmentInfo;
		for (const Rendering::Image* image : a_renderTarget.GetColorAttachments())
		{
			auto* vulkanImage = static_cast<const VulkanImage*>(image);
			colorAttachmentInfo.emplace_back(
				vk::RenderingAttachmentInfo {
					.imageView = vulkanImage->GetVkImageView(),
					.imageLayout = vk::ImageLayout::eColorAttachmentOptimal,
					.loadOp = vk::AttachmentLoadOp::eClear,
					.storeOp = vk::AttachmentStoreOp::eStore,
					.clearValue = vk::ClearColorValue(0.0f, 0.0f, 0.0f, 1.0f)
				}
			);

			size = vulkanImage->GetSize();
		}

		vk::RenderingInfo renderingInfo;
		renderingInfo.setLayerCount(1)
		             .setColorAttachmentCount((uint32) colorAttachmentInfo.size())
		             .setPColorAttachments(colorAttachmentInfo.data());

		vk::RenderingAttachmentInfo depthAttachmentInfo;
		if (auto* image = a_renderTarget.GetDepthAttachment())
		{
			auto* vulkanImage = static_cast<const VulkanImage*>(image);
			depthAttachmentInfo.setImageView(vulkanImage->GetVkImageView())
			                   .setImageLayout(vk::ImageLayout::eDepthStencilAttachmentOptimal)
			                   .setLoadOp(vk::AttachmentLoadOp::eClear)
			                   .setStoreOp(vk::AttachmentStoreOp::eStore)
			                   .setClearValue(vk::ClearDepthStencilValue(1.0f, 0));

			renderingInfo.setPDepthAttachment(&depthAttachmentInfo);

			size = vulkanImage->GetSize();
		}

		renderingInfo.setRenderArea({ .offset = { 0, 0 }, .extent = { size.x, size.y } });

		m_impl->vkCommandBuffer.beginRendering(renderingInfo);
	}

	void
	VulkanCommandBuffer::End() const noexcept
	{
		OYL_PROFILE_FUNCTION();

		m_impl->vkCommandBuffer.end();
	}

	void
	VulkanCommandBuffer::EndRendering() const noexcept
	{
		OYL_PROFILE_FUNCTION();

		m_impl->vkCommandBuffer.endRendering();
	}

	void
	VulkanCommandBuffer::SetViewport(Vector2i a_offset, Vector2u a_size) const noexcept
	{
		OYL_PROFILE_FUNCTION();

		m_impl->vkCommandBuffer.setViewport(
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
	VulkanCommandBuffer::SetScissor(Vector2i a_offset, Vector2u a_size) const noexcept
	{
		OYL_PROFILE_FUNCTION();

		m_impl->vkCommandBuffer.setScissor(
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
	VulkanCommandBuffer::BindShader(const Rendering::Shader& a_shader) const noexcept
	{
		return BindShader(static_cast<const VulkanShader&>(a_shader));
	}

	void
	VulkanCommandBuffer::BindShader(const VulkanShader& a_shader) const noexcept
	{
		OYL_PROFILE_FUNCTION();

		m_impl->vkCommandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, a_shader.GetVkPipeline());
	}

	void
	VulkanCommandBuffer::BindVertexBuffer(const Rendering::VertexBuffer& a_vertexBuffer) const noexcept
	{
		return BindVertexBuffer(static_cast<const VulkanVertexBuffer&>(a_vertexBuffer));
	}

	void
	VulkanCommandBuffer::BindVertexBuffer(const VulkanVertexBuffer& a_vertexBuffer) const noexcept
	{
		OYL_PROFILE_FUNCTION();

		const auto& vkCommandBuffer = m_impl->vkCommandBuffer;
		const auto& vkVertexBuffer = a_vertexBuffer.GetVkBuffer();

		if (a_vertexBuffer.GetIndexCount() != 0)
		{
			vkCommandBuffer.bindIndexBuffer(*vkVertexBuffer, 0, vk::IndexType::eUint16);
			vkCommandBuffer.bindVertexBuffers(0, *vkVertexBuffer, { a_vertexBuffer.GetVertexDataOffset() });
		} else
		{
			vkCommandBuffer.bindVertexBuffers(0, *vkVertexBuffer, { a_vertexBuffer.GetVertexDataOffset() });
		}
	}

	void
	VulkanCommandBuffer::DrawVertexBuffer(const Rendering::VertexBuffer& a_vertexBuffer) const noexcept
	{
		DrawVertexBuffer(static_cast<const VulkanVertexBuffer&>(a_vertexBuffer));
	}

	void
	VulkanCommandBuffer::DrawVertexBuffer(const VulkanVertexBuffer& a_vertexBuffer) const noexcept
	{
		OYL_PROFILE_FUNCTION();

		const auto& vkCommandBuffer = m_impl->vkCommandBuffer;
		const auto& vkVertexBuffer = a_vertexBuffer.GetVkBuffer();

		if (a_vertexBuffer.GetIndexCount() != 0)
		{
			vkCommandBuffer.bindIndexBuffer(*vkVertexBuffer, 0, vk::IndexType::eUint16);
			vkCommandBuffer.bindVertexBuffers(0, *vkVertexBuffer, { a_vertexBuffer.GetVertexDataOffset() });
			vkCommandBuffer.drawIndexed(a_vertexBuffer.GetIndexCount(), 1, 0, 0, 0);
		} else
		{
			vkCommandBuffer.draw(a_vertexBuffer.GetVertexCount(), 1, 0, 0);
			vkCommandBuffer.bindVertexBuffers(0, *vkVertexBuffer, { a_vertexBuffer.GetVertexDataOffset() });
		}
	}

	static
	void
	SetVkStageAndAccessMasksForLayouts(
		vk::ImageMemoryBarrier2& a_barrier,
		vk::ImageLayout a_oldLayout,
		vk::ImageLayout a_newLayout
	);

	void
	VulkanCommandBuffer::TransitionImageLayout(VulkanImage& a_image, ImageLayout a_newLayout) const noexcept
	{
		auto oldLayout = a_image.GetLayout();
		TransitionImageLayout(a_image.GetId(), oldLayout, a_newLayout);
		a_image.SetLayout(a_newLayout);
	}

	void
	VulkanCommandBuffer::TransitionImageLayout(
		VulkanImageId a_imageHandle,
		ImageLayout a_oldLayout,
		ImageLayout a_newLayout
	) const noexcept
	{
		OYL_PROFILE_FUNCTION();

		auto vkOldLayout = ToVkEnum(a_oldLayout);
		auto vkNewLayout = ToVkEnum(a_newLayout);

		vk::ImageMemoryBarrier2 barrier = {
			.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
			.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
			.image = a_imageHandle,
			.subresourceRange = {
				.aspectMask = vk::ImageAspectFlagBits::eColor,
				.baseMipLevel = 0,
				.levelCount = 1,
				.baseArrayLayer = 0,
				.layerCount = 1
			}
		};
		SetVkStageAndAccessMasksForLayouts(barrier, vkOldLayout, vkNewLayout);

		vk::DependencyInfo dependency_info = {
			.dependencyFlags = {},
			.imageMemoryBarrierCount = 1,
			.pImageMemoryBarriers = &barrier
		};
		const auto& vkCommandBuffer = GetVkCommandBuffer();
		vkCommandBuffer.pipelineBarrier2(dependency_info);
	}

	void
	VulkanCommandBuffer::BlitImage(
		VulkanImageId a_srcImageId,
		ImageLayout a_srcLayout,
		Rect2D a_srcRect,
		VulkanImageId a_dstImageId,
		ImageLayout a_dstImageLayout,
		Rect2D a_dstRect,
		ImageFilter a_filter
	) const noexcept
	{
		OYL_PROFILE_FUNCTION();

		m_impl->vkCommandBuffer.blitImage(
			a_srcImageId,
			ToVkEnum(a_srcLayout),
			a_dstImageId,
			ToVkEnum(a_dstImageLayout),
			vk::ImageBlit {
				.srcSubresource = { .aspectMask = vk::ImageAspectFlagBits::eColor, .mipLevel = 0, .baseArrayLayer = 0, .layerCount = 1 },
				.srcOffsets = std::array { vk::Offset3D { a_srcRect.x, a_srcRect.y, 0 }, vk::Offset3D { (int) a_srcRect.width, (int) a_srcRect.height, 1 } },
				.dstSubresource = { .aspectMask = vk::ImageAspectFlagBits::eColor, .mipLevel = 0, .baseArrayLayer = 0, .layerCount = 1 },
				.dstOffsets = std::array { vk::Offset3D { a_dstRect.x, a_dstRect.y, 0 }, vk::Offset3D { (int) a_dstRect.width, (int) a_dstRect.height, 1 } }
			},
			ToVkEnum(a_filter)
		);
	}

	void
	SetVkStageAndAccessMasksForLayouts(
		vk::ImageMemoryBarrier2& a_barrier,
		vk::ImageLayout a_oldLayout,
		vk::ImageLayout a_newLayout
	)
	{
		a_barrier.oldLayout = a_oldLayout;
		a_barrier.newLayout = a_newLayout;

		if (a_oldLayout == vk::ImageLayout::eUndefined)
		{
			if (a_newLayout == vk::ImageLayout::eColorAttachmentOptimal)
			{
				a_barrier.srcStageMask = vk::PipelineStageFlagBits2::eColorAttachmentOutput;
				a_barrier.srcAccessMask = vk::AccessFlagBits2::eNone;
				a_barrier.dstStageMask = vk::PipelineStageFlagBits2::eColorAttachmentOutput;
				a_barrier.dstAccessMask = vk::AccessFlagBits2::eColorAttachmentWrite;
			} else if (a_newLayout == vk::ImageLayout::eTransferDstOptimal)
			{
				a_barrier.srcStageMask = vk::PipelineStageFlagBits2::eTopOfPipe | vk::PipelineStageFlagBits2::eTransfer;
				a_barrier.srcAccessMask = vk::AccessFlagBits2::eTransferWrite;
				a_barrier.dstStageMask = vk::PipelineStageFlagBits2::eTransfer | vk::PipelineStageFlagBits2::eBlit;
				a_barrier.dstAccessMask = vk::AccessFlagBits2::eTransferWrite;
			}
		}

		if (a_oldLayout == vk::ImageLayout::eTransferDstOptimal)
		{
			if (a_newLayout == vk::ImageLayout::eColorAttachmentOptimal)
			{
				a_barrier.srcStageMask = vk::PipelineStageFlagBits2::eTransfer;
				a_barrier.srcAccessMask = vk::AccessFlagBits2::eTransferWrite;
				a_barrier.dstStageMask = vk::PipelineStageFlagBits2::eColorAttachmentOutput;
				a_barrier.dstAccessMask = vk::AccessFlagBits2::eColorAttachmentWrite;
			} else if (a_newLayout == vk::ImageLayout::eShaderReadOnlyOptimal)
			{
				a_barrier.srcStageMask = vk::PipelineStageFlagBits2::eTransfer;
				a_barrier.srcAccessMask = vk::AccessFlagBits2::eTransferWrite;
				a_barrier.dstStageMask = vk::PipelineStageFlagBits2::eFragmentShader;
				a_barrier.dstAccessMask = vk::AccessFlagBits2::eShaderRead;
			} else if (a_newLayout == vk::ImageLayout::ePresentSrcKHR)
			{
				a_barrier.srcStageMask = vk::PipelineStageFlagBits2::eBlit;
				a_barrier.srcAccessMask = vk::AccessFlagBits2::eNone;
				a_barrier.dstStageMask = vk::PipelineStageFlagBits2::eBlit;
				a_barrier.dstAccessMask = vk::AccessFlagBits2::eTransferWrite;
			}
		}

		if (a_oldLayout == vk::ImageLayout::eColorAttachmentOptimal)
		{
			if (a_newLayout == vk::ImageLayout::eTransferSrcOptimal)
			{
				a_barrier.srcStageMask = vk::PipelineStageFlagBits2::eColorAttachmentOutput;
				a_barrier.srcAccessMask = vk::AccessFlagBits2::eColorAttachmentWrite;
				a_barrier.dstStageMask = vk::PipelineStageFlagBits2::eBottomOfPipe;
				a_barrier.dstAccessMask = vk::AccessFlagBits2::eNone;
			}
		}

		if (a_oldLayout != vk::ImageLayout::eUndefined && (!a_barrier.srcStageMask || a_barrier.dstStageMask))
			SetVkStageAndAccessMasksForLayouts(a_barrier, vk::ImageLayout::eUndefined, a_newLayout);

		OYL_ASSERT(
			a_barrier.srcStageMask
			&& a_barrier.dstStageMask
		);
	}
}
