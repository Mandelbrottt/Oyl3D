#include "Shader.h"

namespace Oyl::Rendering
{
	ShaderResource::ShaderResource() {}

	ShaderResource::ShaderResource(std::string_view a_filePath, ShaderCompiler* a_compiler)
		: m_filePath(a_filePath),
		  m_shaderCompiler(a_compiler) {}

	ShaderResource::~ShaderResource() {}

	std::string_view
	ShaderResource::GetFilePath() const
	{
		return m_filePath;
	}

	void
	ShaderResource::SetFilePath(std::string_view a_filePath)
	{
		m_filePath = a_filePath;
		SetDirty();
	}
}
