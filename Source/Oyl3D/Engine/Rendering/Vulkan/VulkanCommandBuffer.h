#pragma once

#include "VulkanCommandPool.h"

#include "Rendering/CommandBuffer.h"

namespace vk::raii
{
	class CommandBuffer;
}

namespace Oyl::Rendering
{
	class ISwapChain;
	class Shader;
	class VertexBuffer;
}

namespace Oyl::Rendering::Vulkan
{
	class CommandPool;
	class Device;
	class Shader;
	class SwapChain;
	class VertexBuffer;

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
		BeginRendering(const ISwapChain& a_swapChain) const noexcept override;

		void
		BeginRendering(const SwapChain& a_swapChain) const noexcept;

		void
		EndRendering(const ISwapChain& a_swapChain) const noexcept override;

		void
		EndRendering(const SwapChain& a_swapChain) const noexcept;

		void
		End() const noexcept override;

		void
		SetViewport(Vector2i a_offset, Vector2u a_size) const noexcept override;

		void
		SetScissor(Vector2i a_offset, Vector2u a_size) const noexcept override;

		void
		BindShader(const Rendering::Shader& a_shader) const noexcept override;

		void
		BindShader(const Shader& a_shader) const noexcept;

		void
		BindVertexBuffer(const Rendering::VertexBuffer& a_vertexBuffer) const noexcept override;

		void
		BindVertexBuffer(const VertexBuffer& a_vertexBuffer) const noexcept;

	private:
		struct Impl;
		std::unique_ptr<Impl> m_impl;
	};
}
