#pragma once

#include <vulkan/vulkan_raii.hpp>

#include "VulkanCommandPool.h"
#include "VulkanShader.h"
#include "VulkanSwapChain.h"
#include "VulkanVertexBuffer.h"

#include "Rendering/CommandBuffer.h"

namespace Oyl::Rendering::Vulkan
{
	class OYL_RENDERING_API CommandBuffer : public Rendering::CommandBuffer
	{
	public:
		CommandBuffer();

		struct CreateParams
		{
			const Device& device;
			const CommandPool& commandPool;
		};

		explicit
		CommandBuffer(const CreateParams& a_params);

		CommandBuffer(CommandBuffer&& a_other) noexcept;
		CommandBuffer&
		operator =(CommandBuffer&& a_other) noexcept;

		virtual
		~CommandBuffer();

		void
		Destroy() override;

		bool
		IsValid() const override;

		const CommandPool*
		GetCommandPool() const override;

		const vk::raii::CommandBuffer&
		GetVkCommandBuffer() const;

		void
		Begin() override;

		void
		BeginRendering(const Rendering::SwapChain& a_swapChain) override
		{
			BeginRendering(static_cast<const SwapChain&>(a_swapChain));
		}

		void
		BeginRendering(const SwapChain& a_swapChain);

		void
		EndRendering(const Rendering::SwapChain& a_swapChain) override
		{
			EndRendering(static_cast<const SwapChain&>(a_swapChain));
		}

		void
		EndRendering(const SwapChain& a_swapChain);

		void
		End() override;

		void
		SetViewport(Vector2i a_offset, Vector2u a_size) override;

		void
		SetScissor(Vector2i a_offset, Vector2u a_size) override;

		void
		BindShader(const Rendering::ShaderResource& a_shader) override
		{
			return BindShader(static_cast<const ShaderResource&>(a_shader));
		}

		void
		BindShader(const ShaderResource& a_shader);

		void
		BindVertexBuffer(const Rendering::VertexBufferResource& a_vertexBuffer) override
		{
			return BindVertexBuffer(static_cast<const VertexBufferResource&>(a_vertexBuffer));
		}

		void
		BindVertexBuffer(const VertexBufferResource& a_vertexBuffer);

	private:
		struct Impl;
		std::unique_ptr<Impl> m_impl;
	};
}
