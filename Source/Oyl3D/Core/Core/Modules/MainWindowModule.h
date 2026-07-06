#pragma once

#include "Module.h"

#include "Core/Application/Window.h"

#include "Core/Reflection/Attribute.h"

namespace Oyl
{
	class OYL_CORE_API MainWindowModule : public Module
	{
		OYL_DECLARE_MODULE(MainWindowModule);

	public:
		void
		OnInit() override;

		void
		OnUpdate() override;

		void
		OnShutdown() override;

		bool
		OnWindowResizeEvent(const WindowResizeEvent& a_event);

		bool
		OnWindowMoveEvent(const WindowMoveEvent& a_event);

		bool
		OnWindowCloseRequestEvent(const WindowCloseRequestEvent& a_event);

		bool
		OnWindowFocusEvent(const WindowFocusEvent& a_event);

		bool
		OnWindowKeyPressEvent(const WindowKeyPressEvent& a_event);

		bool
		OnWindowKeyReleaseEvent(const WindowKeyReleaseEvent& a_event);

		bool
		OnWindowMousePressEvent(const WindowMousePressEvent& a_event);

		bool
		OnWindowMouseReleaseEvent(const WindowMouseReleaseEvent& a_event);

		bool
		OnWindowMouseScrollEvent(const WindowMouseScrollEvent& a_event);

		bool
		OnWindowCursorMoveEvent(const WindowCursorMoveEvent& a_event);

	private:
		Window* m_window;
	};
}
