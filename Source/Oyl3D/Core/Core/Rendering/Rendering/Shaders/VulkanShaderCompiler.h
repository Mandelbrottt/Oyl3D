#pragma once

#include "ShaderCompiler.h"
#include "VulkanShader.h"

namespace Oyl::Rendering::Internal
{
	class OYL_RENDERING_API VulkanShaderCompiler : public ShaderCompiler
	{
	public:
		VulkanShaderCompiler();

		virtual
		~VulkanShaderCompiler();

		VulkanShader*
		Compile() override;

	private:
		struct Impl;
		std::unique_ptr<Impl> m_impl;
	};
}
