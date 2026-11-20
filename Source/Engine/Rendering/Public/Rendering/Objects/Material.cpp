#include "Material.h"

namespace Oyl
{
	void
	MaterialDesc::SetShader(const Shader& a_shader)
	{
		m_shader = a_shader;
		m_properties.Clear();

		auto bufferBindings = m_shader->GetBufferBindings();
		m_shaderBindingSet = RenderEngine::Get().GetDevice().CreateShaderBindingSet({
			.bindings = bufferBindings
		});

		uint bufferOffset = 0;
		for (const auto& bufferBinding : bufferBindings)
		{
			for (const auto& member : bufferBinding.members)
			{
				m_properties[member.name] = {
					.type = member.type,
					.offset = bufferOffset + member.offset,
					.size = member.stride,
				};
			}
			bufferOffset += bufferBinding.stride;
		}
	}

	Matrix4
	MaterialDesc::GetMatrix(const String& a_name) const
	{
		if (!m_properties.ContainsKey(a_name))
			return {};

		auto& property = m_properties[a_name];
		if (property.type != typeidof(Matrix4))
			return {};

		Matrix4 result;
		void* pSrc = (void*) ((uintptr_t) m_shaderBindingSet->GetUnderlyingBuffer() + property.offset);
		std::memcpy(&result, pSrc, property.size);
		return result;
	}

	bool
	MaterialDesc::SetMatrix(const String& a_name, const Matrix4& a_matrix)
	{
		if (!m_shader)
			return false;

		if (!m_properties.ContainsKey(a_name))
		{
			LogError("Trying to SetMatrix on material value \"{}\" that doesn't exist!");
			return false;
		}

		auto& property = m_properties[a_name];
		if (property.type != typeidof(Matrix4))
		{
			LogError("Trying to SetMatrix on material value \"{}\" that isn't a matrix!");
			return false;
		}

		m_shaderBindingSet->CopyToBuffer(&a_matrix, property.offset);
		return true;
	}
}
