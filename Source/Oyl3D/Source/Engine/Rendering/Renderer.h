#pragma once

#include <Application/ApplicationWindow.h>

#include <Core/Containers/ArrayMap.h>
#include <Core/Containers/UniquePtr.h>

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
		PresentTargetHandle
		CreatePresentTarget(IApplicationWindow& a_window) = 0;

		virtual
		DeviceHandle
		CreateDevice(IPresentTarget* a_presentTarget) = 0;

		virtual
		SwapChainHandle
		CreateSwapChain(IDevice& a_device, IApplicationWindow& a_window, IPresentTarget& a_presentTarget) = 0;
	};

	inline
	IRenderer::~IRenderer() {}

	struct WindowInfo
	{
		PresentTargetHandle presentTarget = nullptr;
		SwapChainHandle swapChain = nullptr;
	};

	class RENDERING_EXPORT RendererProcessor final
	{
	public:
		RendererProcessor() = default;

		explicit
		RendererProcessor(UniquePtr<IRenderer>&& a_renderer);

		NO_COPY(RendererProcessor);
		DEFAULT_MOVE(RendererProcessor);

		~RendererProcessor();

		void
		Init(IApplicationWindow* a_window);

		void
		Update();

		void
		Shutdown();

		bool
		IsValid() const noexcept { return m_renderer; }

		void
		RegisterApplicationWindow(IApplicationWindow& a_window);

		void
		UnregisterApplicationWindow(IApplicationWindow& a_window);

	private:
		UniquePtr<IRenderer> m_renderer;

		ArrayMap<IApplicationWindow*, WindowInfo> m_windowInfos;
		DeviceHandle m_device = nullptr;
	};
}
