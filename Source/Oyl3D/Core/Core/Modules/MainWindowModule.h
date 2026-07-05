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
		OnWindowKeyPressEvent(const WindowKeyPressEvent& a_event);

		void
		OnWindowKeyReleaseEvent(const WindowKeyReleaseEvent& a_event);

		void
		OnWindowMousePressEvent(const WindowMousePressEvent& a_event);

		void
		OnWindowMouseReleaseEvent(const WindowMouseReleaseEvent& a_event);

		void
		OnWindowMouseScrollEvent(const WindowMouseScrollEvent& a_event);

		void
		OnWindowCursorMoveEvent(const WindowCursorMoveEvent& a_event);

	private:
		Window* m_window;
	};
}
