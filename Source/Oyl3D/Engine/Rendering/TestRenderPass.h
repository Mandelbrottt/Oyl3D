#pragma once

#include "Image.h"
#include "RenderPass.h"
#include "RenderTarget.h"
#include "Shader.h"
#include "VertexBuffer.h"

namespace Oyl
{
	struct WindowResizeEvent;
}

namespace Oyl::Rendering
{
	class OYL_RENDERING_API TestRenderPass : public RenderPass<TestRenderPass>
	{
	public:
		TestRenderPass();

		virtual
		~TestRenderPass() = default;

		void
		OnWindowResizedEvent(const WindowResizeEvent& a_event);

	protected:
		void
		Begin(const CommandBuffer& a_commandBuffer) override;

		void
		Render(const CommandBuffer& a_commandBuffer) override;

		void
		End(const CommandBuffer& a_commandBuffer) override;

	private:
		void
		CreateRenderTarget(Vector2u a_size);

	private:
		ImageHandle m_colorAttachment = nullptr;
		RenderTarget m_renderTarget = nullptr;

		ShaderHandle m_shader = nullptr;
		VertexBufferHandle m_vertexBuffer = nullptr;
	};
}
