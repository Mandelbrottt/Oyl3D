#include "RenderControlModule.h"

#include "Rendering/RenderContext.h"
#include "Rendering/RenderEngine.h"
#include "Rendering/Glfw/GlfwWindow.h"
#include "Rendering/Vulkan/VulkanRenderContext.h"
#include "Rendering/Vulkan/VulkanRenderEngineInstance.h"

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
		RegisterEventListener(&RenderControlModule::OnWindowClosedEvent);
		RegisterEventListener(&RenderControlModule::OnWindowResizeEvent);
		RegisterEventListener(&RenderControlModule::OnWindowMinimizeEvent);
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

		m_resourceManager = std::make_unique<Internal::ResourceManager>();

		m_renderEngineInstance = std::make_unique<Vulkan::RenderEngineInstance>(m_resourceManager.get());
		RenderEngine::SetInstance(m_renderEngineInstance.get());

		m_renderContext = std::make_unique<Vulkan::RenderContext>();

		auto renderContextParams = RenderContextParams {
			.window = m_mainWindow,
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

	void
	RenderControlModule::OnWindowResizeEvent(const WindowResizeEvent& a_event)
	{
		if (m_mainWindow != a_event.window)
			return;

		if (!m_renderContext)
			return;

		m_renderContext->Resize(a_event.size);
	}

	void
	RenderControlModule::OnWindowMinimizeEvent(const WindowMinimizeEvent& a_event)
	{
		if (m_mainWindow != a_event.window)
			return;

		if (!m_renderContext)
			return;

		// Don't need to handle un-minimized case, OnWindowResizeEvent is fired
		if (a_event.minimized)
			m_renderContext->Resize({ 0, 0 });
	}
}
