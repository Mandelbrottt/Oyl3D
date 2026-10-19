#pragma once

#include <Core/Math/Rect.h>
#include <Core/Math/Vector.h>

//#include "Image.h"
#include <Core/Interfaces/IDestroyable.h>

#include "Image.h"
#include "RenderingEnums.h"
#include "SwapChain.h"

#include "CommandPool.h"
#include "RenderTarget.h"
#include "Shader.h"
#include "VertexBuffer.h"

namespace Oyl
{
	struct CommandBufferCreateParams
	{
		const ICommandPool& commandPool;
	};

	class ICommandBuffer : public IDestroyable
	{
	protected:
		ICommandBuffer() noexcept = default;
		DEFAULT_MOVE(ICommandBuffer);

	public:
		NO_COPY(ICommandBuffer);

		virtual
		~ICommandBuffer() noexcept = 0;

		virtual
		void
		Begin() const noexcept = 0;

		virtual
		void
		BeginRendering(const IRenderTarget& a_renderTarget) const noexcept = 0;

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
		BindShader(const IShader& a_shader) const noexcept = 0;

		virtual
		void
		BindVertexBuffer(const IVertexBuffer& a_buffer) const noexcept = 0;

		virtual
		void
		DrawVertexBuffer(const IVertexBuffer& a_buffer) const noexcept = 0;

		virtual
		void
		TransitionImageLayout(IImage& a_image, ImageLayout a_newLayout) const noexcept = 0;

		virtual
		void
		TransitionImageLayout(ImageId a_image, ImageLayout a_oldLayout, ImageLayout a_newLayout) const noexcept = 0;

		virtual
		void
		BlitImage(const IImage& a_srcImage, const IImage& a_dstImage, ImageFilter a_filter) const noexcept = 0;

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
		const ICommandPool*
		GetCommandPool() const noexcept = 0;
	};

	inline
	ICommandBuffer::~ICommandBuffer() noexcept {}
}
