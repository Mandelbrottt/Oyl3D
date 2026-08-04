#include "VulkanCommandBuffer.h"

#include "VulkanDevice.h"
#include "VulkanImage.h"
#include "VulkanShader.h"
#include "VulkanSwapChain.h"
#include "VulkanVertexBuffer.h"

#include "Rendering/RenderTarget.h"

namespace Oyl::Rendering::Vulkan
{
	struct CommandBufferImpl::Impl
	{
		const CommandPoolImpl* commandPool;

		vk::raii::CommandBuffer vkCommandBuffer = nullptr;
	};

	CommandBufferImpl::CommandBufferImpl(nullptr_t)
		: m_impl(nullptr) {}

	CommandBufferImpl::CommandBufferImpl(const DeviceImpl& a_device, const CreateParams& a_params) noexcept
		: m_impl(std::make_unique<Impl>())
	{
		OYL_PROFILE_FUNCTION();

		m_impl->commandPool = dynamic_cast<const CommandPoolImpl*>(&a_params.commandPool);

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

	CommandBufferImpl::CommandBufferImpl(CommandBufferImpl&& a_other) noexcept
	{
		*this = std::move(a_other);
	}

	CommandBufferImpl&
	CommandBufferImpl::operator=(CommandBufferImpl&& a_other) noexcept
	{
		if (this != &a_other)
		{
			m_impl = std::move(a_other.m_impl);
		}
		return *this;
	}

	CommandBufferImpl::~CommandBufferImpl() noexcept
	{
		CommandBufferImpl::Destroy();
	}

	void
	CommandBufferImpl::Destroy() noexcept
	{
		m_impl->vkCommandBuffer.clear();
	}

	bool
	CommandBufferImpl::IsValid() const noexcept
	{
		return m_impl
		       && *m_impl->vkCommandBuffer;
	}

	const CommandPoolImpl*
	CommandBufferImpl::GetCommandPool() const noexcept
	{
		return m_impl->commandPool;
	}

	const vk::raii::CommandBuffer&
	CommandBufferImpl::GetVkCommandBuffer() const noexcept
	{
		return m_impl->vkCommandBuffer;
	}

	void
	CommandBufferImpl::Begin() const noexcept
	{
		OYL_PROFILE_FUNCTION();

		m_impl->vkCommandBuffer.begin({});
	}

	void
	CommandBufferImpl::BeginRendering(const RenderTarget& a_renderTarget) const noexcept
	{
		OYL_PROFILE_FUNCTION();

		Vector2u size;

		std::vector<vk::RenderingAttachmentInfo> colorAttachmentInfo;
		for (const Rendering::ImageImpl* image : a_renderTarget.GetColorAttachments())
		{
			auto* vulkanImage = static_cast<const ImageImpl*>(image);
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
			auto* vulkanImage = static_cast<const ImageImpl*>(image);
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
	CommandBufferImpl::End() const noexcept
	{
		OYL_PROFILE_FUNCTION();

		m_impl->vkCommandBuffer.end();
	}

	void
	CommandBufferImpl::EndRendering() const noexcept
	{
		OYL_PROFILE_FUNCTION();

		m_impl->vkCommandBuffer.endRendering();
	}

	void
	CommandBufferImpl::SetViewport(Vector2i a_offset, Vector2u a_size) const noexcept
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
	CommandBufferImpl::SetScissor(Vector2i a_offset, Vector2u a_size) const noexcept
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
	CommandBufferImpl::BindShader(const Rendering::ShaderImpl& a_shader) const noexcept
	{
		return BindShader(static_cast<const ShaderImpl&>(a_shader));
	}

	void
	CommandBufferImpl::BindShader(const ShaderImpl& a_shader) const noexcept
	{
		OYL_PROFILE_FUNCTION();

		m_impl->vkCommandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, a_shader.GetVkPipeline());
	}

	void
	CommandBufferImpl::BindVertexBuffer(const Rendering::VertexBufferImpl& a_vertexBuffer) const noexcept
	{
		return BindVertexBuffer(static_cast<const VertexBufferImpl&>(a_vertexBuffer));
	}

	void
	CommandBufferImpl::BindVertexBuffer(const VertexBufferImpl& a_vertexBuffer) const noexcept
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
	CommandBufferImpl::DrawVertexBuffer(const Rendering::VertexBufferImpl& a_vertexBuffer) const noexcept
	{
		DrawVertexBuffer(static_cast<const VertexBufferImpl&>(a_vertexBuffer));
	}

	void
	CommandBufferImpl::DrawVertexBuffer(const VertexBufferImpl& a_vertexBuffer) const noexcept
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
}
