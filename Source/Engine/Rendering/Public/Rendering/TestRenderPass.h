#pragma once

#include "Image.h"
#include "RenderTarget.h"
#include "Shader.h"
#include "VertexBuffer.h"

#include "Rendering/RenderPass/RenderPass.h"

#include "RenderPass/ColorOutputRenderPass.h"

namespace Oyl
{
	class RENDERING_EXPORT TestRenderPass
		: public IColorOutputRenderPass,
		  public RenderPass<TestRenderPass>
	{
	public:
		explicit
		TestRenderPass(Vector2u a_size);

		virtual
		~TestRenderPass() = default;

		void
		OnViewPortResize(Vector2u a_size) override;

		const ImageHandle&
		GetColorAttachment() const override { return m_colorAttachment; }

	protected:
		void
		Begin(const ICommandBuffer& a_commandBuffer) override;

		void
		Render(const ICommandBuffer& a_commandBuffer) override;

		void
		End(const ICommandBuffer& a_commandBuffer) override;

	private:
		void
		CreateRenderTarget(Vector2u a_size);

	private:
		ImageHandle m_colorAttachment = nullptr;

		ShaderHandle m_shader = nullptr;
		VertexBufferHandle m_vertexBuffer = nullptr;
	};
}
