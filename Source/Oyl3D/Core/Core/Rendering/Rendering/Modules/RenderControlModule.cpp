#include "RenderControlModule.h"

#include "Rendering/Renderer/RenderContext.h"
#include "Rendering/Renderer/VulkanRenderContext.h"
#include "Rendering/Window/GlfwWindow.h"

namespace Oyl::Rendering
{
	void
	RenderControlModule::Setup()
	{
		RegisterEventListener(&RenderControlModule::OnWindowCreatedEvent);

		m_renderContext = new Internal::VulkanRenderContext();
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
		m_renderContext->Destroy();

		delete m_renderContext;
		m_renderContext = nullptr;
	}

	void
	RenderControlModule::OnWindowCreatedEvent(const WindowCreatedEvent& a_event)
	{
		// TODO: Check for main window, somehow?
		m_mainWindow = a_event.window;

		auto renderContextParams = RenderContextParams {
			.window = m_mainWindow
		};
		m_renderContext->Init(renderContextParams);
	}

	void
	RenderControlModule::OnWindowClosedEvent(const WindowClosedEvent& a_event)
	{
		if (m_mainWindow != a_event.window)
			return;

		m_renderContext->Destroy();
	}
}
