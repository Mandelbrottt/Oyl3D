#pragma once

#include <Core/Resources/ResourceManager.h>

#include <Rendering/Renderer/RenderContext.h>

#include "Core/Modules/Module.h"

#include "Rendering/Common.h"

namespace Oyl
{
	class Window;

	struct WindowClosedEvent;
	struct WindowCreatedEvent;
}

namespace Oyl::Rendering
{
	class RenderContext;

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

	private:
		std::unique_ptr<RenderContext> m_renderContext;
		std::unique_ptr<Internal::ResourceManager> m_resourceManager;

		Window* m_mainWindow = nullptr;
	};
}
