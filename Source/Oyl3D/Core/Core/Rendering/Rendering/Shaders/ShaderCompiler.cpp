#include "ShaderCompiler.h"

namespace
{
	std::string_view
	GetShaderFragmentSubstringInSource(std::string_view a_source);

	bool
	ReplaceFragmentWithPixelInSource(std::string_view a_sourceView, std::string* a_tempSourceBuffer);
}

namespace Oyl::Rendering
{
	ShaderCompiler::ShaderCompiler() {}

	ShaderCompiler::~ShaderCompiler() {}

	bool
	ShaderCompiler::CompileHlslFromFile(std::string_view a_filePath, ShaderCompileResult* a_outShader) const
	{
		std::ifstream inStream;
		inStream.open(a_filePath);

		if (!inStream)
			return false;

		ShaderCompileResult outShader(
			{
				.filePath = std::filesystem::absolute(a_filePath).string()
			}
		);

		// Reserve memory for string
		std::string content;
		inStream.seekg(0, std::ios::end);
		size_t size = inStream.tellg();
		content.reserve(size); //
		inStream.seekg(0, std::ios::beg);

		// Read file into string
		std::getline(inStream, content, '\0');

		bool success = CompileHlslFromSource(content, &outShader);
		if (!success)
			return false;

		*a_outShader = std::move(outShader);
		return true;
	}

	bool
	ShaderCompiler::CompileHlslFromSource(std::string_view a_source, ShaderCompileResult* a_outShader) const
	{
		// TODO: Logging
		if (!a_outShader)
			return false;

		auto compileStringView = a_source;

		// string buffer to hold modified source until we're done compiling
		// This way, a_source isn't invalidated
		std::string pixelShaderAttributeSource;
		if (ReplaceFragmentWithPixelInSource(a_source, &pixelShaderAttributeSource))
			compileStringView = pixelShaderAttributeSource;

		// Implementation defined CompileHlsl
		ShaderCompileResult outShader;
		bool success = CompileHlsl(compileStringView, &outShader);
		if (!success)
			return false;

		// Ensure the source code we're sending out is the original
		outShader.m_filePath = a_outShader->m_filePath;
		outShader.m_sourceCode = a_source;
		*a_outShader = std::move(outShader);
		return true;
	}

	ShaderStage::ShaderStage() {}

	ShaderStage::ShaderStage(const Params& a_params)
	{
		m_shaderProfile = a_params.shaderProfile;

		if (a_params.buffer && a_params.bufferLength > 0)
			m_byteCode.insert(m_byteCode.end(), a_params.buffer, a_params.buffer + a_params.bufferLength);

		m_entryPoint = a_params.entryPoint;
	}

	ShaderStage::~ShaderStage() {}

	ShaderProfile
	ShaderStage::GetShaderProfile() const
	{
		return m_shaderProfile;
	}

	std::string_view
	ShaderStage::GetEntryPoint() const
	{
		return m_entryPoint;
	}

	const std::vector<byte>&
	ShaderStage::GetByteCode() const
	{
		return m_byteCode;
	}

	ShaderCompileResult::ShaderCompileResult() {}

	ShaderCompileResult::ShaderCompileResult(const Params& a_params)
	{
		m_filePath = a_params.filePath;
		m_sourceCode = a_params.sourceCode;

		m_stages = std::move(a_params.stages);
	}

	ShaderCompileResult::~ShaderCompileResult() {}

	std::string_view
	ShaderCompileResult::GetFilePath() const
	{
		return m_filePath;
	}

	std::string_view
	ShaderCompileResult::GetSourceCode() const
	{
		return m_sourceCode;
	}

	const std::vector<ShaderStage>&
	ShaderCompileResult::GetShaderStages() const
	{
		return m_stages;
	}
}

namespace
{
	std::string_view
	GetShaderFragmentSubstringInSource(std::string_view a_source)
	{
		size_t cursorIndex = 0;
		while (cursorIndex != std::string::npos)
		{
			// return true if next non-space character and start of a_token have the same index
			// set *a_pIndex to that index
			auto isNextNonSpaceTokenEqual = [&](
				const char* a_token,
				size_t* a_pIndex
			) -> bool
			{
				auto it = std::find_if_not(
					a_source.begin() + *a_pIndex,
					a_source.end(),
					[](unsigned char a_c) { return std::isspace(a_c); }
				);

				size_t tokenIndex = a_source.find(a_token, *a_pIndex);
				if (tokenIndex == std::string::npos)
					return false;

				auto tokenIt = a_source.begin() + tokenIndex;
				if (it != tokenIt)
					return false;

				*a_pIndex = tokenIndex + strlen(a_token);
				return true;
			};

			// Search for string [shader("fragment")]
			// equivalent to regex /\[\s*shader\s*\(\s*"fragment"\s*\)\s*\]/
			cursorIndex = a_source.find('[', cursorIndex);
			if (cursorIndex == std::string::npos)
				break;

			auto attributeStartIndex = cursorIndex++;
			auto currentTokenIndex = attributeStartIndex + 1; // Account for length of '['

			if (!isNextNonSpaceTokenEqual("shader", &currentTokenIndex))
				continue;

			if (!isNextNonSpaceTokenEqual("(", &currentTokenIndex))
				continue;

			if (!isNextNonSpaceTokenEqual("\"fragment\"", &currentTokenIndex))
				continue;

			if (!isNextNonSpaceTokenEqual(")", &currentTokenIndex))
				continue;

			if (!isNextNonSpaceTokenEqual("]", &currentTokenIndex))
				continue;

			// substring [shader("fragment")] within the source code
			return std::string_view(a_source.data() + attributeStartIndex, currentTokenIndex - attributeStartIndex);
		}

		return {};
	}

	bool
	ReplaceFragmentWithPixelInSource(std::string_view a_sourceView, std::string* a_outTempSourceBuffer)
	{
		OYL_ASSERT(a_outTempSourceBuffer);

		if (a_sourceView.empty())
			return false;

		auto fragmentShaderAttributeSubstring = GetShaderFragmentSubstringInSource(a_sourceView);
		if (fragmentShaderAttributeSubstring.empty())
			return false;

		// Set up source buffer
		a_outTempSourceBuffer->clear();
		a_outTempSourceBuffer->reserve(a_sourceView.length());

		// Replace [shader("fragment")] with [shader("pixel")]
		a_outTempSourceBuffer->insert(
			a_outTempSourceBuffer->end(),
			a_sourceView.data(),
			fragmentShaderAttributeSubstring.data()
		);
		a_outTempSourceBuffer->append("[shader(\"pixel\")]");
		a_outTempSourceBuffer->insert(
			a_outTempSourceBuffer->end(),
			fragmentShaderAttributeSubstring.data() + fragmentShaderAttributeSubstring.length(),
			a_sourceView.data() + a_sourceView.length()
		);

		return true;
	}
}
