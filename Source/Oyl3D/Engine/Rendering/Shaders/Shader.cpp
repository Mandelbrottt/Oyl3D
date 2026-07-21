#include "Shader.h"

namespace Oyl::Rendering
{
	ShaderResource::ShaderResource()
		: m_options({
			.source = ShaderOptions::SO_None
		}) {}

	ShaderResource::ShaderResource(ShaderOptions a_shaderOptions)
	{
		m_options = std::move(a_shaderOptions);
	}

	ShaderResource::~ShaderResource() {}

	const ShaderOptions&
	ShaderResource::GetShaderOptions() const
	{
		return m_options;
	}

	void
	ShaderResource::SetShaderOptions(ShaderOptions a_options)
	{
		m_options = std::move(a_options);
		SetDirty();
	}
}
