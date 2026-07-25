#pragma once

#include "CommandPool.h"
#include "DeviceObject.h"
#include "SwapChain.h"

#include "Core/Math/Vector.h"

#include "Rendering/Shader.h"
#include "Rendering/VertexBuffer.h"

namespace Oyl::Rendering
{
	class OYL_RENDERING_API CommandBuffer : public Internal::IDeviceObject
	{
	protected:
		CommandBuffer() noexcept = default;

		explicit
		CommandBuffer(const CommandPool* a_commandPool) noexcept;

		CommandBuffer(CommandBuffer&& a_other) noexcept = default;
		CommandBuffer&
		operator =(CommandBuffer&& a_other) noexcept = default;

	public:
		CommandBuffer(const CommandBuffer& a_other) noexcept = delete;
		CommandBuffer&
		operator =(const CommandBuffer& a_other) noexcept = delete;

		virtual
		~CommandBuffer() noexcept;

		void
		Destroy() noexcept override;

		bool
		IsValid() const noexcept override;

		virtual
		void
		Begin() const noexcept = 0;

		virtual
		void
		BeginRendering(const SwapChain& a_swapChain) const noexcept = 0;

		virtual
		void
		End() const noexcept = 0;

		virtual
		void
		EndRendering(const SwapChain& a_swapChain) const noexcept = 0;

		virtual
		void
		SetViewport(Vector2i a_offset, Vector2u a_size) const noexcept = 0;

		virtual
		void
		SetScissor(Vector2i a_offset, Vector2u a_size) const noexcept = 0;

		virtual
		void
		BindShader(const ShaderResource& a_shader) const noexcept = 0;

		virtual
		void
		BindVertexBuffer(const VertexBufferResource& a_shader) const noexcept = 0;

	protected:
		virtual
		const CommandPool*
		GetCommandPool() const noexcept
		{
			return m_commandPool;
		}

	private:
		const CommandPool* m_commandPool = nullptr;
	};
}
