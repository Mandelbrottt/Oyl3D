#pragma once

#include <Core/Math/Vector.h>

#include "RenderDeviceObject.h"

#include "Rendering/RenderTarget.h"
#include "Rendering/ShaderCompiler.h"

namespace Oyl
{
	class IRdoCommandBuffer;
	class IRdoUniformBuffer;
	class IRenderDevice;
}

namespace Oyl
{
	struct Vertex
	{
		Vector3f position;
		Vector3f color;
	};

	struct RdoShaderId : IRenderDeviceObjectId<RdoShaderId> {};

	struct RdoShaderCreateParams
	{
		const RenderTarget& renderTarget;

		ShaderCompileResult compileResult;
	};

	class IRdoShader : public IRenderDeviceObject<RdoShaderId>
	{
	protected:
		IRdoShader() = default;
		DEFAULT_MOVE(IRdoShader);

	public:
		NO_COPY(IRdoShader);

		virtual
		~IRdoShader() = 0;

		virtual
		void
		SetUniformBuffer(const IRenderDevice& a_device, const IRdoUniformBuffer& a_uniformBuffer) = 0;

		virtual
		void
		BindUniformBuffer(const IRdoCommandBuffer& a_commandBuffer, const IRdoUniformBuffer& a_uniformBuffer) = 0;
	};

	inline
	IRdoShader::~IRdoShader() {}

	using RdoShaderHandle = IRenderDeviceObjectHandle<IRdoShader>;
}
