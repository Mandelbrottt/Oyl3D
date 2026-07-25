#pragma once

#include <Core/UniqueHandle.h>
#include <Core/Math/Vector.h>

#include "CommandPool.h"
#include "SwapChain.h"

#include "Rendering/Shader.h"
#include "Rendering/VertexBuffer.h"

namespace Oyl::Rendering
{
	class ICommandBuffer : public IUniqueHandle
	{
	protected:
		ICommandBuffer() noexcept = default;

		DEFAULT_MOVE(ICommandBuffer);

	public:
		NO_COPY(ICommandBuffer);

		virtual
		~ICommandBuffer() noexcept = default;

		virtual
		void
		Begin() const noexcept = 0;

		virtual
		void
		BeginRendering(const ISwapChain& a_swapChain) const noexcept = 0;

		virtual
		void
		End() const noexcept = 0;

		virtual
		void
		EndRendering(const ISwapChain& a_swapChain) const noexcept = 0;

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
		const ICommandPool*
		GetCommandPool() const noexcept = 0;
	};
}
