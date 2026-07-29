#pragma once

#include <Core/Math/Vector.h>

#include "DeviceObject.h"

namespace Oyl::Rendering
{
	struct Vertex
	{
		Vector2f position;
		Vector3f color;
	};

	struct ShaderHandle : OpaqueHandle<ShaderHandle> {};

	class Shader : public IDeviceObject<ShaderHandle>
	{
	protected:
		Shader() = default;

		DEFAULT_MOVE(Shader);

	public:
		NO_COPY(Shader);

		virtual
		~Shader() = default;
	};
}
