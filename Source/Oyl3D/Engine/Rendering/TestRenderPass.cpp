#include "TestRenderPass.h"

#include "RenderEngine.h"
#include "Window.h"

namespace Oyl::Rendering
{
	TestRenderPass::TestRenderPass()
	{
		CreateRenderTarget(RenderEngine::GetCurrentViewPortSize());

		ShaderCompileResult result;
		RenderEngine::GetShaderCompiler()->CompileHlslFromFile(
			"G:/dev/Oyl3D/Oyl3D/Source/Oyl3D/Engine/Rendering/Shaders/shader.hlsl",
			&result
		);

		m_shader = RenderEngine::GetCurrentDevice()->CreateShader(
			{
				.renderTarget = m_renderTarget,
				.compileResult = result,
			}
		);

		const std::vector vertices {
			Vertex { Vector2f { -0.5f, -0.5f }, Vector3f { 1.0f, 0.0f, 0.0f } },
			Vertex { Vector2f { 0.5f, -0.5f }, Vector3f { 0.0f, 1.0f, 0.0f } },
			Vertex { Vector2f { 0.5f, 0.5f }, Vector3f { 0.0f, 0.0f, 1.0f } },
			Vertex { Vector2f { -0.5f, 0.5f }, Vector3f { 1.0f, 1.0f, 1.0f } },
		};

		const std::vector<uint16> indices { 0, 1, 2, 2, 3, 0 };

		std::vector<byte> vertexData;
		vertexData.insert(vertexData.end(), (byte*) &vertices[0], (byte*) &vertices.data()[vertices.size()]);
		std::vector<byte> indexData;
		indexData.insert(indexData.end(), (byte*) &indices[0], (byte*) &indices.data()[indices.size()]);

		m_vertexBuffer = RenderEngine::GetCurrentDevice()->CreateVertexBuffer(
			{
				.vertexData = vertexData,
				.vertexStride = sizeof(Vertex),
				.indexData = indexData,
				.indexStride = sizeof(uint16)
			}
		);
	}

	void
	TestRenderPass::OnWindowResizedEvent(const WindowResizeEvent& a_event)
	{
		CreateRenderTarget(a_event.size);
	}

	void
	TestRenderPass::Begin(const CommandBufferImpl& a_commandBuffer)
	{
		auto size = m_colorAttachment->GetSize();

		a_commandBuffer.SetViewport(Vector2i::Zero(), size);
		a_commandBuffer.SetScissor(Vector2i::Zero(), size);
		a_commandBuffer.TransitionImageLayout(*m_colorAttachment, ImageLayout::ColorAttachment);

		a_commandBuffer.BeginRendering(*GetRenderTarget());
	}

	void
	TestRenderPass::Render(const CommandBufferImpl& a_commandBuffer)
	{
		a_commandBuffer.BindShader(*m_shader);
		a_commandBuffer.DrawVertexBuffer(*m_vertexBuffer);
	}

	void
	TestRenderPass::End(const CommandBufferImpl& a_commandBuffer)
	{
		a_commandBuffer.EndRendering();

		a_commandBuffer.TransitionImageLayout(*m_colorAttachment, ImageLayout::TransferSource);
	}

	void
	TestRenderPass::CreateRenderTarget(Vector2u a_size)
	{
		m_colorAttachment = RenderEngine::GetCurrentDevice()->CreateImage(
			{
				.size = a_size,
				.format = ImageFormat::RGBA_8_SRGB,
				.usageFlags = ImageUsageFlagBits::ColorAttachment | ImageUsageFlagBits::TransferSrc,
				.layout = ImageLayout::ColorAttachment
			}
		);
		m_renderTarget = RenderTarget({ .colorImages = &*m_colorAttachment });
		SetRenderTarget(&m_renderTarget);
	}
}
