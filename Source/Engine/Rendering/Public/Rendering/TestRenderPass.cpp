#include "TestRenderPass.h"

#include "RenderEngine.h"

namespace Oyl
{
	TestRenderPass::TestRenderPass(Vector2u a_size)
	{
		CreateRenderTarget(a_size);

		auto& renderEngine = RenderEngine::Get();

		ShaderCompileResult result;
		renderEngine.GetShaderCompiler().CompileHlslFromFile(
			"G:/dev/Oyl3D/Oyl3D/Source/Engine/Rendering/Private/Shaders/shader.hlsl",
			&result
		);

		auto& device = renderEngine.GetDevice();

		m_shader = device.CreateShader(
			{
				.renderTarget = m_renderTarget,
				.compileResult = result,
			}
		);

		const Array<Vertex> vertices = std::vector {
			Vertex { Vector2f { -0.5f, -0.5f }, Vector3f { 1.0f, 0.0f, 0.0f } },
			Vertex { Vector2f { 0.5f, -0.5f }, Vector3f { 0.0f, 1.0f, 0.0f } },
			Vertex { Vector2f { 0.5f, 0.5f }, Vector3f { 0.0f, 0.0f, 1.0f } },
			Vertex { Vector2f { -0.5f, 0.5f }, Vector3f { 1.0f, 1.0f, 1.0f } },
		};

		const Array<uint16> indices  = std::vector<uint16> { 0, 1, 2, 2, 3, 0 };

		Array<byte> vertexData;
		vertexData.insert(vertexData.end(), (byte*) &vertices[0], (byte*) &vertices.data()[vertices.size()]);
		Array<byte> indexData;
		indexData.insert(indexData.end(), (byte*) &indices[0], (byte*) &indices.data()[indices.size()]);

		m_vertexBuffer = device.CreateVertexBuffer(
			{
				.vertexData = vertexData,
				.vertexStride = sizeof(Vertex),
				.indexData = indexData,
				.indexStride = sizeof(uint16)
			}
		);
	}

	void
	TestRenderPass::OnViewPortResize(Vector2u a_size)
	{
		CreateRenderTarget(a_size);
	}

	void
	TestRenderPass::Begin(const ICommandBuffer& a_commandBuffer)
	{
		auto size = m_colorAttachment->GetSize();

		a_commandBuffer.SetViewport(Vector2i::Zero, size);
		a_commandBuffer.SetScissor(Vector2i::Zero, size);
		a_commandBuffer.TransitionImageLayout(*m_colorAttachment, ImageLayout::ColorAttachment);

		a_commandBuffer.BeginRendering(GetRenderTarget());
	}

	void
	TestRenderPass::Render(const ICommandBuffer& a_commandBuffer)
	{
		a_commandBuffer.BindShader(*m_shader);
		a_commandBuffer.DrawVertexBuffer(*m_vertexBuffer);
	}

	void
	TestRenderPass::End(const ICommandBuffer& a_commandBuffer)
	{
		a_commandBuffer.EndRendering();
	}

	void
	TestRenderPass::CreateRenderTarget(Vector2u a_size)
	{
		auto& device = RenderEngine::Get().GetDevice();
		m_colorAttachment = device.CreateImage(
			{
				.size = a_size,
				.format = ImageFormat::RGBA_8_SRGB,
				.usageFlags = ImageUsageFlagBits::ColorAttachment | ImageUsageFlagBits::TransferSrc,
				.layout = ImageLayout::ColorAttachment
			}
		);
		m_renderTarget = RenderTarget({ .colorImages = &*m_colorAttachment });
	}
}
