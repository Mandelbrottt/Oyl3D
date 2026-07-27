#pragma once

#include <Core/Math/Vector.h>
#include <Core/Resources/ResourceHandle.h>

#include "Rendering/DeviceResource.h"

namespace Oyl::Rendering
{
	enum ShaderProfile
	{
		SP_Vertex,
		SP_Geometry,
		SP_Fragment,

		SP_Count
	};

	enum ShaderLanguage
	{
		SL_None,
		SL_Hlsl,
	};

	struct ShaderOptions
	{
		ShaderLanguage language = SL_None;

		enum
		{
			SO_None,
			SO_File,
			SO_Code,
		} source = SO_None;

		std::string filepath = {};
		std::string code = {};
	};

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
}

namespace Oyl
{
	using Shader = ResourceHandle<Rendering::ShaderResource>;
}
