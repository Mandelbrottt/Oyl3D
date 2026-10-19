#include "Renderer.h"

#include <Core/Logging/Logging.h>
#include <Core/Profiling/Profiling.h>

namespace Oyl
{
	RendererProcessor::RendererProcessor(UniquePtr<IRenderer>&& a_renderer)
		: m_renderer(std::move(a_renderer)) {}

	RendererProcessor::~RendererProcessor()
	{
		Shutdown();
	}

	void
	RendererProcessor::Init(IApplicationWindow* a_mainWindow)
	{
		OYL_PROFILE_FUNCTION();

		if (!IsValid())
			return;

		m_renderer->Init();

		PresentTargetHandle presentTarget = nullptr;
		if (a_mainWindow)
		{
			RegisterApplicationWindow(*a_mainWindow);
			presentTarget = m_windowInfos[a_mainWindow].presentTarget;
		}

		LogTrace("Creating Device");
		m_device = m_renderer->CreateDevice(presentTarget.Get());
	}

	void
	RendererProcessor::Update()
	{
		OYL_PROFILE_FUNCTION();

		if (!IsValid())
			return;

		if (m_renderer)
			m_renderer->Update();

		for (auto& [window, info] : m_windowInfos)
		{
			if (!info.swapChain || !info.swapChain->IsValid())
			{
				LogTrace("Creating SwapChain for window [{} \"{}\"]", window->GetNativeWindowHandle(), window->GetTitle());
				info.swapChain = m_renderer->CreateSwapChain(m_device, *window, info.presentTarget);
			}
		}
	}

	void
	RendererProcessor::Shutdown()
	{
		OYL_PROFILE_FUNCTION();

		if (!IsValid())
			return;

		for (auto& [window, info] : m_windowInfos)
		{
			UnregisterApplicationWindow(*window);
		}

		if (m_renderer)
			m_renderer->Shutdown();

		m_windowInfos.Clear();
		m_device.Reset();
		m_renderer.Reset();
	}

	void
	RendererProcessor::RegisterApplicationWindow(IApplicationWindow& a_window)
	{
		OYL_PROFILE_FUNCTION();

		if (!IsValid())
			return;

		if (!a_window.IsValid())
			return;

		if (m_windowInfos.Contains(&a_window))
			return;

		LogDebug("Registering window [{} \"{}\"] with RendererProcessor", a_window.GetNativeWindowHandle(), a_window.GetTitle());

		WindowInfo info;
		LogTrace("Creating PresentTarget for window [{} \"{}\"]", a_window.GetNativeWindowHandle(), a_window.GetTitle());
		info.presentTarget = m_renderer->CreatePresentTarget(a_window);

		m_windowInfos.Emplace(&a_window, std::move(info));
	}

	void
	RendererProcessor::UnregisterApplicationWindow(IApplicationWindow& a_window)
	{
		OYL_PROFILE_FUNCTION();

		if (!IsValid())
			return;

		if (!m_windowInfos.Contains(&a_window))
			return;

		LogDebug("Unregistering window [{} {}] with RendererProcessor", a_window.GetNativeWindowHandle(), a_window.GetTitle());

		m_device->WaitUntilIdle();

		auto& [presentTarget, swapChain] = m_windowInfos[&a_window];
		swapChain->Destroy();
		presentTarget->Destroy();
		m_windowInfos.Erase(&a_window);
	}
}
