#pragma once

#include <Core/Containers/ArrayMap.h>
#include <Core/Containers/SharedPtr.h>
#include <Core/Math/Matrix4.h>

#include "Shader.h"

namespace Oyl
{
	class MaterialDesc;

	using Material = SharedPtr<MaterialDesc>;

	struct MaterialProperty
	{
		TypeId type;
		uint64 offset;
		uint64 size;
	};

	class RENDERING_EXPORT MaterialDesc
	{
	public:
		MaterialDesc() = default;

		NO_COPY(MaterialDesc);
		NO_MOVE(MaterialDesc);

		~MaterialDesc() = default;

		const Shader&
		GetShader() const { return m_shader; }

		void
		SetShader(const Shader& a_shader);

		Matrix4
		GetMatrix(const String& a_name) const;

		bool
		SetMatrix(const String& a_name, const Matrix4& a_matrix);

		const ArrayMap<String, MaterialProperty>&
		GetProperties() const { return m_properties; }

		// TEMPORARY:
		const RdoShaderBindingSetHandle&
		GetShaderBindingSet() const { return m_shaderBindingSet; }

	private:
		Shader m_shader;

		ArrayMap<String, MaterialProperty> m_properties;

		RdoShaderBindingSetHandle m_shaderBindingSet = nullptr;
	};
}
