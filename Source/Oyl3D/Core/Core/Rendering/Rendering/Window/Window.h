#pragma once

#include "Core/Events/Event.h"
#include "Core/Input/InputCodes.h"
#include "Core/Math/Vector2.h"
#include "Core/Modules/Module.h"

namespace Oyl
{
	enum WindowStateFlags
	{
		WS_None       = 0,
		WS_Fullscreen = 1 << 0,
		WS_Borderless = 1 << 1,
		WS_VSync      = 1 << 2,
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

	struct WindowParams
	{
		Vector2i position = Vector2i(-1, -1);
		Vector2i size;

		std::string title;

		WindowStateFlags windowState;
		CursorState cursorState;

		PostEventDelegate postEventCallback;

		// Requested Default Graphics API
	};

	class OYL_RENDERING_API Window
	{
	protected:
		Window() noexcept;

		explicit
		Window(const WindowParams& a_params) noexcept;

		Window(Window&& a_other) noexcept = default;
		Window&
		operator=(Window&& a_other) noexcept = default;

	public:
		Window(const Window&) = delete;
		virtual
		Window&
		operator=(const Window&) = delete;

		virtual
		~Window() noexcept;

		virtual
		void
		Init(const WindowParams& a_params)
		{
			OYL_UNUSED(a_params);
		}

		virtual
		void
		Destroy() {}

		virtual
		void
		Update() {}

		virtual
		bool
		IsValid() const
		{
			return false;
		}

		virtual
		void
		SetPostEventCallback(PostEventDelegate a_delegate)
		{
			OYL_UNUSED(a_delegate);
		}

		virtual
		Vector2i
		GetSize() const
		{
			return {};
		}

		virtual
		void
		SetSize(Vector2i a_size)
		{
			OYL_UNUSED(a_size);
		}

		virtual
		Vector2i
		GetPosition() const
		{
			return {};
		}

		virtual
		void
		SetPosition(Vector2i a_position)
		{
			OYL_UNUSED(a_position);
		}

		virtual
		std::string_view
		GetTitle() const
		{
			return {};
		}

		virtual
		void
		SetTitle(std::string_view a_title)
		{
			OYL_UNUSED(a_title);
		}

		virtual
		WindowStateFlags
		GetWindowStateFlags() const
		{
			return {};
		}

		virtual
		void
		SetWindowStateFlags(WindowStateFlags a_flags)
		{
			OYL_UNUSED(a_flags);
		}

		virtual
		CursorState
		GetCursorStateFlags() const
		{
			return {};
		}

		virtual
		void
		SetCursorStateFlags(CursorState a_state)
		{
			OYL_UNUSED(a_state);
		}

		virtual
		void*
		GetNativeWindowHandle() const
		{
			return nullptr;
		}
	};

	struct WindowEvent
	{
		Window* window = nullptr;

	protected:
		WindowEvent() = default;
	};

	struct WindowCreatedEvent : WindowEvent, EventBase<WindowCreatedEvent> {};

	struct WindowResizeEvent : WindowEvent, EventBase<WindowResizeEvent>
	{
		Vector2i size;
	};

	struct WindowMoveEvent : WindowEvent, EventBase<WindowMoveEvent>
	{
		Vector2i position;
	};

	struct WindowCloseRequestEvent : WindowEvent, EventBase<WindowCloseRequestEvent> {};

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
