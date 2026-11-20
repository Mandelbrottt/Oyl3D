#pragma once

#include <Core/Containers/Optional.h>
#include <Core/Interfaces/IDestroyable.h>
#include <Core/Math/Rect.h>

#include "RdoCommandPool.h"
#include "RdoImage.h"
#include "RdoShader.h"
#include "RdoSwapChain.h"
#include "RdoVertexBuffer.h"

#include "Rendering/RenderingEnums.h"
#include "Rendering/RenderTarget.h"

namespace Oyl
{
	struct RdoCommandBufferCreateParams
	{
		const IRdoCommandPool& commandPool;
	};

	struct BeginRenderingParams
	{
		Optional<Vector4> clearColor;
	};

	static_assert(std::is_default_constructible_v<BeginRenderingParams>);

	class IRdoCommandBuffer : public IDestroyable
	{
	protected:
		IRdoCommandBuffer() noexcept = default;
		DEFAULT_MOVE(IRdoCommandBuffer);

	public:
		NO_COPY(IRdoCommandBuffer);

		virtual
		~IRdoCommandBuffer() noexcept = 0;

		virtual
		void
		Begin() const noexcept = 0;

		virtual
		void
		BeginRendering(const RenderTarget& a_renderTarget, const BeginRenderingParams& a_params = {}) const noexcept = 0;

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
		BindShader(const IRdoShader& a_shader) const noexcept = 0;

		virtual
		void
		BindShaderBindingSet(const IRdoShader& a_shader, const IRdoShaderBindingSet& a_bindingSet) const noexcept = 0;

		virtual
		void
		BindVertexBuffer(const IRdoVertexBuffer& a_buffer) const noexcept = 0;

		virtual
		void
		DrawVertexBuffer(const IRdoVertexBuffer& a_buffer) const noexcept = 0;

		virtual
		void
		TransitionImageLayout(IRdoImage& a_image, ImageLayout a_newLayout) const noexcept = 0;

		virtual
		void
		TransitionImageLayout(RdoImageId a_image, ImageLayout a_oldLayout, ImageLayout a_newLayout) const noexcept = 0;

		virtual
		void
		BlitImage(const IRdoImage& a_srcImage, const IRdoImage& a_dstImage, ImageFilter a_filter) const noexcept = 0;

		virtual
		void
		BlitImage(
			RdoImageId a_srcImageId,
			ImageLayout a_srcLayout,
			Rect2D a_srcRect,
			RdoImageId a_dstImageId,
			ImageLayout a_dstImageLayout,
			Rect2D a_dstRect,
			ImageFilter a_filter
		) const noexcept = 0;

	protected:
		virtual
		const IRdoCommandPool*
		GetCommandPool() const noexcept = 0;
	};

	inline
	IRdoCommandBuffer::~IRdoCommandBuffer() noexcept {}

	using RdoCommandBufferHandle = IRenderDeviceObjectHandle<IRdoCommandBuffer>;
}
