#pragma once

#include <Application/ApplicationWindow.h>

#include "Rendering/Hardware/RenderDevice.h"
#include "Rendering/Hardware/HardwarePresentTarget.h"

namespace Oyl
{
	struct IRenderContext
	{
		virtual
		~IRenderContext() = 0;

		virtual
		void
		Init() = 0;

		virtual
		void
		Update() = 0;

		virtual
		void
		Shutdown() = 0;

		virtual
		RenderingApi
		GetRenderingApi() const = 0;

		virtual
		HardwarePresentTargetHandle
		CreatePresentTarget(IApplicationWindow& a_window) = 0;

		virtual
		RenderDeviceHandle
		CreateDevice(IHardwarePresentTarget* a_presentTarget) = 0;

		virtual
		RdoSwapChainHandle
		CreateSwapChain(
			IRenderDevice& a_device,
			IApplicationWindow& a_window,
			IHardwarePresentTarget& a_presentTarget
		) = 0;

		virtual
		SharedPtr<IShaderCompiler>
		CreateShaderCompiler() = 0;
	};

	inline
	IRenderContext::~IRenderContext() {}
}
