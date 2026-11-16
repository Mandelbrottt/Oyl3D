#pragma once

#include <Core/Containers/Array.h>
#include <Core/Containers/String.h>
#include <Core/Types/TypeId.h>

#include "RenderingEnums.h"

namespace Oyl
{
	class ShaderCompileResult;

	class RENDERING_EXPORT IShaderCompiler
	{
	protected:
		IShaderCompiler() = default;

	public:
		virtual
		~IShaderCompiler() = default;

		DEFAULT_COPY(IShaderCompiler);
		DEFAULT_MOVE(IShaderCompiler);

		bool
		CompileHlslFromFile(StringView a_filePath, ShaderCompileResult* a_outShader) const;

		bool
		CompileHlslFromSource(StringView a_source, ShaderCompileResult* a_outShader) const;

	protected:
		virtual
		bool
		CompileHlsl(StringView a_source, ShaderCompileResult* a_outShader) const = 0;
	};

	struct ShaderInputBinding
	{
		String name;
		uint location;
		TypeId type;
		uint size;
		uint offset;
	};

	struct ShaderBufferBinding
	{
		String name;
		uint binding;

		struct BufferMember
		{
			String name;
			TypeId type;

			uint stride;
			uint offset;
		};

		Array<BufferMember> members;
	};

	struct ShaderStageCreateParams
	{
		ShaderProfileFlagBits shaderProfile;

		const byte* buffer;
		size_t bufferLength;

		String entryPoint;

		Array<ShaderInputBinding> inputBindings;
		Array<ShaderBufferBinding> bufferBindings;
	};

	class RENDERING_EXPORT ShaderStage final
	{
	public:
		ShaderStage();

		explicit
		ShaderStage(const ShaderStageCreateParams& a_params);

		~ShaderStage();

		ShaderProfileFlagBits
		GetShaderProfile() const { return m_shaderProfile; }

		StringView
		GetEntryPoint() const { return m_entryPoint; }

		const Array<byte>&
		GetByteCode() const { return m_byteCode; }

		const Array<ShaderInputBinding>&
		GetInputBindings() const { return m_inputBindings; }

		const Array<ShaderBufferBinding>&
		GetBufferBindings() const { return m_bufferBindings; }

	private:
		ShaderProfileFlagBits m_shaderProfile = {};
		Array<byte> m_byteCode;
		String m_entryPoint;

		Array<ShaderInputBinding> m_inputBindings;
		Array<ShaderBufferBinding> m_bufferBindings;
	};

	class RENDERING_EXPORT ShaderCompileResult
	{
		friend class IShaderCompiler;

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
		GetFilePath() const { return m_filePath; }

		StringView
		GetSourceCode() const { return m_sourceCode; }

		const Array<ShaderStage>&
		GetShaderStages() const { return m_stages; }

	private:
		String m_filePath;
		String m_sourceCode;
		Array<ShaderStage> m_stages;
	};
}
