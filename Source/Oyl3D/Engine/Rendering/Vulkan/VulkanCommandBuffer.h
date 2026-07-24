#pragma once

#include "Rendering/CommandBuffer.h"
#include "Rendering/Vulkan/VulkanCommandPool.h"

namespace vk::raii
{
	class CommandBuffer;
}

namespace Oyl::Rendering::Vulkan
{
	class Device;
	class CommandPool;
	class SwapChain;

	class OYL_RENDERING_API CommandBuffer : Rendering::CommandBuffer
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
		BeginRendering(const Rendering::SwapChain& a_swapChain) override;

		void
		EndRendering(const Rendering::SwapChain& a_swapChain) override;

		void
		End() override;

		void
		SetViewport(Vector2i a_offset, Vector2u a_size) override;

		void
		SetScissor(Vector2i a_offset, Vector2u a_size) override;

	private:
		struct Impl;
		std::unique_ptr<Impl> m_impl;
	};
}
