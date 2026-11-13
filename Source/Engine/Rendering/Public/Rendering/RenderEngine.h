#pragma once

#include <Core/Preprocessor/Common.h>

#include "Rendering/Hardware/HardwarePresentTarget.h"
#include "Rendering/Hardware/RenderDevice.h"

namespace Oyl
{
	class RENDERING_EXPORT RenderEngine
	{
		friend class Renderer;

		RenderEngine();
		~RenderEngine();

		NO_COPY(RenderEngine);
		DEFAULT_MOVE(RenderEngine);

	public:
		static
		RenderEngine&
		Get();

		IRenderDevice&
		GetDevice() const;

		IShaderCompiler&
		GetShaderCompiler() const;

	private:
		void
		RegisterDevice(const RenderDeviceHandle& a_device);

		void
		UnregisterDevice(const RenderDeviceHandle& a_device);

		void
		RegisterShaderCompiler(const SharedPtr<IShaderCompiler>& a_shaderCompiler);

		void
		UnregisterShaderCompiler(const SharedPtr<IShaderCompiler>& a_shaderCompiler);

	private:
		struct Impl;
		Impl* m_impl;
	};
}
