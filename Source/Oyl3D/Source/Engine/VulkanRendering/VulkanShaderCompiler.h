#pragma once

#include <memory>

#include "Rendering/ShaderCompiler.h"

namespace Oyl
{
	class VulkanShaderCompiler : public IShaderCompiler
	{
	public:
		VulkanShaderCompiler();

		VulkanShaderCompiler(const VulkanShaderCompiler& a_other);
		VulkanShaderCompiler&
		operator =(const VulkanShaderCompiler& a_other);

		VulkanShaderCompiler(VulkanShaderCompiler&& a_other) noexcept;
		VulkanShaderCompiler&
		operator =(VulkanShaderCompiler&& a_other) noexcept;

		virtual
		~VulkanShaderCompiler();

	private:
		bool
		CompileHlsl(StringView a_source, ShaderCompileResult* a_outShader) const override;

	private:
		struct Impl;
		std::unique_ptr<Impl> m_impl;
	};
}
