#include "RenderControlModule.h"

#include "MainWindowModule.h"

#include "Rendering/RenderEngine.h"

namespace Oyl
{
	bool
	RenderControlModule::IsEnabled()
	{
		return !!m_renderEngineInstance;
	}

	void
	RenderControlModule::Setup()
	{
		RegisterEventListener(&RenderControlModule::OnWindowClosedEvent);
		RegisterEventListener(&RenderControlModule::OnWindowResizeEvent);
		RegisterEventListener(&RenderControlModule::OnWindowMinimizeEvent);
	}

	void
	RenderControlModule::Init()
	{
		auto* mainWindowModule = GetModule<MainWindowModule>();
		if (!mainWindowModule || !mainWindowModule->GetWindow())
			return;

		m_mainWindow = mainWindowModule->GetWindow();

		m_resourceManager = UniquePtr<Internal::ResourceManager>::Create();

		m_renderEngineInstance = RenderEngine::CreateInstance(Rendering::GraphicsApi::Vulkan, { .window = m_mainWindow });
		RenderEngine::SetCurrentInstance(&m_renderEngineInstance);

		m_renderer = UniquePtr<Rendering::Renderer>::Create(
			m_renderEngineInstance->GetCurrentDevice(),
			m_renderEngineInstance->GetCurrentSwapChain()
		);

		m_testRenderPass = UniquePtr<Rendering::TestRenderPass>::Create();
		m_renderer->GetRenderGraph().AddRenderPass(m_testRenderPass.Get());
	}

	void
	RenderControlModule::Update()
	{
		OYL_PROFILE_FUNCTION();

		if (!m_renderEngineInstance)
			return;

		m_renderer->Render();
	}

	void
	RenderControlModule::Shutdown()
	{
		OYL_PROFILE_FUNCTION();

		RenderEngine::SetCurrentInstance(nullptr);
		m_renderEngineInstance.Reset();
	}

	void
	RenderControlModule::OnWindowClosedEvent(const WindowClosedEvent& a_event)
	{
		if (m_mainWindow != a_event.window)
			return;

		OYL_PROFILE_FUNCTION();

		m_renderer.Reset();
		m_testRenderPass.Reset();
		m_renderEngineInstance.Reset();
		m_resourceManager.Reset();
	}

	void
	RenderControlModule::OnWindowResizeEvent(const WindowResizeEvent& a_event)
	{
		if (m_mainWindow != a_event.window)
			return;

		auto& swapChain = m_renderEngineInstance->GetCurrentSwapChain();
		if (!swapChain)
			return;

		OYL_PROFILE_FUNCTION();

		swapChain.Recreate();
		m_testRenderPass->OnWindowResizedEvent(a_event);
	}

	void
	RenderControlModule::OnWindowMinimizeEvent(const WindowMinimizeEvent& a_event)
	{
		if (m_mainWindow != a_event.window)
			return;

		auto& swapChain = m_renderEngineInstance->GetCurrentSwapChain();
		if (!swapChain)
			return;

		OYL_PROFILE_FUNCTION();

		// Don't need to handle un-minimized case, OnWindowResizeEvent is fired
		if (a_event.minimized)
			swapChain.Recreate();
	}
}
