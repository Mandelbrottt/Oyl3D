#pragma once

#include "Rendering/Shaders/Shader.h"

namespace Oyl::Rendering
{
	class OYL_RENDERING_API ShaderCompiler
	{
	protected:
		ShaderCompiler();

	public:
		virtual
		~ShaderCompiler();

		virtual
		Shader*
		Compile() = 0;
	};
}