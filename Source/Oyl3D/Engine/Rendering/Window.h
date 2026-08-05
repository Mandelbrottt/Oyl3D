#pragma once

#include <Core/UniqueHandle.h>
#include <Core/Events/Event.h>
#include <Core/Input/InputCodes.h>
#include <Core/Math/Vector2.h>

namespace Oyl
{
	enum WindowStateFlags
	{
		WS_None = 0,
		WS_Fullscreen = 1 << 0,
		WS_Borderless = 1 << 1,
		WS_VSync = 1 << 2,
	};

	enum CursorState
	{
		CS_Start,
		CS_Normal = CS_Start,
		CS_Hidden,
		CS_Disabled,
		CS_Locked,
		CS_Last,
	};

	class IWindow : public IUniqueHandle
	{
	protected:
		IWindow() noexcept = default;

		IWindow(IWindow&& a_other) noexcept = default;
		IWindow&
		operator=(IWindow&& a_other) noexcept = default;

	public:
		virtual
		~IWindow() noexcept = default;

		void
		Destroy() override = 0;

		bool
		IsValid() const override = 0;

		virtual
		void
		Update() = 0;

		virtual
		void
		SetPostEventCallback(PostEventDelegate a_delegate) = 0;

		virtual
		Vector2u
		GetSize() const = 0;

		virtual
		void
		SetSize(Vector2u a_size) = 0;

		virtual
		Vector2u
		GetFrameBufferSize() const = 0;

		virtual
		Vector2i
		GetPosition() const = 0;

		virtual
		void
		SetPosition(Vector2i a_position) = 0;

		virtual
		std::string_view
		GetTitle() const = 0;

		virtual
		void
		SetTitle(std::string_view a_title) = 0;

		virtual
		WindowStateFlags
		GetWindowStateFlags() const = 0;

		virtual
		void
		SetWindowStateFlags(WindowStateFlags a_flags) = 0;

		virtual
		CursorState
		GetCursorStateFlags() const = 0;

		virtual
		void
		SetCursorStateFlags(CursorState a_state) = 0;

		virtual
		void*
		GetNativeWindowHandle() const = 0;
	};

	struct WindowEvent
	{
		IWindow* window = nullptr;

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
