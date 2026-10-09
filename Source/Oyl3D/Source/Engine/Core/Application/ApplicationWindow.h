#pragma once

#include "Core/Containers/String.h"
#include "Core/Events/Event.h"
#include "Core/Math/Vector.h"

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

	struct ApplicationWindowCreateParams
	{
		Vector2i position = Vector2i(-1, -1);
		Vector2u size;

		String title;

		WindowStateFlags windowState;
		CursorState cursorState;

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
}
