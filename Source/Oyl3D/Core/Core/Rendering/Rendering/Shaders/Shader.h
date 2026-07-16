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

	enum ShaderProfile
	{
		SP_Vertex,
		SP_Geometry,
		SP_Fragment,

		SP_Count
	};

	class ShaderCompiler;

	class OYL_RENDERING_API ShaderResource : public DeviceResource<ShaderResource>
	{
	protected:
		ShaderResource();

		explicit
		ShaderResource(std::string_view a_filePath, ShaderCompiler* a_compiler);

	public:
		virtual
		~ShaderResource();

		std::string_view
		GetFilePath() const;

		void
		SetFilePath(std::string_view a_filePath);

	protected:
		std::string m_filePath;
		ShaderCompiler* m_shaderCompiler = nullptr;
	};

	using Shader = ResourceHandle<ShaderResource>;
}
