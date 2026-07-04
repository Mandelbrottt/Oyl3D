#pragma once

#include "Module.h"

#include "Core/Application/Window.h"

namespace Oyl
{
	class MainWindowModule : public Module
	{
		OYL_DECLARE_MODULE(MainWindowModule);

	public:
		void
		OnInit() override;

		void
		OnUpdate() override;

		void
		OnShutdown() override;

		void
		OnWindowResizedEvent(const WindowResizedEvent& a_event);

	private:
		Window m_window;
	};
}
