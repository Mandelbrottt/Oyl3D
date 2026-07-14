#pragma once

#include <Core/Resources/Resource.h>
#include <Core/Resources/ResourceHandle.h>

namespace Oyl::Rendering
{
	class OYL_RENDERING_API ShaderResource : public Resource<ShaderResource>
	{
	public:
		ShaderResource();

		virtual
		~ShaderResource();

		std::string_view
		GetFilePath() const;

		void
		SetFilePath(std::string_view a_filePath);

	private:
		std::string m_filePath;
	};

	using Shader = ResourceHandle<ShaderResource>;
}
