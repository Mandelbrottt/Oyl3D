#pragma once

#include "VulkanCommandPool.h"

#include "Rendering/CommandBuffer.h"

namespace vk::raii
{
	class CommandBuffer;
}

namespace Oyl::Rendering
{
	class SwapChainImpl;
	class ShaderImpl;
	class VertexBufferImpl;
}

namespace Oyl::Rendering::Vulkan
{
	class CommandPoolImpl;
	class DeviceImpl;
	class ShaderImpl;
	class SwapChainImpl;
	class VertexBufferImpl;

	class OYL_RENDERING_API CommandBufferImpl : public Rendering::CommandBufferImpl
	{
	public:
		CommandBufferImpl(nullptr_t);

		CommandBufferImpl(const DeviceImpl& a_device, const CreateParams& a_params) noexcept;

		CommandBufferImpl(CommandBufferImpl&& a_other) noexcept;
		CommandBufferImpl&
		operator =(CommandBufferImpl&& a_other) noexcept;

		virtual
		~CommandBufferImpl() noexcept;

		void
		Destroy() noexcept override;

		bool
		IsValid() const noexcept override;

		const CommandPoolImpl*
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
		BindShader(const Rendering::ShaderImpl& a_shader) const noexcept override;

		void
		BindShader(const ShaderImpl& a_shader) const noexcept;

		void
		BindVertexBuffer(const Rendering::VertexBufferImpl& a_vertexBuffer) const noexcept override;

		void
		BindVertexBuffer(const VertexBufferImpl& a_vertexBuffer) const noexcept;

		void
		DrawVertexBuffer(const Rendering::VertexBufferImpl& a_vertexBuffer) const noexcept override;

		void
		DrawVertexBuffer(const VertexBufferImpl& a_vertexBuffer) const noexcept;

	private:
		struct Impl;
		std::unique_ptr<Impl> m_impl;
	};

	using CommandBuffer = PImpl<CommandBufferImpl>;
}
