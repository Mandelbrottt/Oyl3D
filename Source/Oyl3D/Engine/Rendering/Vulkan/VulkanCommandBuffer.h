#pragma once

#include <vulkan/vulkan_raii.hpp>

#include "VulkanCommandPool.h"
#include "VulkanShader.h"
#include "VulkanSwapChain.h"
#include "VulkanVertexBuffer.h"

#include "Rendering/CommandBuffer.h"

namespace Oyl::Rendering::Vulkan
{
	class OYL_RENDERING_API CommandBuffer : public ICommandBuffer
	{
	public:
		CommandBuffer() noexcept;

		struct CreateParams
		{
			const Device& device;
			const CommandPool& commandPool;
		};

		explicit
		CommandBuffer(const CreateParams& a_params) noexcept;

		CommandBuffer(CommandBuffer&& a_other) noexcept;
		CommandBuffer&
		operator =(CommandBuffer&& a_other) noexcept;

		virtual
		~CommandBuffer() noexcept;

		void
		Destroy() noexcept override;

		bool
		IsValid() const noexcept override;

		const CommandPool*
		GetCommandPool() const noexcept override;

		const vk::raii::CommandBuffer&
		GetVkCommandBuffer() const noexcept;

		void
		Begin() const noexcept override;

		void
		BeginRendering(const ISwapChain& a_swapChain) const noexcept override
		{
			BeginRendering(static_cast<const SwapChain&>(a_swapChain));
		}

		void
		BeginRendering(const SwapChain& a_swapChain) const noexcept;

		void
		EndRendering(const ISwapChain& a_swapChain) const noexcept override
		{
			EndRendering(static_cast<const SwapChain&>(a_swapChain));
		}

		void
		EndRendering(const SwapChain& a_swapChain) const noexcept;

		void
		End() const noexcept override;

		void
		SetViewport(Vector2i a_offset, Vector2u a_size) const noexcept override;

		void
		SetScissor(Vector2i a_offset, Vector2u a_size) const noexcept override;

		void
		BindShader(const Rendering::ShaderResource& a_shader) const noexcept override
		{
			return BindShader(static_cast<const ShaderResource&>(a_shader));
		}

		void
		BindShader(const ShaderResource& a_shader) const noexcept;

		void
		BindVertexBuffer(const Rendering::VertexBufferResource& a_vertexBuffer) const noexcept override
		{
			return BindVertexBuffer(static_cast<const VertexBufferResource&>(a_vertexBuffer));
		}

		void
		BindVertexBuffer(const VertexBufferResource& a_vertexBuffer) const noexcept;

	private:
		struct Impl;
		std::unique_ptr<Impl> m_impl;
	};
}
