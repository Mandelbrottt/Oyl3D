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
		void
		RegisterWindowResizeCallback(Delegate<void(const WindowResizeEvent&)> a_callback) = 0;

		virtual
		void
		UnregisterWindowResizeCallback(Delegate<void(const WindowResizeEvent&)> a_callback) = 0;

		virtual
		void
		RegisterWindowMaximizeCallback(Delegate<void(const WindowMaximizeEvent&)> a_callback) = 0;

		virtual
		void
		UnregisterWindowMaximizeCallback(Delegate<void(const WindowMaximizeEvent&)> a_callback) = 0;

		virtual
		void
		RegisterWindowMinimizeCallback(Delegate<void(const WindowMinimizeEvent&)> a_callback) = 0;

		virtual
		void
		UnregisterWindowMinimizeCallback(Delegate<void(const WindowMinimizeEvent&)> a_callback) = 0;

		virtual
		void
		RegisterWindowMoveCallback(Delegate<void(const WindowMoveEvent&)> a_callback) = 0;

		virtual
		void
		UnregisterWindowMoveCallback(Delegate<void(const WindowMoveEvent&)> a_callback) = 0;

		virtual
		void
		RegisterWindowClosedCallback(Delegate<void(const WindowClosedEvent&)> a_callback) = 0;

		virtual
		void
		UnregisterWindowClosedCallback(Delegate<void(const WindowClosedEvent&)> a_callback) = 0;

		virtual
		void
		RegisterWindowCloseRequestCallback(Delegate<void(const WindowCloseRequestEvent&)> a_callback) = 0;

		virtual
		void
		UnregisterWindowCloseRequestCallback(Delegate<void(const WindowCloseRequestEvent&)> a_callback) = 0;

		virtual
		void
		RegisterWindowFocusCallback(Delegate<void(const WindowFocusEvent&)> a_callback) = 0;

		virtual
		void
		UnregisterWindowFocusCallback(Delegate<void(const WindowFocusEvent&)> a_callback) = 0;

		virtual
		void
		RegisterKeyPressCallback(Delegate<void(const WindowKeyPressEvent&)> a_callback) = 0;

		virtual
		void
		UnregisterKeyPressCallback(Delegate<void(const WindowKeyPressEvent&)> a_callback) = 0;

		virtual
		void
		RegisterKeyReleaseCallback(Delegate<void(const WindowKeyReleaseEvent&)> a_callback) = 0;

		virtual
		void
		UnregisterKeyReleaseCallback(Delegate<void(const WindowKeyReleaseEvent&)> a_callback) = 0;

		virtual
		void
		RegisterMousePressCallback(Delegate<void(const WindowMousePressEvent&)> a_callback) = 0;

		virtual
		void
		UnregisterMousePressCallback(Delegate<void(const WindowMousePressEvent&)> a_callback) = 0;

		virtual
		void
		RegisterMouseReleaseCallback(Delegate<void(const WindowMouseReleaseEvent&)> a_callback) = 0;

		virtual
		void
		UnregisterMouseReleaseCallback(Delegate<void(const WindowMouseReleaseEvent&)> a_callback) = 0;

		virtual
		void
		RegisterMouseScrollCallback(Delegate<void(const WindowMouseScrollEvent&)> a_callback) = 0;

		virtual
		void
		UnregisterMouseScrollCallback(Delegate<void(const WindowMouseScrollEvent&)> a_callback) = 0;

		virtual
		void
		RegisterCursorMoveCallback(Delegate<void(const WindowCursorMoveEvent&)> a_callback) = 0;

		virtual
		void
		UnregisterCursorMoveCallback(Delegate<void(const WindowCursorMoveEvent&)> a_callback) = 0;
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
