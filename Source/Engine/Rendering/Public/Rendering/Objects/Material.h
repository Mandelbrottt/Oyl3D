#pragma once

#include <Core/Containers/SharedPtr.h>

#include "Shader.h"

namespace Oyl
{
	class MaterialDesc;

	using Material = SharedPtr<MaterialDesc>;

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
		SetShader(const Shader& a_shader) { m_shader = a_shader; }

	private:
		Shader m_shader;
	};
}
