#pragma once

#include <memory>

#include "Rendering/ShaderCompiler.h"

namespace Oyl::Rendering::Vulkan
{
	class OYL_RENDERING_API ShaderCompiler : public Rendering::ShaderCompiler
	{
	public:
		ShaderCompiler();

		virtual
		~ShaderCompiler();

		ShaderCompiler(const ShaderCompiler& a_other);
		ShaderCompiler&
		operator =(const ShaderCompiler& a_other);

		ShaderCompiler(ShaderCompiler&& a_other) noexcept;
		ShaderCompiler&
		operator =(ShaderCompiler&& a_other) noexcept;

	private:
		bool
		CompileHlsl(std::string_view a_source, ShaderCompileResult* a_outShader) const override;

		struct Impl;
		std::unique_ptr<Impl> m_impl;
	};
}
