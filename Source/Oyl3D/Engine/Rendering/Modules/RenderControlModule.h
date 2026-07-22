#pragma once

#include "Core/Modules/Module.h"
#include "Core/Resources/ResourceManager.h"

#include "Rendering/RenderContext.h"
#include "Rendering/RenderEngineInstance.h"

namespace Oyl
{
	class Window;

	struct WindowClosedEvent;
	struct WindowCreatedEvent;
}

namespace Oyl::Rendering
{
	class OYL_RENDERING_API RenderControlModule : public Module
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
		OnWindowCreatedEvent(const WindowCreatedEvent& a_event);

		void
		OnWindowClosedEvent(const WindowClosedEvent& a_event);

		void
		OnWindowResizeEvent(const WindowResizeEvent& a_event);

		void
		OnWindowMinimizeEvent(const WindowMinimizeEvent& a_event);

	private:
		std::unique_ptr<Oyl::Internal::ResourceManager> m_resourceManager;

		std::unique_ptr<Internal::RenderEngineInstance> m_renderEngineInstance;

		Window* m_mainWindow = nullptr;
	};
}
