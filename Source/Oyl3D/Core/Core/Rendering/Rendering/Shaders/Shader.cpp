#include "Shader.h"

namespace Oyl::Rendering
{
	ShaderResource::ShaderResource()
		: m_dirty(true) {}

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

	bool
	ShaderResource::IsDirty() const
	{
		return m_dirty;
	}

	void
	ShaderResource::SetDirty()
	{
		m_dirty = true;
	}

	bool
	ShaderResource::Compile()
	{
		m_dirty = false;
		return m_dirty;
	}
}
