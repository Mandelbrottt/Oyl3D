#pragma once

#include "Core/PPExport.h"
#include "Core/Containers/String.h"
#include "Core/Events/Event.h"
#include "Core/Input/InputCodes.h"
#include "Core/Math/Vector.h"
#include "Core/Types/EnumFlags.h"

namespace Oyl
{
	enum class WindowStateFlagBits : uint32
	{
		None = 0,
		Fullscreen = 1 << 0,
		Borderless = 1 << 1,
		VSync = 1 << 2,
	};

	OYL_ENUM_CLASS_BITWISE_OPERATIONS(WindowStateFlagBits)

	using WindowStateFlags = EnumFlags<WindowStateFlagBits>;

	enum class CursorState : uint32
	{
		Begin,
		Normal = Begin,
		Hidden,
		Disabled,
		Locked,
		Last,
	};

	struct ApplicationWindowCreateParams
	{
		String title;

		Vector2u size = Vector2u::Zero;
		Vector2i position = Vector2i(-1);

		WindowStateFlags windowState = WindowStateFlagBits::None;
		CursorState cursorState = CursorState::Normal;

		PostEventDelegate postEventCallback;

		// Requested Default Graphics API
	};

	class IApplicationWindow
	{
	public:
		virtual
		~IApplicationWindow() noexcept = 0;

		virtual
		void
		Destroy() = 0;

		virtual
		bool
		IsValid() const = 0;

		virtual
		void
		Update() = 0;

		virtual
		Vector2u
		GetSize() const = 0;

		virtual
		void
		SetSize(Vector2u a_size) = 0;

		virtual
		Vector2u
		GetContentSize() const = 0;

		virtual
		Vector2i
		GetPosition() const = 0;

		virtual
		void
		SetPosition(Vector2i a_position) = 0;

		virtual
		StringView
		GetTitle() const = 0;

		virtual
		void
		SetTitle(StringView a_title) = 0;

		virtual
		WindowStateFlags
		GetWindowState() const = 0;

		virtual
		void
		SetWindowState(WindowStateFlags a_flags) = 0;

		virtual
		CursorState
		GetCursorState() const = 0;

		virtual
		void
		SetCursorState(CursorState a_state) = 0;

		virtual
		void*
		GetNativeWindowHandle() const = 0;
	};

	inline
	IApplicationWindow::~IApplicationWindow() noexcept {}

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
