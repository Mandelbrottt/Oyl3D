#pragma once

#include <Core/Math/Rect.h>
#include <Core/Math/Vector.h>

//#include "Image.h"
#include <Core/Interfaces/IDestroyable.h>

#include "Image.h"
#include "RenderingEnums.h"
#include "SwapChain.h"

namespace Oyl
{
	class RenderTarget;
	class CommandPool;
	class SwapChain;
	class Shader;
	class VertexBuffer;

	struct CommandBufferCreateParams
	{
		const CommandPool& commandPool;
	};

	class CommandBuffer : public IDestroyable
	{
	protected:
		CommandBuffer() noexcept = default;

	public:
		NO_MOVE(CommandBuffer);
		NO_COPY(CommandBuffer);

		virtual
		~CommandBuffer() noexcept = 0;

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

		virtual
		void
		BlitImage(const Image& a_srcImage, const Image& a_dstImage, ImageFilter a_filter) const noexcept = 0;

		virtual
		void
		BlitImage(
			ImageId a_srcImageId,
			ImageLayout a_srcLayout,
			Rect2D a_srcRect,
			ImageId a_dstImageId,
			ImageLayout a_dstImageLayout,
			Rect2D a_dstRect,
			ImageFilter a_filter
		) const noexcept = 0;

	protected:
		virtual
		const CommandPool*
		GetCommandPool() const noexcept = 0;
	};

	inline
	CommandBuffer::~CommandBuffer() noexcept {}
}
