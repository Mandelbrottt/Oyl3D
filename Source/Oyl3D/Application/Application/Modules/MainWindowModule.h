#pragma once

#include "Application/Common.h"

#include "Core/Modules/Module.h"

#include "Rendering/Window/Window.h"

namespace Oyl
{
	class OYL_APPLICATION_API MainWindowModule : public Module
	{
		OYL_DECLARE_MODULE(MainWindowModule);

	public:
		void
		Setup() override;

		void
		Init() override;

		void
		Update() override;

		void
		Shutdown() override;

		void
		OnWindowResizeEvent(const WindowResizeEvent& a_event);

		void
		OnWindowMoveEvent(const WindowMoveEvent& a_event);

		void
		OnWindowClosedEvent(const WindowClosedEvent& a_event);

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
