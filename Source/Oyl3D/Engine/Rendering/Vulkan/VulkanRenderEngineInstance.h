#pragma once

#include "Rendering/RenderEngineInstance.h"

namespace Oyl::Rendering::Vulkan
{
	class RenderContext;

	class OYL_RENDERING_API RenderEngineInstance : public Rendering::RenderEngineInstance
	{
	public:
		explicit
		RenderEngineInstance(
			Internal::ResourceManager* a_resourceManager,
			const RenderContext* a_renderContext
		);

		virtual
		~RenderEngineInstance() = default;

		Shader
		CreateShader(const ShaderOptions& a_options) override;

		VertexBuffer
		CreateVertexBuffer(const VertexBufferOptions& a_options) override;
	};
}