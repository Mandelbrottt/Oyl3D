#pragma once

#include "Rendering/VertexBuffer.h"
#include "Rendering/Shader.h"

namespace Oyl::Rendering
{
	class ShaderCompiler;

	class OYL_RENDERING_API RenderEngineInstance
	{
	protected:
		explicit
		RenderEngineInstance(Internal::ResourceManager* a_resourceManager);

	public:
		virtual
		~RenderEngineInstance();

		virtual
		const ShaderCompiler*
		GetShaderCompiler() const
		{
			return m_shaderCompiler.get();
		}

		virtual
		Shader
		CreateShader(const ShaderOptions& a_options) = 0;

		virtual
		VertexBuffer
		CreateVertexBuffer(const VertexBufferOptions& a_options) = 0;

	protected:
		Internal::ResourceManager*
		GetResourceManager() const
		{
			return m_resourceManager;
		}

		virtual
		void
		SetShaderCompiler(std::unique_ptr<ShaderCompiler>&& a_shaderCompiler);

	private:
		Internal::ResourceManager* m_resourceManager;

		std::unique_ptr<ShaderCompiler> m_shaderCompiler;
	};
}