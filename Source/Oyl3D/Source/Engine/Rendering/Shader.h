#pragma once

#include <Core/Math/Vector.h>

#include "IDeviceObject.h"
#include "RenderTarget.h"
#include "ShaderCompiler.h"

namespace Oyl::Rendering
{
	struct Vertex
	{
		Vector2f position;
		Vector3f color;
	};

	struct ShaderId : IDeviceObjectId<ShaderId> {};

	struct ShaderCreateParams
	{
		const RenderTarget& renderTarget;

		ShaderCompileResult compileResult;
	};

	class Shader : public IDeviceObject<ShaderId>
	{
	protected:
		Shader() = default;
		DEFAULT_MOVE(Shader);

	public:
		NO_COPY(Shader);

		virtual
		~Shader() = 0;
	};

	inline
	Shader::~Shader() {}
}
