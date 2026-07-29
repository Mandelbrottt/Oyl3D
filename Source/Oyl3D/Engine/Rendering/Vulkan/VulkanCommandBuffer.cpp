#include "VulkanCommandBuffer.h"

#include "VulkanDevice.h"
#include "VulkanImage.h"
#include "VulkanShader.h"
#include "VulkanSwapChain.h"
#include "VulkanVertexBuffer.h"

#include "Rendering/RenderTarget.h"

namespace Oyl::Rendering::Vulkan
{
	struct CommandBuffer::Impl
	{
		const CommandPool* commandPool;

		vk::raii::CommandBuffer commandBuffer = nullptr;
	};

	CommandBuffer::CommandBuffer() noexcept
		: m_impl(nullptr) {}

	CommandBuffer::CommandBuffer(const Device& a_device, const CreateParams& a_params) noexcept
		: m_impl(std::make_unique<Impl>())
	{
		OYL_PROFILE_FUNCTION();

		m_impl->commandPool = &a_params.commandPool;

		const auto& vkCommandPool = a_params.commandPool.GetVkCommandPool();
		const auto& vkDevice = a_device.GetVkDevice();

		vk::CommandBufferAllocateInfo allocInfo {
			.commandPool = vkCommandPool,
			.level = vk::CommandBufferLevel::ePrimary,
			.commandBufferCount = 1
		};

		// We can only create command buffers through CommandBuffers vector constructor
		m_impl->commandBuffer = std::move(vk::raii::CommandBuffers(vkDevice, allocInfo).front());
	}

	CommandBuffer::CommandBuffer(CommandBuffer&& a_other) noexcept
	{
		*this = std::move(a_other);
	}

	CommandBuffer&
	CommandBuffer::operator=(CommandBuffer&& a_other) noexcept
	{
		if (this != &a_other)
		{
			m_impl = std::move(a_other.m_impl);
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
	}

	bool
	CommandBuffer::IsValid() const noexcept
	{
		return m_impl
		       && *m_impl->commandBuffer;
	}

	const CommandPool*
	CommandBuffer::GetCommandPool() const noexcept
	{
		return m_impl->commandPool;
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
	CommandBuffer::BeginRendering(const RenderTarget& a_renderTarget) const noexcept
	{
		OYL_PROFILE_FUNCTION();

		Vector2u size;

		std::vector<vk::RenderingAttachmentInfo> colorAttachmentInfo;
		for (const Rendering::Image* image : a_renderTarget.GetColorAttachments())
		{
			auto* vulkanImage = static_cast<const Image*>(image);
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
			auto* vulkanImage = static_cast<const Image*>(image);
			depthAttachmentInfo.setImageView(vulkanImage->GetVkImageView())
			                   .setImageLayout(vk::ImageLayout::eDepthStencilAttachmentOptimal)
			                   .setLoadOp(vk::AttachmentLoadOp::eClear)
			                   .setStoreOp(vk::AttachmentStoreOp::eStore)
			                   .setClearValue(vk::ClearDepthStencilValue(1.0f, 0));

			renderingInfo.setPDepthAttachment(&depthAttachmentInfo);

			size = vulkanImage->GetSize();
		}

		renderingInfo.setRenderArea({ .offset = { 0, 0 }, .extent = { size.x, size.y } });

		m_impl->commandBuffer.beginRendering(renderingInfo);
	}

	void
	CommandBuffer::End() const noexcept
	{
		OYL_PROFILE_FUNCTION();

		m_impl->commandBuffer.end();
	}

	void
	CommandBuffer::EndRendering() const noexcept
	{
		OYL_PROFILE_FUNCTION();

		m_impl->commandBuffer.endRendering();
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
	CommandBuffer::BindShader(const Rendering::Shader& a_shader) const noexcept
	{
		return BindShader(static_cast<const Shader&>(a_shader));
	}

	void
	CommandBuffer::BindShader(const Shader& a_shader) const noexcept
	{
		OYL_PROFILE_FUNCTION();

		m_impl->commandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, a_shader.GetVkPipeline());
	}

	void
	CommandBuffer::BindVertexBuffer(const Rendering::VertexBuffer& a_vertexBuffer) const noexcept
	{
		return BindVertexBuffer(static_cast<const VertexBuffer&>(a_vertexBuffer));
	}

	void
	CommandBuffer::BindVertexBuffer(const VertexBuffer& a_vertexBuffer) const noexcept
	{
		OYL_PROFILE_FUNCTION();

		const auto& vkCommandBuffer = m_impl->commandBuffer;
		const auto& vkVertexBuffer = a_vertexBuffer.GetVkBuffer();

		if (a_vertexBuffer.GetIndexCount() != 0)
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
}
