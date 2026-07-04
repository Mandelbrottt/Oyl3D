#pragma once

#include "Core/Events/Event.h"
#include "Core/Math/Vector2.h"

namespace Oyl
{
	enum WindowStateFlags
	{
		WS_None = 0,
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

		// Requested Default Graphics API
	};

	class Window
	{
	public:
		Window();

		explicit
		Window(const WindowParams& a_params);

		Window(const Window&) = delete;
		Window&
		operator=(const Window&) = delete;

		Window(Window&& a_other) noexcept;
		Window&
		operator=(Window&& a_other) noexcept;

		virtual
		~Window();

		void
		Init(const WindowParams& a_params);

		void
		Destroy();

		void
		Update();

		bool
		IsValid() const;

		void
		SetEventCallback(EventDelegate a_delegate);

		Vector2i
		GetSize() const;

		void
		SetSize(Vector2i a_size);

		Vector2i
		GetPosition() const;

		void
		SetPosition(Vector2i a_position);

		std::string_view
		GetTitle() const;

		void
		SetTitle(std::string_view a_title);

		WindowStateFlags
		GetWindowStateFlags() const;

		void
		SetWindowStateFlags(WindowStateFlags a_flags);

		CursorState
		GetCursorStateFlags() const;

		void
		SetCursorStateFlags(CursorState a_state);

		void*
		GetNativeWindowHandle() const;

	private:
		struct Impl;
		Impl* m_impl;
	};

	struct WindowResizeEvent : Event
	{
		OYL_DECLARE_EVENT();

		Vector2i size;
	};

	struct WindowMoveEvent : Event
	{
		OYL_DECLARE_EVENT();

		Vector2i position;
	};

	struct WindowCloseRequestEvent : Event
	{
		OYL_DECLARE_EVENT();
	};

	struct WindowFocusEvent : Event
	{
		OYL_DECLARE_EVENT();

		bool focused;
	};

	struct WindowCursorMoveEvent : Event
	{
		OYL_DECLARE_EVENT();

		Vector2f position;
	};
}
