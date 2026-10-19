#pragma once

#include <Core/Math/Vector.h>

#include "IDeviceObject.h"
#include "RenderTarget.h"
#include "ShaderCompiler.h"

namespace Oyl
{
	struct Vertex
	{
		Vector2f position;
		Vector3f color;
	};

	struct ShaderId : IDeviceObjectId<ShaderId> {};

	struct ShaderCreateParams
	{
		const IRenderTarget& renderTarget;

		ShaderCompileResult compileResult;
	};

	class IShader : public IDeviceObject<ShaderId>
	{
	protected:
		IShader() = default;
		DEFAULT_MOVE(IShader);

	public:
		NO_COPY(IShader);

		virtual
		~IShader() = 0;
	};

	inline
	IShader::~IShader() {}
}
