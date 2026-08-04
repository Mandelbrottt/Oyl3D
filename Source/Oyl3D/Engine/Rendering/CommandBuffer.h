#pragma once

#include <Core/PImpl.h>
#include <Core/UniqueHandle.h>
#include <Core/Math/Vector.h>

#include "Image.h"
#include "SwapChain.h"

namespace Oyl::Rendering
{
	class RenderTarget;
	class CommandPool;
	class SwapChain;
	class Shader;
	class VertexBuffer;

	class CommandBuffer : public IUniqueHandle
	{
	protected:
		CommandBuffer() noexcept = default;

	public:
		struct CreateParams
		{
			const CommandPool& commandPool;
		};

		NO_MOVE(CommandBuffer);
		NO_COPY(CommandBuffer);

		virtual
		~CommandBuffer() noexcept = default;

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

		virtual
		void
		DrawVertexBuffer(const VertexBuffer& a_buffer) const noexcept = 0;

		virtual
		void
		TransitionImageLayout(Image& a_image, ImageLayout a_newLayout) const noexcept = 0;

		virtual
		void
		TransitionImageLayout(ImageId a_image, ImageLayout a_oldLayout, ImageLayout a_newLayout) const noexcept = 0;

	protected:
		virtual
		const CommandPool*
		GetCommandPool() const noexcept = 0;
	};

	using CommandBufferHandle = PImpl<CommandBuffer>;
}
