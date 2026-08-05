#pragma once

#include <Core/Math/Vector.h>

#include "DeviceObject.h"
#include "RenderTarget.h"
#include "ShaderCompiler.h"

namespace Oyl::Rendering
{
	struct Vertex
	{
		Vector2f position;
		Vector3f color;
	};

	struct ShaderId : OpaqueHandle<ShaderId> {};

	class Shader : public IDeviceObject<ShaderId>
	{
	protected:
		Shader() = default;

	public:
		struct CreateParams
		{
			const RenderTarget& renderTarget;

			ShaderCompileResult compileResult;
		};

		NO_MOVE(Shader);
		NO_COPY(Shader);

		virtual
		~Shader() = default;
	};

	using ShaderHandle = PImpl<Shader>;
}
