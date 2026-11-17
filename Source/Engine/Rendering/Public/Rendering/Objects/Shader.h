#pragma once

#include <filesystem>

#include <Core/Containers/SharedPtr.h>
#include <Core/Containers/String.h>

#include "Rendering/RenderEngine.h"
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
			if (!RenderEngine::Get().GetShaderCompiler().CompileHlslFromFile(a_path, &m_compileResult))
			{
				return;
			}

			for (const auto& stage : m_compileResult.GetShaderStages())
				for (const auto& bufferBinding : stage.GetBufferBindings())
					for (const auto& member : bufferBinding.members)
						m_shaderProperties[member.name] = member.type;

			SetPath(a_path);
		}

		~ShaderDesc() = default;

		const String&
		GetPath() const { return m_path; }

		ShaderLanguage
		GetLanguage() const { return m_language; }

		ShaderProfileFlags
		GetShaderProfiles() const { return m_profiles; }

		const ShaderCompileResult&
		GetCompileResult() const { return m_compileResult; }

		Array<ShaderBufferBinding>
		GetBufferBindings() const
		{
			Array<ShaderBufferBinding> result;
			for (const auto& stage : m_compileResult.GetShaderStages())
				for (const auto& bufferBinding : stage.GetBufferBindings())
					result.Add(bufferBinding);
			return result;
		}

		TypeId
		GetShaderPropertyType(const String& a_name) const
		{
			if (!m_shaderProperties.ContainsKey(a_name))
				return TypeId::Null;
			return m_shaderProperties[a_name];
		}

	private:
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

		void
		SetLanguage(ShaderLanguage a_language) { m_language = a_language; }

	private:
		String m_path;

		ShaderLanguage m_language = ShaderLanguage::None;
		ShaderProfileFlags m_profiles = ShaderProfileFlagBits::None;

		ShaderCompileResult m_compileResult;

		ArrayMap<String, TypeId> m_shaderProperties;
	};
}
