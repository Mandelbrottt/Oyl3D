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
		m_renderEngineInstance->GetRenderContext()->Update();
	}

	void
	RenderControlModule::Shutdown()
	{
		if (!m_renderEngineInstance->GetRenderContext())
			return;

		RenderEngine::SetInstance(nullptr);
		m_renderEngineInstance.release();
	}

	void
	RenderControlModule::OnWindowCreatedEvent(const WindowCreatedEvent& a_event)
	{
		if (m_mainWindow)
			return;

		// TODO: Check for main window, somehow?
		m_mainWindow = a_event.window;

		m_resourceManager = std::make_unique<Oyl::Internal::ResourceManager>();

		m_renderEngineInstance = std::make_unique<Vulkan::RenderEngineInstance>(
			Vulkan::RenderEngineInstance::CreateParams {
				.window = m_mainWindow
			}
		);
		RenderEngine::SetInstance(m_renderEngineInstance.get());
	}

	void
	RenderControlModule::OnWindowClosedEvent(const WindowClosedEvent& a_event)
	{
		if (m_mainWindow != a_event.window)
			return;

		m_renderEngineInstance.release();
		m_resourceManager.release();
	}

	void
	RenderControlModule::OnWindowResizeEvent(const WindowResizeEvent& a_event)
	{
		if (m_mainWindow != a_event.window)
			return;

		auto renderContext = m_renderEngineInstance->GetRenderContext();
		if (!renderContext)
			return;

		renderContext->Resize(a_event.size);
	}

	void
	RenderControlModule::OnWindowMinimizeEvent(const WindowMinimizeEvent& a_event)
	{
		if (m_mainWindow != a_event.window)
			return;

		auto renderContext = m_renderEngineInstance->GetRenderContext();
		if (!renderContext)
			return;

		// Don't need to handle un-minimized case, OnWindowResizeEvent is fired
		if (a_event.minimized)
			renderContext->Resize({ 0, 0 });
	}
}
