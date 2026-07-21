#pragma once

#include "Rendering/RenderEngineInstance.h"
#include "Rendering/Shader.h"
#include "Rendering/VertexBuffer.h"

namespace Oyl::Rendering
{
	class OYL_RENDERING_API RenderEngine final
	{
		friend class RenderControlModule;

	public:
		RenderEngine() = delete;

		static
		const Device*
		GetCurrentDevice()
		{
			return s_instance->GetCurrentDevice();
		}

		static
		const ShaderCompiler*
		GetShaderCompiler()
		{
			return s_instance->GetShaderCompiler();
		}

		static
		Shader
		CreateShader(ShaderOptions a_options)
		{
			return s_instance->CreateShader(std::move(a_options));
		}

		static
		VertexBuffer
		CreateVertexBuffer(VertexBufferOptions a_options)
		{
			return s_instance->CreateVertexBuffer(std::move(a_options));
		}

	private:
		static
		RenderEngineInstance*
		GetInstance()
		{
			return s_instance;
		}

		static
		void
		SetInstance(RenderEngineInstance* a_instance)
		{
			s_instance = a_instance;
		}

	private:
		static RenderEngineInstance* s_instance;
	};
}
