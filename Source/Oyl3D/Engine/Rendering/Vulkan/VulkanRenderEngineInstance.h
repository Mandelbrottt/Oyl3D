#pragma once

#include "VulkanDevice.h"
#include "VulkanShaderCompiler.h"

#include "Rendering/RenderEngineInstance.h"

namespace Oyl::Rendering::Vulkan
{
	class RenderContext;

	class OYL_RENDERING_API RenderEngineInstance : public Internal::RenderEngineInstance
	{
	public:
		struct CreateParams
		{
			const Window* window;
		};

		explicit
		RenderEngineInstance(CreateParams a_params);

		virtual
		~RenderEngineInstance() = default;

		Shader
		CreateShader(const ShaderOptions& a_options) override;

		VertexBuffer
		CreateVertexBuffer(const VertexBufferOptions& a_options) override;

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
	};
}
