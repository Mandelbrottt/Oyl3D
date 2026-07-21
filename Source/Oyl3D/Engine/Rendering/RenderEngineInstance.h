#pragma once

#include "Rendering/Shader.h"
#include "Rendering/VertexBuffer.h"

namespace Oyl::Rendering
{
	class RenderContext;
	class Device;
	class ShaderCompiler;

	class OYL_RENDERING_API RenderEngineInstance
	{
	protected:
		explicit
		RenderEngineInstance(
			Internal::ResourceManager* a_resourceManager,
			const RenderContext* a_renderContext
		);

	public:
		virtual
		~RenderEngineInstance();

		virtual
		const Device*
		GetCurrentDevice() const;

		virtual
		const ShaderCompiler*
		GetShaderCompiler() const;

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
		SetCurrentDevice(const Device* a_device);

		virtual
		void
		SetShaderCompiler(std::unique_ptr<ShaderCompiler>&& a_shaderCompiler);

	private:
		Internal::ResourceManager* m_resourceManager;
		std::unique_ptr<ShaderCompiler> m_shaderCompiler;

		const RenderContext* m_renderContext = nullptr;
		const Device* m_device = nullptr;
	};
}
