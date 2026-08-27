#pragma once

#include "Core/Modules/Module.h"
#include "Core/Resources/ResourceManager.h"

#include "Rendering/RenderEngine.h"
#include "Rendering/Renderer.h"
#include "Rendering/RenderTarget.h"
#include "Rendering/TestRenderPass.h"
#include "Rendering/Window.h"

namespace Oyl
{
	class IWindow;

	struct WindowClosedEvent;
	struct WindowCreatedEvent;
}

namespace Oyl
{
	class RenderControlModule : public Module
	{
		OYL_DECLARE_MODULE(RenderControlModule);

	public:
		bool
		IsEnabled() override;

		void
		Setup() override;

		void
		Init() override;

		void
		Update() override;

		void
		Shutdown() override;

	private:
		void
		OnWindowClosedEvent(const WindowClosedEvent& a_event);

		void
		OnWindowResizeEvent(const WindowResizeEvent& a_event);

		void
		OnWindowMinimizeEvent(const WindowMinimizeEvent& a_event);

	private:
		UniquePtr<Internal::ResourceManager> m_resourceManager;

		UniquePtr<Internal::RenderEngineInstance> m_renderEngineInstance;

		UniquePtr<Rendering::Renderer> m_renderer;
		UniquePtr<Rendering::TestRenderPass> m_testRenderPass;

		const IWindow* m_mainWindow = nullptr;
	};
}
