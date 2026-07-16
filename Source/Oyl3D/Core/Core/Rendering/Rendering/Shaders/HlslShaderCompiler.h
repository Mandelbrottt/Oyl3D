#pragma once

#include <memory>

#include "Rendering/Shaders/ShaderCompiler.h"

namespace Oyl::Rendering::Vulkan
{
	class OYL_RENDERING_API ShaderCompiler : public Rendering::ShaderCompiler
	{
	public:
		ShaderCompiler();

		virtual
		~ShaderCompiler();

	private:
		bool
		CompileHlsl(std::string_view a_source, ShaderCompileResult* a_outShader) const override;

		struct Impl;
		std::unique_ptr<Impl> m_impl;
	};
}
