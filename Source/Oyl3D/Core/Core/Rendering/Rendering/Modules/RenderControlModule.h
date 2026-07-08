#pragma once

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
		RenderContext* m_renderContext = nullptr;

		Window* m_mainWindow = nullptr;
	};
}
