#include "TestRenderPass.h"

#include <Application/ApplicationModule.h>

#include <Core/Math/Matrix4.h>
#include <Core/Math/Transformations.h>
#include <Core/Modules/ModuleManager.h>
#include <Core/Time/Time.h>

#include "RenderEngine.h"
#include "Rendering.h"

namespace Oyl
{
	struct UniformBufferObject
	{
		alignas(16) Matrix4 mvp;
	};

	TestRenderPass::TestRenderPass(Vector2u a_size)
	{
		CreateRenderTarget(a_size);

		CreateDescriptors();
		CreateRdoObjects();

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
	TestRenderPass::Render(const IRdoCommandBuffer& a_commandBuffer)
	{
		auto size = m_colorAttachment->GetSize();

		a_commandBuffer.SetViewport(Vector2i::Zero, size);
		a_commandBuffer.SetScissor(Vector2i::Zero, size);
		a_commandBuffer.TransitionImageLayout(m_colorAttachment, ImageLayout::ColorAttachment);

		a_commandBuffer.BeginRendering(GetRenderTarget(),
		                               {
			                               .clearColor = Vector4(0.0f, 0.0f, 0.0f, 1.0f)
		                               });

		a_commandBuffer.BindShader(m_shader);

		UpdateUniformBuffer(m_material1, m_gunPosition1);
		a_commandBuffer.BindShaderBindingSet(m_shader, m_material1->GetShaderBindingSet());
		a_commandBuffer.DrawVertexBuffer(m_vertexBuffer);

		UpdateUniformBuffer(m_material2, m_gunPosition2);
		a_commandBuffer.BindShaderBindingSet(m_shader, m_material2->GetShaderBindingSet());
		a_commandBuffer.DrawVertexBuffer(m_vertexBuffer);

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
	TestRenderPass::CreateDescriptors()
	{
		auto shader = Shader::Make("G:/dev/Oyl3D/Oyl3D/Source/Engine/Rendering/Private/Shaders/shader.hlsl");
		m_material1 = Material::Make();
		m_material1->SetShader(shader);

		m_material2 = Material::Make();
		m_material2->SetShader(shader);

		m_model = Model::Make("G:/dev/Oyl3D/Oyl3D/Source/Packages/Assimp/test/models-nonbsd/OBJ/rifle.obj");
		OYL_ASSERT(m_model && !m_model->GetMeshes().Empty());
	}

	void
	TestRenderPass::CreateRdoObjects()
	{
		auto& renderEngine = RenderEngine::Get();

		ShaderCompileResult result;
		renderEngine.GetShaderCompiler().CompileHlslFromFile(
			m_material1->GetShader()->GetPath(),
			&result
		);

		auto& device = renderEngine.GetDevice();

		m_shader = device.CreateShader(
			{
				.renderTarget = m_renderTarget,
				.compileResult = result,
			}
		);

		Array<byte> vertexData;
		Array<byte> indexData;
		if (m_model->GetMeshes().Length() > 0)
		{
			auto& mesh = m_model->GetMeshes()[0];
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

			vertexData.insert(vertexData.end(), (byte*) &vertices[0], (byte*) &vertices.data()[vertices.size()]);
			indexData.insert(indexData.end(), (byte*) &indices[0], (byte*) &indices.data()[indices.size()]);
		}

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
	TestRenderPass::UpdateUniformBuffer(const Material& a_material, const Vector3& a_position)
	{
		static auto startTime = Time::ElapsedTime();

		auto currentTime = Time::ElapsedTime();
		float time = float32(currentTime - startTime);

		Matrix4 model = Matrix4(Matrix::RotateY(time * 90)) * Matrix::Translate(a_position);
		Matrix4 view = Matrix::LookAt(Vector3(0, 100.f, 100.f), Vector3(0, 0, 0));;
		float sizeX = (float) m_colorAttachment->GetSize().x;
		float sizeY = (float) m_colorAttachment->GetSize().y;
		Matrix4 projection =  Matrix::Perspective(75, sizeX / sizeY, 0.01f, 1000.f);
		Matrix4 mvp = model * view * projection;

		a_material->SetMatrix("g_mvp", mvp);
	}

	void
	TestRenderPass::OnDebugGui()
	{
		ImGui::Begin("Gun");

		ImGui::DragFloat3("Gun Position 1", m_gunPosition1.data, 1, -100, -5);
		ImGui::DragFloat3("Gun Position 2", m_gunPosition2.data, 1, 5, 100);

		ImGui::End();
	}
}
