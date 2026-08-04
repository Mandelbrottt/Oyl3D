#pragma once

#include <Core/PImpl.h>
#include <Core/UniqueHandle.h>
#include <Core/Math/Vector.h>

#include "SwapChain.h"

namespace Oyl::Rendering
{
	class RenderTarget;
	class CommandPoolImpl;
	class SwapChainImpl;
	class ShaderImpl;
	class VertexBufferImpl;

	class CommandBufferImpl : public IUniqueHandle
	{
	protected:
		CommandBufferImpl() noexcept = default;

	public:
		struct CreateParams
		{
			const CommandPoolImpl& commandPool;
		};

		NO_MOVE(CommandBufferImpl);
		NO_COPY(CommandBufferImpl);

		virtual
		~CommandBufferImpl() noexcept = default;

		virtual
		void
		Begin() const noexcept = 0;

		virtual
		void
		BeginRendering(const RenderTarget& a_renderTarget) const noexcept = 0;

		virtual
		void
		End() const noexcept = 0;

		virtual
		void
		EndRendering() const noexcept = 0;

		virtual
		void
		SetViewport(Vector2i a_offset, Vector2u a_size) const noexcept = 0;

		virtual
		void
		SetScissor(Vector2i a_offset, Vector2u a_size) const noexcept = 0;

		virtual
		void
		BindShader(const ShaderImpl& a_shader) const noexcept = 0;

		virtual
		void
		BindVertexBuffer(const VertexBufferImpl& a_buffer) const noexcept = 0;

		virtual
		void
		DrawVertexBuffer(const VertexBufferImpl& a_buffer) const noexcept = 0;

	protected:
		virtual
		const CommandPoolImpl*
		GetCommandPool() const noexcept = 0;
	};

	using CommandBuffer = PImpl<CommandBufferImpl>;
}
