#pragma once

#include <memory>

#include "Rendering/ShaderCompiler.h"

namespace Oyl::Rendering
{
	class OYL_RENDERING_API VulkanShaderCompiler : public ShaderCompiler
	{
	public:
		VulkanShaderCompiler();

		virtual
		~VulkanShaderCompiler();

		VulkanShaderCompiler(const VulkanShaderCompiler& a_other);
		VulkanShaderCompiler&
		operator =(const VulkanShaderCompiler& a_other);

		VulkanShaderCompiler(VulkanShaderCompiler&& a_other) noexcept;
		VulkanShaderCompiler&
		operator =(VulkanShaderCompiler&& a_other) noexcept;

	private:
		bool
		CompileHlsl(std::string_view a_source, ShaderCompileResult* a_outShader) const override;

		struct Impl;
		std::unique_ptr<Impl> m_impl;
	};
}
