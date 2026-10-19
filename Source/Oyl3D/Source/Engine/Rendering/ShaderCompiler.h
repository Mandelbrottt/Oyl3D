#pragma once

#include <Core/Containers/Array.h>
#include <Core/Containers/String.h>

#include "RenderingEnums.h"

namespace Oyl
{
	class ShaderCompileResult;

	class ShaderCompiler
	{
	protected:
		ShaderCompiler() = default;

	public:
		virtual
		~ShaderCompiler() = default;

		DEFAULT_COPY(ShaderCompiler);
		DEFAULT_MOVE(ShaderCompiler);

		bool
		CompileHlslFromFile(StringView a_filePath, ShaderCompileResult* a_outShader) const;

		bool
		CompileHlslFromSource(StringView a_source, ShaderCompileResult* a_outShader) const;

	protected:
		virtual
		bool
		CompileHlsl(StringView a_source, ShaderCompileResult* a_outShader) const = 0;
	};

	struct ShaderStageCreateParams
	{
		ShaderProfile shaderProfile;

		const byte* buffer;
		size_t bufferLength;

		String entryPoint;
	};

	class ShaderStage final
	{
	public:
		ShaderStage();

		explicit
		ShaderStage(const ShaderStageCreateParams& a_params);

		~ShaderStage();

		ShaderProfile
		GetShaderProfile() const;

		StringView
		GetEntryPoint() const;

		const Array<byte>&
		GetByteCode() const;

	private:
		ShaderProfile m_shaderProfile = {};
		Array<byte> m_byteCode;
		String m_entryPoint;
	};

	class ShaderCompileResult
	{
		friend class ShaderCompiler;

	public:
		ShaderCompileResult();

		struct Params
		{
			StringView filePath;
			StringView sourceCode;

			Array<ShaderStage> stages;
		};

		explicit
		ShaderCompileResult(const Params& a_params);

		~ShaderCompileResult();

		StringView
		GetFilePath() const;

		StringView
		GetSourceCode() const;

		const Array<ShaderStage>&
		GetShaderStages() const;

	private:
		String m_filePath;
		String m_sourceCode;
		Array<ShaderStage> m_stages;
	};
}
