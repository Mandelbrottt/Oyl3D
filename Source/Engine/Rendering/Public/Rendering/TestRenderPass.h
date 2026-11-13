#pragma once

#include "RenderTarget.h"

#include "Rendering/Hardware/RdoImage.h"
#include "Rendering/Hardware/RdoShader.h"
#include "Rendering/Hardware/RdoUniformBuffer.h"
#include "Rendering/Hardware/RdoVertexBuffer.h"
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
		~TestRenderPass();

		void
		OnViewPortResize(Vector2u a_size) override;

		const RdoImageHandle&
		GetColorAttachment() const override { return m_colorAttachment; }

	protected:
		void
		Begin(const IRdoCommandBuffer& a_commandBuffer) override;

		void
		Render(const IRdoCommandBuffer& a_commandBuffer) override;

		void
		End(const IRdoCommandBuffer& a_commandBuffer) override;

	private:
		void
		CreateRenderTarget(Vector2u a_size);

		void
		UpdateUniformBuffer();

		void
		OnDebugGui();

	private:
		RdoImageHandle m_colorAttachment = nullptr;

		RdoShaderHandle m_shader = nullptr;
		RdoVertexBufferHandle m_vertexBuffer = nullptr;
		RdoUniformBufferHandle m_uniformBuffer = nullptr;

		Vector3 m_gunPosition;
	};
}
