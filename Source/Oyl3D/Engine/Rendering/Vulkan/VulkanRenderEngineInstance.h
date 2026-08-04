#pragma once

#include "VulkanDevice.h"
#include "VulkanShader.h"
#include "VulkanShaderCompiler.h"
#include "VulkanVertexBuffer.h"

#include "Rendering/RenderEngineInstance.h"

namespace Oyl::Rendering
{
	class OYL_RENDERING_API VulkanRenderEngineInstance : public Internal::RenderEngineInstance
	{
	public:
		struct CreateParams
		{
			const IWindow* window;
		};

		explicit
		VulkanRenderEngineInstance(CreateParams a_params);

		virtual
		~VulkanRenderEngineInstance() = default;

		const VulkanShaderCompiler*
		GetShaderCompiler() const override
		{
			const auto* abstract = RenderEngineInstance::GetShaderCompiler();
			return static_cast<const VulkanShaderCompiler*>(abstract);
		}

		const VulkanDevice*
		GetCurrentDevice() const override
		{
			const auto* abstract = RenderEngineInstance::GetCurrentDevice();
			return static_cast<const VulkanDevice*>(abstract);
		}

		//Rendering::Shader
		//CreateShader(const ShaderOptions& a_options) override;

		//Rendering::VertexBuffer
		//CreateVertexBuffer(const VertexBufferOptions& a_options) override;
	};
}
