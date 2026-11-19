#pragma once

#include "Rendering/RenderTarget.h"
#include "Rendering/Hardware/RdoImage.h"
#include "Rendering/Hardware/RdoShader.h"
#include "Rendering/Hardware/RdoUniformBuffer.h"
#include "Rendering/Hardware/RdoVertexBuffer.h"
#include "Rendering/Objects/Material.h"
#include "Rendering/Objects/Mesh.h"
#include "Rendering/Objects/Model.h"
#include "Rendering/Objects/Shader.h"
#include "Rendering/RenderPass/ColorOutputRenderPass.h"
#include "Rendering/RenderPass/RenderPass.h"

namespace Oyl
{
	class TESTRECOMPILABLEMODULE_EXPORT RecompilableRenderPass
		: public IColorOutputRenderPass,
		  public RenderPass<RecompilableRenderPass>
	{
	public:
		explicit
		RecompilableRenderPass(Vector2u a_size);

		virtual
		~RecompilableRenderPass();

		void
		OnViewPortResize(Vector2u a_size) override;

		const RdoImageHandle&
		GetColorAttachment() const override { return m_colorAttachment; }

		void
		Render(const IRdoCommandBuffer& a_commandBuffer) override;

	private:
		void
		CreateRenderTarget(Vector2u a_size);

		void
		CreateDescriptors();

		void
		CreateRdoObjects();

		void
		UpdateUniformBuffer(const Material& a_material);

		void
		OnKeyPress(const WindowKeyPressEvent& a_event);

	private:
		RdoImageHandle m_colorAttachment = nullptr;

		Material m_material;
		Model m_model;

		RdoShaderHandle m_shader = nullptr;
		RdoVertexBufferHandle m_vertexBuffer = nullptr;

		Vector3 m_position = Vector3::Zero;
	};
}
