#include "RenderControlModule.h"

#include "Rendering/Renderer/RenderContext.h"
#include "Rendering/Renderer/VulkanRenderContext.h"
#include "Rendering/Window/GlfwWindow.h"

namespace Oyl::Rendering
{
	bool
	RenderControlModule::IsEnabled()
	{
		return !!m_renderContext;
	}

	void
	RenderControlModule::Setup()
	{
		RegisterEventListener(&RenderControlModule::OnWindowCreatedEvent);
	}

	void
	RenderControlModule::Init() {}

	void
	RenderControlModule::Update()
	{
		m_renderContext->Update();
	}

	void
	RenderControlModule::Shutdown()
	{
		if (!m_renderContext)
			return;

		m_renderContext.release();
	}

	void
	RenderControlModule::OnWindowCreatedEvent(const WindowCreatedEvent& a_event)
	{
		// TODO: Check for main window, somehow?
		m_mainWindow = a_event.window;

		m_renderContext = std::make_unique<Internal::VulkanRenderContext>();
		m_resourceManager = std::make_unique<Oyl::Internal::ResourceManager>();

		auto renderContextParams = RenderContextParams {
			.window = m_mainWindow,
			.resourceManager = m_resourceManager.get()
		};
		m_renderContext->Init(renderContextParams);
	}

	void
	RenderControlModule::OnWindowClosedEvent(const WindowClosedEvent& a_event)
	{
		if (m_mainWindow != a_event.window)
			return;

		m_renderContext.release();
		m_resourceManager.release();
	}
}
