#pragma once

#include "Rendering/Shader.h"
#include "Rendering/VertexBuffer.h"

namespace Oyl::Rendering
{
	class IRenderContext;
	class IDevice;
	class ShaderCompiler;
}

namespace Oyl::Rendering::Internal
{
	class OYL_RENDERING_API RenderEngineInstance
	{
	protected:
		RenderEngineInstance();

		struct CreateParams
		{
			std::unique_ptr<ShaderCompiler> shaderCompiler;
			std::unique_ptr<IRenderContext> renderContext;
		};

		explicit
		RenderEngineInstance(CreateParams a_params);

	public:
		virtual
		~RenderEngineInstance();

	#pragma region Instance State
		virtual
		Oyl::Internal::ResourceManager*
		GetResourceManager() const;

		virtual
		const ShaderCompiler*
		GetShaderCompiler() const;

		virtual
		const IDevice*
		GetCurrentDevice() const;

		virtual
		IRenderContext*
		GetRenderContext() const;
	#pragma endregion Instance State

	#pragma region Resources
		virtual
		Shader
		CreateShader(const ShaderOptions& a_options) = 0;

		virtual
		VertexBuffer
		CreateVertexBuffer(const VertexBufferOptions& a_options) = 0;
	#pragma endregion Resources

	protected:
		std::unique_ptr<Oyl::Internal::ResourceManager> m_resourceManager;
		std::unique_ptr<ShaderCompiler> m_shaderCompiler;
		std::unique_ptr<IDevice> m_device;

		// TEMPORARY: Should the renderer own the render context?
		std::unique_ptr<IRenderContext> m_renderContext;
	};
}
