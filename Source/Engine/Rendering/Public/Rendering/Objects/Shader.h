#pragma once

#include <filesystem>

#include <Core/Containers/SharedPtr.h>
#include <Core/Containers/String.h>

#include "Rendering/RenderingEnums.h"

namespace Oyl
{
	class ShaderDesc;

	using Shader = SharedPtr<ShaderDesc>;

	class RENDERING_EXPORT ShaderDesc
	{
	public:
		ShaderDesc() = default;

		NO_COPY(ShaderDesc);
		NO_MOVE(ShaderDesc);

		explicit
		ShaderDesc(StringView a_path)
		{
			SetPath(a_path);
		}

		~ShaderDesc() = default;

		const String&
		GetPath() const { return m_path; }

		void
		SetPath(StringView a_path, bool a_detectLanguage = true)
		{
			m_path = String(a_path);

			if (a_detectLanguage)
			{
				auto extension = std::filesystem::path(std::string(a_path)).extension();
				if (extension == ".hlsl")
				{
					SetLanguage(ShaderLanguage::Hlsl);
				}
			}
		}

		ShaderLanguage
		GetLanguage() const { return m_language; }

		void
		SetLanguage(ShaderLanguage a_language) { m_language = a_language; }

		ShaderProfileFlags
		GetShaderProfiles() const { return m_profiles; }

	private:
		String m_path;

		ShaderLanguage m_language = ShaderLanguage::None;
		ShaderProfileFlags m_profiles = ShaderProfileFlagBits::None;
	};
}
