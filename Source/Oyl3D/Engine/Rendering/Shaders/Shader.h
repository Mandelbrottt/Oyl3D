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

	enum ShaderLanguage
	{
		SL_Hlsl,
	};

	struct ShaderOptions
	{
		ShaderLanguage language;

		enum
		{
			SO_None,
			SO_File,
			SO_Code,
		} source;

		std::string filepath;
		std::string code;
	};

	class ShaderCompiler;

	class OYL_RENDERING_API ShaderResource : public DeviceResource<ShaderResource>
	{
	protected:
		ShaderResource();

		explicit
		ShaderResource(ShaderOptions a_shaderOptions);

	public:
		virtual
		~ShaderResource();

		const ShaderOptions&
		GetShaderOptions() const;

		void
		SetShaderOptions(ShaderOptions a_options);

		ShaderLanguage
		GetLanguage() const
		{
			return m_options.language;
		}

		std::string_view
		GetFilePath() const
		{
			return m_options.filepath;
		}

		std::string_view
		GetCode() const
		{
			return m_options.code;
		}

	protected:
		ShaderOptions m_options;
	};

	using Shader = ResourceHandle<ShaderResource>;
}
