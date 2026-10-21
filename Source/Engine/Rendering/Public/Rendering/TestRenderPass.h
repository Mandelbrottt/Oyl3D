#pragma once

#include "Image.h"
#include "RenderPass.h"
#include "RenderTarget.h"
#include "Shader.h"
#include "VertexBuffer.h"

namespace Oyl
{
	class RENDERING_EXPORT TestRenderPass : public RenderPass<TestRenderPass>
	{
	public:
		explicit
		TestRenderPass(Vector2u a_size);

		virtual
		~TestRenderPass() = default;

		void
		Resize(Vector2u a_size);

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
		RenderTarget m_renderTarget;

		ShaderHandle m_shader = nullptr;
		VertexBufferHandle m_vertexBuffer = nullptr;
	};
}
