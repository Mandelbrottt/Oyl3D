#pragma once

#include <Application/ApplicationWindow.h>

#include "Device.h"
#include "PresentTarget.h"

namespace Oyl
{
	struct IRenderer
	{
		virtual
		~IRenderer() = 0;

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
		PresentTargetHandle
		CreatePresentTarget(IApplicationWindow& a_window) = 0;

		virtual
		DeviceHandle
		CreateDevice(IPresentTarget* a_presentTarget) = 0;

		virtual
		SwapChainHandle
		CreateSwapChain(IDevice& a_device, IApplicationWindow& a_window, IPresentTarget& a_presentTarget) = 0;

		virtual
		SharedPtr<IShaderCompiler>
		CreateShaderCompiler() = 0;
	};

	inline
	IRenderer::~IRenderer() {}
}
