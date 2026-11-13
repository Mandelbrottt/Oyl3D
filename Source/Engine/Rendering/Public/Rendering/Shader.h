#pragma once

#include <Core/Math/Vector.h>

#include "IDeviceObject.h"
#include "RenderTarget.h"
#include "ShaderCompiler.h"

namespace Oyl
{
	class ICommandBuffer;
	class IUniformBuffer;
	class IDevice;
}

namespace Oyl
{
	struct Vertex
	{
		Vector3f position;
		Vector3f color;
	};

	struct ShaderId : IDeviceObjectId<ShaderId> {};

	struct ShaderCreateParams
	{
		const RenderTarget& renderTarget;

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

		virtual
		void
		SetUniformBuffer(const IDevice& a_device, const IUniformBuffer& a_uniformBuffer) = 0;

		virtual
		void
		BindUniformBuffer(const ICommandBuffer& a_commandBuffer, const IUniformBuffer& a_uniformBuffer) = 0;
	};

	inline
	IShader::~IShader() {}

	using ShaderHandle = IObjectHandle<IShader>;
}
