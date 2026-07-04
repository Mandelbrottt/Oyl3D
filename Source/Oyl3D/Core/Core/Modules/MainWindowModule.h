#pragma once

#include "Module.h"

#include "Core/Application/Window.h"

#include "Core/Reflection/Attribute.h"

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
		OnWindowResizeEvent(const WindowResizeEvent& a_event);

		void
		OnWindowMoveEvent(const WindowMoveEvent& a_event);

		void
		OnWindowCloseRequestEvent(const WindowCloseRequestEvent& a_event);

		void
		OnWindowFocusEvent(const WindowFocusEvent& a_event);

		void
		OnWindowCursorMoveEvent(const WindowCursorMoveEvent& a_event);

	private:
		Window m_window;
	};
}
