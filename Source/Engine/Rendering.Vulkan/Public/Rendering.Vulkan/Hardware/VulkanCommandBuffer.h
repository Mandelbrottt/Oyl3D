#pragma once

#include <vulkan/vulkan_raii.hpp>

#include "VulkanCommandPool.h"
#include "VulkanImage.h"

#include "Rendering/Hardware/RdoCommandBuffer.h"

namespace Oyl
{
	class IRdoShader;
	class IRdoVertexBuffer;

	class VulkanDevice;
	class VulkanShader;
	class VulkanVertexBuffer;

	class RENDERING_VULKAN_EXPORT VulkanCommandBuffer : public IRdoCommandBuffer
	{
	public:
		VulkanCommandBuffer(nullptr_t) {}

		VulkanCommandBuffer(const VulkanDevice& a_device, const RdoCommandBufferCreateParams& a_params) noexcept;

		DEFAULT_MOVE(VulkanCommandBuffer);

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
		BeginRendering(const RenderTarget& a_renderTarget, const BeginRenderingParams& a_params = {}) const noexcept override;

		void
		EndRendering() const noexcept override;

		void
		End() const noexcept override;

		void
		SetViewport(Vector2i a_offset, Vector2u a_size) const noexcept override;

		void
		SetScissor(Vector2i a_offset, Vector2u a_size) const noexcept override;

		void
		BindShader(const IRdoShader& a_shader) const noexcept override;

		void
		BindShader(const VulkanShader& a_shader) const noexcept;

		void
		BindShaderBindingSet(
			const IRdoShader& a_shader,
			const IRdoShaderBindingSet& a_bindingSet
		) const noexcept override;

		void
		BindVertexBuffer(const IRdoVertexBuffer& a_vertexBuffer) const noexcept override;

		void
		BindVertexBuffer(const VulkanVertexBuffer& a_vertexBuffer) const noexcept;

		void
		DrawVertexBuffer(const IRdoVertexBuffer& a_vertexBuffer) const noexcept override;

		void
		DrawVertexBuffer(const VulkanVertexBuffer& a_vertexBuffer) const noexcept;

		void
		TransitionImageLayout(IRdoImage& a_image, ImageLayout a_newLayout) const noexcept override;

		void
		TransitionImageLayout(VulkanImage& a_image, ImageLayout a_newLayout) const noexcept;

		void
		TransitionImageLayout(
			RdoImageId a_imageHandle,
			ImageLayout a_oldLayout,
			ImageLayout a_newLayout
		) const noexcept override;

		void
		TransitionImageLayout(
			VulkanImageId a_imageHandle,
			ImageLayout a_oldLayout,
			ImageLayout a_newLayout
		) const noexcept;

		void
		BlitImage(
			const IRdoImage& a_srcImage,
			const IRdoImage& a_dstImage,
			ImageFilter a_filter
		) const noexcept override;

		void
		BlitImage(
			RdoImageId a_srcImageId,
			ImageLayout a_srcLayout,
			Rect2D a_srcRect,
			RdoImageId a_dstImageId,
			ImageLayout a_dstImageLayout,
			Rect2D a_dstRect,
			ImageFilter a_filter
		) const noexcept override;

		void
		BlitImage(
			const VulkanImage& a_srcImage,
			const VulkanImage& a_dstImage,
			ImageFilter a_filter
		) const noexcept;

		void
		BlitImage(
			VulkanImageId a_srcImageId,
			ImageLayout a_srcLayout,
			Rect2D a_srcRect,
			VulkanImageId a_dstImageId,
			ImageLayout a_dstImageLayout,
			Rect2D a_dstRect,
			ImageFilter a_filter
		) const noexcept;

	private:
		const VulkanCommandPool* m_commandPool = nullptr;
		vk::raii::CommandBuffer m_vkCommandBuffer = nullptr;
	};
}
