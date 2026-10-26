#pragma once

#include <Core/Containers/Delegate.h>
#include <Core/Containers/String.h>
#include <Core/Events/Event.h>
#include <Core/Input/InputCodes.h>
#include <Core/Math/Vector.h>
#include <Core/Types/EnumFlags.h>

#include "Application/ApplicationWindowEvents.h"

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

		// Requested Default Graphics API
	};

	class IApplicationWindow
	{
	public:
		virtual
		~IApplicationWindow() noexcept = 0;

		virtual
		void
		Close() = 0;

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

		virtual
		const MulticastDelegate<void(const WindowResizeEvent&)>&
		OnResizeCallback() const = 0;

		virtual
		const MulticastDelegate<void(const WindowMaximizeEvent&)>&
		OnMaximizeCallback() const = 0;

		virtual
		const MulticastDelegate<void(const WindowMinimizeEvent&)>&
		OnMinimizeCallback() const = 0;

		virtual
		const MulticastDelegate<void(const WindowMoveEvent&)>&
		OnMoveCallback() const = 0;

		virtual
		const MulticastDelegate<void(const WindowCloseRequestEvent&)>&
		OnCloseRequestCallback() const = 0;

		virtual
		const MulticastDelegate<void(const WindowCloseEvent&)>&
		OnCloseCallback() const = 0;

		virtual
		const MulticastDelegate<void(const WindowFocusEvent&)>&
		OnFocusCallback() const = 0;

		virtual
		const MulticastDelegate<void(const WindowKeyPressEvent&)>&
		OnKeyPressCallback() const = 0;

		virtual
		const MulticastDelegate<void(const WindowKeyReleaseEvent&)>&
		OnKeyReleaseCallback() const = 0;

		virtual
		const MulticastDelegate<void(const WindowMousePressEvent&)>&
		OnMousePressCallback() const = 0;

		virtual
		const MulticastDelegate<void(const WindowMouseReleaseEvent&)>&
		OnMouseReleaseCallback() const = 0;

		virtual
		const MulticastDelegate<void(const WindowMouseScrollEvent&)>&
		OnMouseScrollCallback() const = 0;

		virtual
		const MulticastDelegate<void(const WindowCursorMoveEvent&)>&
		OnCursorMoveCallback() const = 0;
	};

	inline
	IApplicationWindow::~IApplicationWindow() noexcept {}

	class IApplicationWindowFactory
	{
	public:
		virtual
		~IApplicationWindowFactory() = 0;

		virtual
		IApplicationWindow*
		CreateApplicationWindow(const ApplicationWindowCreateParams& a_params) = 0;
	};

	inline
	IApplicationWindowFactory::~IApplicationWindowFactory() {}
}
