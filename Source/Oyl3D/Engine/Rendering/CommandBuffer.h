#pragma once

#include <Core/UniqueHandle.h>
#include <Core/Math/Vector.h>

#include "SwapChain.h"

namespace Oyl::Rendering
{
	class RenderTarget;
	class ICommandPool;
	class ISwapChain;
	class Shader;
	class VertexBuffer;

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
		BindShader(const Shader& a_shader) const noexcept = 0;

		virtual
		void
		BindVertexBuffer(const VertexBuffer& a_buffer) const noexcept = 0;

	protected:
		virtual
		const ICommandPool*
		GetCommandPool() const noexcept = 0;
	};
}
