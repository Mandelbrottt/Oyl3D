#pragma once

#include "RenderEngineInstance.h"

namespace Oyl::Rendering::Vulkan
{
	class OYL_RENDERING_API RenderEngineInstance : public Rendering::RenderEngineInstance
	{
	public:
		explicit
		RenderEngineInstance(Internal::ResourceManager* a_resourceManager);

		Shader
		CreateShader(const ShaderOptions& a_options) override;

		VertexBuffer
		CreateVertexBuffer(const VertexBufferOptions& a_options) override;
	};
}