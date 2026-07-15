#pragma once

#include <Core/Math/Vector.h>
#include <Core/Resources/Resource.h>
#include <Core/Resources/ResourceHandle.h>

namespace Oyl::Rendering
{
	struct Vertex
	{
		Vector2f position;
		Vector3f color;
	};

	class OYL_RENDERING_API ShaderResource : public DeviceResource<ShaderResource>
	{
	protected:
		ShaderResource();

		explicit
		ShaderResource(std::string_view a_filePath);

	public:
		virtual
		~ShaderResource();

		std::string_view
		GetFilePath() const;

		void
		SetFilePath(std::string_view a_filePath);

	protected:
		std::string m_filePath;
	};

	using Shader = ResourceHandle<ShaderResource>;
}
