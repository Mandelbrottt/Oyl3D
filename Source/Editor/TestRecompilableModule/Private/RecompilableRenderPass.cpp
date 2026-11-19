#include "RecompilableRenderPass.h"

#include <Application/ApplicationModule.h>

#include <Core/Logging/Logging.h>
#include <Core/Math/Matrix4.h>
#include <Core/Math/Transformations.h>
#include <Core/Modules/ModuleManager.h>
#include <Core/Time/Time.h>

#include <EditorClient/EditorClientApplication.h>

#include "Rendering/RenderEngine.h"
#include "Rendering/Rendering.h"

namespace Oyl
{
	struct UniformBufferObject
	{
		alignas(16) Matrix4 mvp;
	};

	RecompilableRenderPass::RecompilableRenderPass(Vector2u a_size)
	{
		CreateRenderTarget(a_size);

		CreateDescriptors();
		CreateRdoObjects();

		auto app = ModuleManager::GetModulePointer<IApplicationModule>("Application")->CurrentApplication();
		app->GetApplicationWindows().front()->OnKeyPressCallback() += InstanceDelegate(OnKeyPress);
	}

	RecompilableRenderPass::~RecompilableRenderPass()
	{
		auto app = ModuleManager::GetModulePointer<IApplicationModule>("Application")->CurrentApplication();
		app->GetApplicationWindows().front()->OnKeyPressCallback() -= InstanceDelegate(OnKeyPress);
	}

	void
	RecompilableRenderPass::OnViewPortResize(Vector2u a_size)
	{
		CreateRenderTarget(a_size);
	}

	void
	RecompilableRenderPass::Render(const IRdoCommandBuffer& a_commandBuffer)
	{
		auto size = m_colorAttachment->GetSize();

		a_commandBuffer.SetViewport(Vector2i::Zero, size);
		a_commandBuffer.SetScissor(Vector2i::Zero, size);
		a_commandBuffer.TransitionImageLayout(m_colorAttachment, ImageLayout::ColorAttachment);

		a_commandBuffer.BeginRendering(GetRenderTarget(), {.clearColor = Vector4::Zero});

		a_commandBuffer.BindShader(m_shader);

		UpdateUniformBuffer(m_material);
		a_commandBuffer.BindShaderBindingSet(m_shader, m_material->GetShaderBindingSet());
		a_commandBuffer.DrawVertexBuffer(m_vertexBuffer);

		a_commandBuffer.EndRendering();
	}

	void
	RecompilableRenderPass::CreateRenderTarget(Vector2u a_size)
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
	RecompilableRenderPass::CreateDescriptors()
	{
		auto shader = Shader::Make("G:/dev/Oyl3D/Oyl3D/Source/Engine/Rendering/Private/Shaders/shader.hlsl");
		m_material = Material::Make();
		m_material->SetShader(shader);

		m_model = Model::Make("G:/dev/Oyl3D/Oyl3D/Source/Packages/Assimp/test/models-nonbsd/FBX/2013_ASCII/mp5_sil.fbx");
		OYL_ASSERT(m_model && !m_model->GetMeshes().Empty());
	}

	void
	RecompilableRenderPass::CreateRdoObjects()
	{
		auto& renderEngine = RenderEngine::Get();

		ShaderCompileResult result;
		renderEngine.GetShaderCompiler().CompileHlslFromFile(
			m_material->GetShader()->GetPath(),
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
	RecompilableRenderPass::UpdateUniformBuffer(const Material& a_material)
	{
		static float time = 0;

		time += Time::DeltaTime();

		Matrix4 model = Matrix4(Matrix::RotateY(time * 90)) * Matrix::Translate(m_position);
		Matrix4 view = Matrix::LookAt(Vector3(0, 100.f, 100.f), Vector3(0, 0, 0));;
		float sizeX = (float) m_colorAttachment->GetSize().x;
		float sizeY = (float) m_colorAttachment->GetSize().y;
		Matrix4 projection = Matrix::Perspective(75, sizeX / sizeY, 0.01f, 1000.f);
		Matrix4 mvp = model * view * projection;

		a_material->SetMatrix("g_mvp", mvp);
	}

	void
	RecompilableRenderPass::OnKeyPress(const WindowKeyPressEvent& a_event)
	{
		if (a_event.key == Input::KB_W)
			m_position.z -= 5;
		if (a_event.key == Input::KB_S)
			m_position.z += 5;
		if (a_event.key == Input::KB_A)
			m_position.x -= 5;
		if (a_event.key == Input::KB_D)
			m_position.x += 5;
		if (a_event.key == Input::KB_Q)
			m_position.y -= 5;
		if (a_event.key == Input::KB_E)
			m_position.y += 5;

		LogDebug("Haha!");
	}
}
