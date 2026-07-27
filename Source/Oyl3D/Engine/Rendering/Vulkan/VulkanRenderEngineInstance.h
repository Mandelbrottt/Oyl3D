#pragma once

#include "VulkanDevice.h"
#include "VulkanShader.h"
#include "VulkanShaderCompiler.h"
#include "VulkanVertexBuffer.h"

#include "Rendering/RenderEngineInstance.h"

namespace Oyl::Rendering::Vulkan
{
	class OYL_RENDERING_API RenderEngineInstance : public Internal::RenderEngineInstance
	{
	public:
		struct CreateParams
		{
			const IWindow* window;
		};

		explicit
		RenderEngineInstance(CreateParams a_params);

		virtual
		~RenderEngineInstance() = default;

		const ShaderCompiler*
		GetShaderCompiler() const override
		{
			const auto* abstract = Internal::RenderEngineInstance::GetShaderCompiler();
			return static_cast<const ShaderCompiler*>(abstract);
		}

		const Device*
		GetCurrentDevice() const override
		{
			const auto* abstract = Internal::RenderEngineInstance::GetCurrentDevice();
			return static_cast<const Device*>(abstract);
		}

		//Rendering::Shader
		//CreateShader(const ShaderOptions& a_options) override;

		//Rendering::VertexBuffer
		//CreateVertexBuffer(const VertexBufferOptions& a_options) override;
	};
}
