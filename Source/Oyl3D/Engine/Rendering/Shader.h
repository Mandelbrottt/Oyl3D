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

	struct ShaderHandle : OpaqueHandle<ShaderHandle> {};

	class ShaderImpl : public IDeviceObject<ShaderHandle>
	{
	protected:
		ShaderImpl() = default;

	public:
		struct CreateParams
		{
			const RenderTarget& renderTarget;

			ShaderCompileResult compileResult;
		};

		NO_MOVE(ShaderImpl);
		NO_COPY(ShaderImpl);

		virtual
		~ShaderImpl() = default;
	};

	using Shader = PImpl<ShaderImpl>;
}
