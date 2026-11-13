#include "TestRenderPass.h"

#include <Application/ApplicationModule.h>

#include <Core/Math/Matrix4.h>
#include <Core/Math/Transformations.h>
#include <Core/Modules/ModuleManager.h>
#include <Core/Time/Time.h>

#include "RenderEngine.h"
#include "Rendering.h"

#include "../../../../../../../VulkanSDK/1.4.341.1/Include/glm/ext/matrix_clip_space.hpp"

namespace Oyl
{
	struct UniformBufferObject
	{
		alignas(16) Matrix4 model;
		alignas(16) Matrix4 view;
		alignas(16) Matrix4 projection;
	};

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

		auto model = ModuleManager::GetModulePointer<IRenderingModule>("Rendering")->CreateModel("G:/dev/Oyl3D/Oyl3D/Source/Packages/Assimp/test/models-nonbsd/OBJ/rifle.obj");
		if (model.HasValue())
		{
			auto mesh = model->GetMeshes()[0];
			Array<Vertex> vertices;
			for (uint i = 0; i < mesh.GetVertexPositions().Length(); i++)
			{
				vertices.Add(Vertex(mesh.GetVertexPositions()[i], mesh.GetVertexNormals()[i] * 0.5f + Vector3(0.5f)));
			}
			Array<uint16> indices;
			for (uint i = 0; i < mesh.GetVertexIndices().Length(); i++)
			{
				indices.Add((uint16) mesh.GetVertexIndices()[i]);
			}

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
		} else
		{
			const Array<Vertex> vertices = std::vector {
				Vertex { Vector3f { -0.5f, 0, -0.5f }, Vector3f { 1.0f, 0.0f, 0.0f } },
				Vertex { Vector3f { -0.5f, 0, 0.5f }, Vector3f { 1.0f, 1.0f, 1.0f } },
				Vertex { Vector3f { 0.5f, 0, 0.5f }, Vector3f { 0.0f, 0.0f, 1.0f } },
				Vertex { Vector3f { 0.5f, 0, -0.5f }, Vector3f { 0.0f, 1.0f, 0.0f } },
			};

			const Array<uint16> indices = std::vector<uint16> { 0, 1, 2, 2, 3, 0 };

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

		m_uniformBuffer = device.CreateUniformBuffer({
			.stride = sizeof(UniformBufferObject)
		});

		m_shader->SetUniformBuffer(renderEngine.GetDevice(), m_uniformBuffer);

		auto* application = ModuleManager::GetModulePointer<IApplicationModule>("Application")->CurrentApplication();
		application->OnDebugGuiCallback() += InstanceDelegate(OnDebugGui);
	}

	TestRenderPass::~TestRenderPass()
	{
		auto* application = ModuleManager::GetModulePointer<IApplicationModule>("Application")->CurrentApplication();
		application->OnDebugGuiCallback() -= InstanceDelegate(OnDebugGui);
	}

	void
	TestRenderPass::OnViewPortResize(Vector2u a_size)
	{
		CreateRenderTarget(a_size);
	}

	void
	TestRenderPass::Begin(const IRdoCommandBuffer& a_commandBuffer)
	{
		auto size = m_colorAttachment->GetSize();

		a_commandBuffer.SetViewport(Vector2i::Zero, size);
		a_commandBuffer.SetScissor(Vector2i::Zero, size);
		a_commandBuffer.TransitionImageLayout(*m_colorAttachment, ImageLayout::ColorAttachment);

		a_commandBuffer.BeginRendering(GetRenderTarget(),
		                               {
			                               .clearColor = Vector4(0.0f, 0.0f, 0.0f, 1.0f)
		                               });
	}

	void
	TestRenderPass::Render(const IRdoCommandBuffer& a_commandBuffer)
	{
		UpdateUniformBuffer();

		a_commandBuffer.BindShader(*m_shader);
		m_shader->BindUniformBuffer(a_commandBuffer, m_uniformBuffer);
		a_commandBuffer.DrawVertexBuffer(*m_vertexBuffer);
	}

	void
	TestRenderPass::End(const IRdoCommandBuffer& a_commandBuffer)
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

	void
	TestRenderPass::UpdateUniformBuffer()
	{
		static auto startTime = Time::ElapsedTime();

		auto currentTime = Time::ElapsedTime();
		float time = float32(currentTime - startTime);

		UniformBufferObject ubo;
		ubo.model = Matrix4(Matrix::RotateY(time * 90)) * Matrix::Translate(m_gunPosition);
		ubo.view = Matrix::LookAt(Vector3(0, 100.f, 100.f), Vector3(0, 0, 0));
		float sizeX = (float) m_colorAttachment->GetSize().x;
		float sizeY = (float) m_colorAttachment->GetSize().y;
		ubo.projection = Matrix::Perspective(75, sizeX / sizeY, 0.01f, 1000.f);
		m_uniformBuffer->CopyToBuffer(&ubo);
	}

	void
	TestRenderPass::OnDebugGui()
	{
		ImGui::Begin("Gun");

		ImGui::DragFloat3("Gun Position", m_gunPosition.data, 1, -100, 100);

		ImGui::End();
	}
}
