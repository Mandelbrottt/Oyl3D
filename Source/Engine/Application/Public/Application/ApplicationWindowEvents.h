#pragma once

#include <Core/Events/Event.h>
#include <Core/Input/InputCodes.h>
#include <Core/Math/Vector.h>

namespace Oyl
{
	class IApplicationWindow;

	struct WindowEvent
	{
		IApplicationWindow* window = nullptr;

	protected:
		WindowEvent() = default;
	};

	/**
	 * \brief Sent after a window is created, and is fully valid
	 */
	struct WindowCreatedEvent : WindowEvent, EventBase<WindowCreatedEvent> {};

	/**
	 * \brief Sent while window is being destroyed, before the window is freed in the underlying windowing API
	 */
	struct WindowClosedEvent : WindowEvent, EventBase<WindowClosedEvent> {};

	struct WindowCloseRequestEvent : WindowEvent, EventBase<WindowCloseRequestEvent> {};

	struct WindowResizeEvent : WindowEvent, EventBase<WindowResizeEvent>
	{
		Vector2u size;
	};

	struct WindowMaximizeEvent : WindowEvent, EventBase<WindowMaximizeEvent>
	{
		bool maximized;
	};

	struct WindowMinimizeEvent : WindowEvent, EventBase<WindowMinimizeEvent>
	{
		bool minimized;
	};

	struct WindowMoveEvent : WindowEvent, EventBase<WindowMoveEvent>
	{
		Vector2i position;
	};

	struct WindowFocusEvent : WindowEvent, EventBase<WindowFocusEvent>
	{
		bool focused;
	};

	struct WindowCursorMoveEvent : WindowEvent, EventBase<WindowCursorMoveEvent>
	{
		Vector2f position;
	};

	struct WindowKeyPressEvent : WindowEvent, EventBase<WindowKeyPressEvent>
	{
		Input::KeyboardKey key;
		int mods;
	};

	struct WindowKeyReleaseEvent : WindowEvent, EventBase<WindowKeyReleaseEvent>
	{
		Input::KeyboardKey key;
		int mods;
	};

	struct WindowMousePressEvent : WindowEvent, EventBase<WindowMousePressEvent>
	{
		Input::MouseButton button;
		int mods;
	};

	struct WindowMouseReleaseEvent : WindowEvent, EventBase<WindowMouseReleaseEvent>
	{
		Input::MouseButton button;
		int mods;
	};

	struct WindowMouseScrollEvent : WindowEvent, EventBase<WindowMouseScrollEvent>
	{
		Vector2f scroll;
	};
}
