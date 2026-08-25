#include "RenderControlModule.h"

#include "Rendering/RenderContext.h"
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
		RegisterEventListener(&RenderControlModule::OnWindowCreatedEvent);
		RegisterEventListener(&RenderControlModule::OnWindowClosedEvent);
		RegisterEventListener(&RenderControlModule::OnWindowResizeEvent);
		RegisterEventListener(&RenderControlModule::OnWindowMinimizeEvent);
	}

	void
	RenderControlModule::Init() {}

	void
	RenderControlModule::Update()
	{
		OYL_PROFILE_FUNCTION();

		if (!m_renderEngineInstance)
			return;

		m_renderEngineInstance->GetRenderContext()->Update();
		m_renderer->Render();
	}

	void
	RenderControlModule::Shutdown()
	{
		if (!m_renderEngineInstance->GetRenderContext())
			return;

		OYL_PROFILE_FUNCTION();

		RenderEngine::SetCurrentInstance(nullptr);
		m_renderEngineInstance.Reset();
	}

	void
	RenderControlModule::OnWindowCreatedEvent(const WindowCreatedEvent& a_event)
	{
		if (m_mainWindow)
			return;

		OYL_PROFILE_FUNCTION();

		// TODO: Check for main window, somehow?
		m_mainWindow = a_event.window;

		m_resourceManager = std::make_unique<Internal::ResourceManager>();

		m_renderEngineInstance = RenderEngine::CreateInstance(Rendering::GraphicsApi::Vulkan, { .window = m_mainWindow });
		RenderEngine::SetCurrentInstance(&m_renderEngineInstance);

		m_renderer = std::make_unique<Rendering::Renderer>(*m_renderEngineInstance->GetRenderContext());

		m_testRenderPass = std::make_unique<Rendering::TestRenderPass>();
		m_renderer->GetRenderGraph().AddRenderPass(m_testRenderPass.get());
	}

	void
	RenderControlModule::OnWindowClosedEvent(const WindowClosedEvent& a_event)
	{
		if (m_mainWindow != a_event.window)
			return;

		OYL_PROFILE_FUNCTION();

		m_renderer.reset();
		m_testRenderPass.reset();
		m_renderEngineInstance.Reset();
		m_resourceManager.reset();
	}

	void
	RenderControlModule::OnWindowResizeEvent(const WindowResizeEvent& a_event)
	{
		if (m_mainWindow != a_event.window)
			return;

		auto renderContext = m_renderEngineInstance->GetRenderContext();
		if (!renderContext)
			return;

		OYL_PROFILE_FUNCTION();

		renderContext->Resize(a_event.size);
		m_testRenderPass->OnWindowResizedEvent(a_event);
	}

	void
	RenderControlModule::OnWindowMinimizeEvent(const WindowMinimizeEvent& a_event)
	{
		if (m_mainWindow != a_event.window)
			return;

		auto renderContext = m_renderEngineInstance->GetRenderContext();
		if (!renderContext)
			return;

		OYL_PROFILE_FUNCTION();

		// Don't need to handle un-minimized case, OnWindowResizeEvent is fired
		if (a_event.minimized)
			renderContext->Resize({ 0, 0 });
	}
}
