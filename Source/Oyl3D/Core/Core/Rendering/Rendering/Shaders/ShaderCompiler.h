#pragma once

#include "Shader.h"

namespace Oyl::Rendering
{
	class ShaderCompileResult;

	class OYL_RENDERING_API ShaderCompiler
	{
	protected:
		ShaderCompiler();

	public:
		virtual
		~ShaderCompiler();

		bool
		CompileHlslFromFile(std::string_view a_filePath, ShaderCompileResult* a_outShader) const;

		bool
		CompileHlslFromSource(std::string_view a_source, ShaderCompileResult* a_outShader) const;

	protected:
		virtual
		bool
		CompileHlsl(std::string_view a_source, ShaderCompileResult* a_outShader) const = 0;
	};

	class OYL_RENDERING_API ShaderStage final
	{
	public:
		ShaderStage();

		~ShaderStage();
	};

	class OYL_RENDERING_API ShaderCompileResult
	{
		friend class ShaderCompiler;

	public:
		ShaderCompileResult();

		struct Params
		{
			std::string_view filePath;
			std::string_view sourceCode;
		};

		explicit
		ShaderCompileResult(const Params& a_params);

		~ShaderCompileResult();

		std::string_view
		GetFilePath() const;

		std::string_view
		GetSourceCode() const;

	private:
		std::string m_filePath;
		std::string m_sourceCode;
		std::unordered_map<ShaderProfile, ShaderStage> m_stages;
	};
}
