#pragma once

#include "VulkanCommandPool.h"
#include "VulkanImage.h"

#include "Rendering/CommandBuffer.h"

namespace vk::raii
{
	class CommandBuffer;
}

namespace Oyl::Rendering
{
	class VulkanCommandPool;
	class VulkanDevice;
	class VulkanShader;
	class VulkanSwapChain;
	class VulkanVertexBuffer;

	class OYL_RENDERING_API VulkanCommandBuffer : public CommandBuffer
	{
	public:
		VulkanCommandBuffer(nullptr_t);

		VulkanCommandBuffer(const VulkanDevice& a_device, const CreateParams& a_params) noexcept;

		VulkanCommandBuffer(VulkanCommandBuffer&& a_other) noexcept;
		VulkanCommandBuffer&
		operator =(VulkanCommandBuffer&& a_other) noexcept;

		virtual
		~VulkanCommandBuffer() noexcept;

		void
		Destroy() noexcept override;

		bool
		IsValid() const noexcept override;

		const VulkanCommandPool*
		GetCommandPool() const noexcept override;

		const vk::raii::CommandBuffer&
		GetVkCommandBuffer() const noexcept;

		void
		Begin() const noexcept override;

		void
		BeginRendering(const RenderTarget& a_renderTarget) const noexcept override;

		void
		EndRendering() const noexcept override;

		void
		End() const noexcept override;

		void
		SetViewport(Vector2i a_offset, Vector2u a_size) const noexcept override;

		void
		SetScissor(Vector2i a_offset, Vector2u a_size) const noexcept override;

		void
		BindShader(const Shader& a_shader) const noexcept override;

		void
		BindShader(const VulkanShader& a_shader) const noexcept;

		void
		BindVertexBuffer(const VertexBuffer& a_vertexBuffer) const noexcept override;

		void
		BindVertexBuffer(const VulkanVertexBuffer& a_vertexBuffer) const noexcept;

		void
		DrawVertexBuffer(const VertexBuffer& a_vertexBuffer) const noexcept override;

		void
		DrawVertexBuffer(const VulkanVertexBuffer& a_vertexBuffer) const noexcept;

	protected:
		void
		TransitionImageLayout(Image& a_image, ImageLayout a_newLayout) const noexcept override
		{
			auto& vulkanImageImpl = dynamic_cast<VulkanImage&>(a_image);
			TransitionImageLayout(vulkanImageImpl, a_newLayout);
		}

	public:
		void
		TransitionImageLayout(VulkanImage& a_image, ImageLayout a_newLayout) const noexcept;

	protected:
		void
		TransitionImageLayout(
			ImageId a_imageHandle,
			ImageLayout a_oldLayout,
			ImageLayout a_newLayout
		) const noexcept override
		{
			TransitionImageLayout(static_cast<VulkanImageId>(a_imageHandle), a_oldLayout, a_newLayout);
		}

	public:
		void
		TransitionImageLayout(
			VulkanImageId a_imageHandle,
			ImageLayout a_oldLayout,
			ImageLayout a_newLayout
		) const noexcept;

	private:
		struct Impl;
		std::unique_ptr<Impl> m_impl;
	};

	using VulkanCommandBufferHandle = PImpl<VulkanCommandBuffer>;
}
