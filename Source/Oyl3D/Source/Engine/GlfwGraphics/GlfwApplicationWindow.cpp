#include "GlfwApplicationWindow.h"

#include <Core/Profiling/Profiling.h>
#include <Core/Input/InputCodes.h>

namespace
{
	Oyl::Input::KeyboardKey
	GetKeyCodeFromGlfw(int a_key);

	Oyl::Input::MouseButton
	GetMouseButtonFromGlfw(int a_button);

	Oyl::Input::GamePadButton
	GetGamePadButtonFromGlfw(int a_button);

	Oyl::Input::GamePadAxis
	GetGamePadAxisFromGlfw(int a_axis);
}

namespace Oyl
{
	GlfwApplicationWindow::GlfwApplicationWindow(const ApplicationWindowCreateParams& a_params)
		: m_title(a_params.title),
		  m_position(a_params.position),
		  m_size(a_params.size),
		  m_windowState(a_params.windowState),
		  m_cursorState(a_params.cursorState),
		  m_postEventCallback(a_params.postEventCallback)
	{
		OYL_PROFILE_FUNCTION();

		if (!m_postEventCallback)
			m_postEventCallback = PostEventDelegate::Create([](const Event&) {});

		CreateGlfwWindow();
		SetupGlfwWindowCallbacks();
	}

	GlfwApplicationWindow::~GlfwApplicationWindow()
	{
		GlfwApplicationWindow::Destroy();
	}

	void
	GlfwApplicationWindow::Destroy()
	{
		OYL_PROFILE_FUNCTION();

		if (!IsValid())
			return;

		// Send event before we do any actual cleanup
		WindowClosedEvent event;
		event.window = this;
		m_postEventCallback(event);

		glfwDestroyWindow(m_glfwWindow);
		m_glfwWindow = nullptr;
	}

	bool
	GlfwApplicationWindow::IsValid() const
	{
		return m_glfwWindow;
	}

	void
	GlfwApplicationWindow::Update()
	{
		glfwPollEvents();
	}

	Vector2u
	GlfwApplicationWindow::GetSize() const
	{
		return m_size;
	}

	void
	GlfwApplicationWindow::SetSize(Vector2u a_size)
	{
		auto monitor = glfwGetWindowMonitor(m_glfwWindow);
		auto mode = glfwGetVideoMode(monitor);

		glfwSetWindowMonitor(
			m_glfwWindow,
			monitor,
			m_position.x,
			m_position.y,
			a_size.x,
			a_size.y,
			mode->refreshRate
		);

		m_size = a_size;
	}

	Vector2u
	GlfwApplicationWindow::GetContentSize() const
	{
		Vector2i size;
		glfwGetFramebufferSize(m_glfwWindow, &size.x, &size.y);

		return Vector2u(size.x, size.y);
	}

	Vector2i
	GlfwApplicationWindow::GetPosition() const
	{
		return m_position;
	}

	void
	GlfwApplicationWindow::SetPosition(Vector2i a_position)
	{
		auto monitor = glfwGetWindowMonitor(m_glfwWindow);
		auto mode = glfwGetVideoMode(monitor);

		glfwSetWindowMonitor(
			m_glfwWindow,
			monitor,
			a_position.x,
			a_position.y,
			m_size.x,
			m_size.y,
			mode->refreshRate
		);

		m_position = a_position;
	}

	StringView
	GlfwApplicationWindow::GetTitle() const
	{
		return m_title;
	}

	void
	GlfwApplicationWindow::SetTitle(StringView a_title)
	{
		m_title = String(a_title);
		glfwSetWindowTitle(m_glfwWindow, m_title.c_str());
	}

	WindowStateFlags
	GlfwApplicationWindow::GetWindowState() const
	{
		return m_windowState;
	}

	void
	GlfwApplicationWindow::SetWindowState(WindowStateFlags a_flags)
	{
		if (m_windowState == a_flags)
		{
			return;
		}

		OYL_PROFILE_FUNCTION();

		Vector2u desiredSize;

		// get resolution of monitor
		const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());

		if (a_flags & (WindowStateFlagBits::Fullscreen | WindowStateFlagBits::Borderless))
		{
			// backup window position and window size
			glfwGetWindowPos(m_glfwWindow, &m_position.x, &m_position.y);
			Vector2i tempSize;
			glfwGetWindowSize(m_glfwWindow, &tempSize.x, &tempSize.y);
			m_size = Vector2u(tempSize.x, tempSize.y);

			// switch to full screen
			glfwSetWindowMonitor(
				m_glfwWindow,
				glfwGetWindowMonitor(m_glfwWindow),
				0,
				0,
				mode->width,
				mode->height,
				mode->refreshRate
			);

			desiredSize = { (uint32) mode->width, (uint32) mode->height };
		} else if (a_flags ^ (WindowStateFlagBits::Fullscreen | WindowStateFlagBits::Borderless))
		{
			glfwSetWindowMonitor(
				m_glfwWindow,
				nullptr,
				m_position.x,
				m_position.y,
				m_size.x,
				m_size.y,
				mode->refreshRate
			);
			desiredSize = m_size;
		}

		// Set Context Size
		OYL_UNUSED(desiredSize);
		m_windowState = a_flags;
	}

	CursorState
	GlfwApplicationWindow::GetCursorState() const
	{
		return m_cursorState;
	}

	void
	GlfwApplicationWindow::SetCursorState(CursorState a_state)
	{
		if (m_cursorState == a_state)
		{
			return;
		}

		auto to_index = [](CursorState a_inState)
		{
			return static_cast<Traits::TUnderlyingType<CursorState>>(a_inState);
		};

		int table[to_index(CursorState::Last)];
		table[to_index(CursorState::Normal)] = GLFW_CURSOR_NORMAL;
		table[to_index(CursorState::Hidden)] = GLFW_CURSOR_HIDDEN;
		table[to_index(CursorState::Disabled)] = GLFW_CURSOR_DISABLED;
		table[to_index(CursorState::Locked)] = GLFW_CURSOR_CAPTURED;

		uint glfwState = table[to_index(a_state)];
		glfwSetInputMode(m_glfwWindow, GLFW_CURSOR, glfwState);

		m_cursorState = a_state;
	}

	void*
	GlfwApplicationWindow::GetNativeWindowHandle() const
	{
		return m_glfwWindow;
	}

	void
	GlfwApplicationWindow::CreateGlfwWindow()
	{
		OYL_PROFILE_FUNCTION();

		auto monitor = glfwGetPrimaryMonitor();
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		m_glfwWindow = glfwCreateWindow(
			m_size.x,
			m_size.y,
			m_title.c_str(),
			m_windowState & WindowStateFlagBits::Fullscreen ? monitor : nullptr,
			nullptr
		);

		auto* mode = glfwGetVideoMode(monitor);
		m_position.x = mode->width / 2 - m_size.x / 2;
		m_position.y = mode->height / 2 - m_size.y / 2;

		if (m_windowState ^ WindowStateFlagBits::Fullscreen)
		{
			glfwSetWindowMonitor(
				m_glfwWindow,
				nullptr,
				m_position.x,
				m_position.y,
				m_size.x,
				m_size.y,
				mode->refreshRate
			);
		}

		glfwSetWindowUserPointer(m_glfwWindow, this);
	}

	void
	GlfwApplicationWindow::SetupGlfwWindowCallbacks()
	{
		OYL_PROFILE_FUNCTION();

		glfwSetWindowCloseCallback(
			m_glfwWindow,
			[](GLFWwindow* a_window)
			{
				auto* this_ = reinterpret_cast<decltype(this)>(glfwGetWindowUserPointer(a_window));

				if (!this_->m_postEventCallback)
					return;

				// The glfw callback is only a request - we still control destruction of the window
				WindowCloseRequestEvent event;
				event.window = this_;
				this_->m_postEventCallback(event);
			}
		);

		glfwSetWindowSizeCallback(
			m_glfwWindow,
			[](GLFWwindow* a_window, int a_width, int a_height)
			{
				if (a_width <= 0 || a_height <= 0)
					return;

				auto* this_ = reinterpret_cast<decltype(this)>(glfwGetWindowUserPointer(a_window));
				this_->m_size = Vector2u(a_width, a_height);

				if (!this_->m_postEventCallback)
					return;

				WindowResizeEvent event;
				event.window = this_;
				event.size = this_->m_size;
				this_->m_postEventCallback(event);
			}
		);

		glfwSetWindowMaximizeCallback(
			m_glfwWindow,
			[](GLFWwindow* a_window, int a_maximised)
			{
				auto* this_ = reinterpret_cast<decltype(this)>(glfwGetWindowUserPointer(a_window));

				if (!this_->m_postEventCallback)
					return;

				WindowMaximizeEvent event;
				event.window = this_;
				event.maximized = a_maximised;
				this_->m_postEventCallback(event);
			}
		);

		glfwSetWindowIconifyCallback(
			m_glfwWindow,
			[](GLFWwindow* a_window, int a_minimized)
			{
				auto* this_ = reinterpret_cast<decltype(this)>(glfwGetWindowUserPointer(a_window));

				if (!this_->m_postEventCallback)
					return;

				WindowMinimizeEvent event;
				event.window = this_;
				event.minimized = a_minimized;
				this_->m_postEventCallback(event);
			}
		);

		glfwSetWindowPosCallback(
			m_glfwWindow,
			[](GLFWwindow* a_window, int a_posX, int a_posY)
			{
				if (a_posX <= 0 || a_posY <= 0)
					return;

				auto* this_ = reinterpret_cast<decltype(this)>(glfwGetWindowUserPointer(a_window));
				this_->m_position = { a_posX, a_posY };

				if (!this_->m_postEventCallback)
					return;

				WindowMoveEvent event;
				event.window = this_;
				event.position = this_->m_position;
				this_->m_postEventCallback(event);
			}
		);

		glfwSetWindowFocusCallback(
			m_glfwWindow,
			[](GLFWwindow* a_window, int a_focused)
			{
				auto* this_ = reinterpret_cast<decltype(this)>(glfwGetWindowUserPointer(a_window));

				if (!this_->m_postEventCallback)
					return;

				WindowFocusEvent event;
				event.window = this_;
				event.focused = a_focused;
				this_->m_postEventCallback(event);
			}
		);

		glfwSetKeyCallback(
			m_glfwWindow,
			[](GLFWwindow* a_window, int a_key, int a_scanCode, int a_action, int a_mods)
			{
				OYL_UNUSED(a_scanCode);

				auto* this_ = reinterpret_cast<decltype(this)>(glfwGetWindowUserPointer(a_window));

				if (!this_->m_postEventCallback)
					return;

				Input::KeyboardKey key = GetKeyCodeFromGlfw(a_key);

				switch (a_action)
				{
					case GLFW_PRESS:
					{
						WindowKeyPressEvent event;
						event.window = this_;
						event.key = key;
						event.mods = a_mods;
						this_->m_postEventCallback(event);
						break;
					}
					case GLFW_RELEASE:
					{
						WindowKeyReleaseEvent event;
						event.window = this_;
						event.key = key;
						event.mods = a_mods;
						this_->m_postEventCallback(event);
						break;
					}
					default:
						break;
				}
			}
		);

		glfwSetMouseButtonCallback(
			m_glfwWindow,
			[](GLFWwindow* a_window, int a_button, int a_action, int a_mods)
			{
				auto* this_ = reinterpret_cast<decltype(this)>(glfwGetWindowUserPointer(a_window));

				if (!this_->m_postEventCallback)
					return;

				Input::MouseButton button = GetMouseButtonFromGlfw(a_button);

				switch (a_action)
				{
					case GLFW_PRESS:
					{
						WindowMousePressEvent event;
						event.window = this_;
						event.button = button;
						event.mods = a_mods;
						this_->m_postEventCallback(event);
						break;
					}
					case GLFW_RELEASE:
					{
						WindowMouseReleaseEvent event;
						event.window = this_;
						event.button = button;
						event.mods = a_mods;
						this_->m_postEventCallback(event);
						break;
					}
					default:
						break;
				}
			}
		);

		glfwSetScrollCallback(
			m_glfwWindow,
			[](GLFWwindow* a_window, double a_scrollX, double a_scrollY)
			{
				auto* this_ = reinterpret_cast<decltype(this)>(glfwGetWindowUserPointer(a_window));

				if (!this_->m_postEventCallback)
					return;

				WindowMouseScrollEvent event;
				event.window = this_;
				event.scroll = { (float32) a_scrollX, (float32) a_scrollY };
				this_->m_postEventCallback(event);
			}
		);

		glfwSetCursorPosCallback(
			m_glfwWindow,
			[](GLFWwindow* a_window, double a_posX, double a_posY)
			{
				auto* this_ = reinterpret_cast<decltype(this)>(glfwGetWindowUserPointer(a_window));

				if (!this_->m_postEventCallback)
					return;

				WindowCursorMoveEvent event;
				event.window = this_;
				event.position = { (float32) a_posX, (float32) a_posY };
				this_->m_postEventCallback(event);
			}
		);
	}
}

namespace
{
	Oyl::Input::KeyboardKey
	GetKeyCodeFromGlfw(int a_key)
	{
		using Oyl::Input::KeyboardKey;
		using enum KeyboardKey;

		using KeyArray = std::array<KeyboardKey, GLFW_KEY_LAST + 1>;

		static constexpr KeyArray table = []() -> KeyArray
		{
			KeyArray result { static_cast<KeyboardKey>(-1) };

			auto setRangeInTable = [](
				KeyArray& a_table,
				int a_inputRangeBegin,
				int a_inputRangeEnd,
				KeyboardKey a_outputRangeBegin
			)
			{
				auto inputRangeSize = a_inputRangeEnd - a_inputRangeBegin;
				for (int i = 0; i < inputRangeSize; i++)
				{
					int tableIndex = a_inputRangeBegin + i;
					int newKey = static_cast<int>(a_outputRangeBegin) + i;
					a_table[tableIndex] = static_cast<KeyboardKey>(newKey);
				}
			};

			// The following ranges are defined in the same order in GLFW and our API, shortcut
			setRangeInTable(result, GLFW_KEY_0, GLFW_KEY_9, KB_NumbersFirst);
			setRangeInTable(result, GLFW_KEY_A, GLFW_KEY_Z, KB_LettersFirst);
			setRangeInTable(result, GLFW_KEY_F1, GLFW_KEY_F25, KB_FunctionKeysFirst);
			setRangeInTable(result, GLFW_KEY_KP_0, GLFW_KEY_KP_9, KB_Numpad0);

			result[GLFW_KEY_SPACE] = KB_Space;
			result[GLFW_KEY_APOSTROPHE] = KB_Apostrophe;
			result[GLFW_KEY_COMMA] = KB_Comma;
			result[GLFW_KEY_MINUS] = KB_Minus;
			result[GLFW_KEY_PERIOD] = KB_Period;
			result[GLFW_KEY_SLASH] = KB_Slash;
			result[GLFW_KEY_SEMICOLON] = KB_Semicolon;
			result[GLFW_KEY_EQUAL] = KB_Equal;

			result[GLFW_KEY_LEFT_BRACKET] = KB_LeftBracket;
			result[GLFW_KEY_BACKSLASH] = KB_Backslash;
			result[GLFW_KEY_RIGHT_BRACKET] = KB_RightBracket;
			result[GLFW_KEY_GRAVE_ACCENT] = KB_Grave;

			result[GLFW_KEY_ESCAPE] = KB_Escape;
			result[GLFW_KEY_ENTER] = KB_Enter;
			result[GLFW_KEY_TAB] = KB_Tab;
			result[GLFW_KEY_BACKSPACE] = KB_Backspace;
			result[GLFW_KEY_INSERT] = KB_Insert;
			result[GLFW_KEY_DELETE] = KB_Delete;
			result[GLFW_KEY_RIGHT] = KB_Right;
			result[GLFW_KEY_LEFT] = KB_Left;
			result[GLFW_KEY_DOWN] = KB_Down;
			result[GLFW_KEY_UP] = KB_Up;
			result[GLFW_KEY_PAGE_UP] = KB_PageUp;
			result[GLFW_KEY_PAGE_DOWN] = KB_PageDown;
			result[GLFW_KEY_HOME] = KB_Home;
			result[GLFW_KEY_END] = KB_End;
			result[GLFW_KEY_CAPS_LOCK] = KB_CapsLock;
			result[GLFW_KEY_SCROLL_LOCK] = KB_ScrollLock;
			result[GLFW_KEY_NUM_LOCK] = KB_NumLock;
			result[GLFW_KEY_PRINT_SCREEN] = KB_PrintScreen;
			result[GLFW_KEY_PAUSE] = KB_PauseBreak;

			result[GLFW_KEY_KP_DECIMAL] = KB_NumpadDecimal;
			result[GLFW_KEY_KP_DIVIDE] = KB_NumpadDivide;
			result[GLFW_KEY_KP_MULTIPLY] = KB_NumpadMultiply;
			result[GLFW_KEY_KP_SUBTRACT] = KB_NumpadSubtract;
			result[GLFW_KEY_KP_ADD] = KB_NumpadAdd;
			result[GLFW_KEY_KP_ENTER] = KB_NumpadEnter;
			result[GLFW_KEY_LEFT_SHIFT] = KB_LeftShift;
			result[GLFW_KEY_LEFT_CONTROL] = KB_LeftCtrl;
			result[GLFW_KEY_LEFT_ALT] = KB_LeftAlt;
			result[GLFW_KEY_LEFT_SUPER] = KB_LeftSuper;
			result[GLFW_KEY_RIGHT_SHIFT] = KB_RightShift;
			result[GLFW_KEY_RIGHT_CONTROL] = KB_RightCtrl;
			result[GLFW_KEY_RIGHT_ALT] = KB_RightAlt;
			result[GLFW_KEY_RIGHT_SUPER] = KB_RightSuper;
			result[GLFW_KEY_MENU] = KB_Menu;

			return result;
		}();

		return table[a_key];
	}

	Oyl::Input::MouseButton
	GetMouseButtonFromGlfw(int a_button)
	{
		using Oyl::Input::MouseButton;
		using enum MouseButton;

		using ButtonArray = std::array<MouseButton, GLFW_MOUSE_BUTTON_LAST + 1>;

		static constexpr ButtonArray table = []() -> ButtonArray
		{
			ButtonArray result { static_cast<MouseButton>(-1) };

			for (int i = 0; i < GLFW_MOUSE_BUTTON_LAST - GLFW_MOUSE_BUTTON_1; i++)
			{
				int button = MB_Button1 + i;
				result[GLFW_MOUSE_BUTTON_1 + i] = static_cast<MouseButton>(button);
			}

			return result;
		}();

		return table[a_button];
	}

	[[maybe_unused]]
	Oyl::Input::GamePadButton
	GetGamePadButtonFromGlfw(int a_button)
	{
		using Oyl::Input::GamePadButton;
		using enum GamePadButton;

		using ButtonArray = std::array<GamePadButton, GLFW_GAMEPAD_BUTTON_LAST + 1>;

		static constexpr ButtonArray table = []() -> ButtonArray
		{
			ButtonArray result { static_cast<GamePadButton>(-1) };

			result[GLFW_GAMEPAD_BUTTON_A] = GP_South;
			result[GLFW_GAMEPAD_BUTTON_B] = GP_East;
			result[GLFW_GAMEPAD_BUTTON_X] = GP_West;
			result[GLFW_GAMEPAD_BUTTON_Y] = GP_North;
			result[GLFW_GAMEPAD_BUTTON_LEFT_BUMPER] = GP_LeftShoulder;
			result[GLFW_GAMEPAD_BUTTON_RIGHT_BUMPER] = GP_RightShoulder;
			result[GLFW_GAMEPAD_BUTTON_BACK] = GP_Back;
			result[GLFW_GAMEPAD_BUTTON_START] = GP_Start;
			result[GLFW_GAMEPAD_BUTTON_GUIDE] = GP_Home;
			result[GLFW_GAMEPAD_BUTTON_LEFT_THUMB] = GP_LeftStickButton;
			result[GLFW_GAMEPAD_BUTTON_RIGHT_THUMB] = GP_RightStickButton;
			result[GLFW_GAMEPAD_BUTTON_DPAD_UP] = GP_DPadUp;
			result[GLFW_GAMEPAD_BUTTON_DPAD_RIGHT] = GP_DPadRight;
			result[GLFW_GAMEPAD_BUTTON_DPAD_DOWN] = GP_DPadDown;
			result[GLFW_GAMEPAD_BUTTON_DPAD_LEFT] = GP_DPadLeft;

			return result;
		}();

		return table[a_button];
	}

	[[maybe_unused]]
	Oyl::Input::GamePadAxis
	GetGamePadAxisFromGlfw(int a_axis)
	{
		using Oyl::Input::GamePadAxis;
		using enum GamePadAxis;

		using ButtonArray = std::array<GamePadAxis, GLFW_GAMEPAD_AXIS_LAST + 1>;

		static constexpr ButtonArray table = []() -> ButtonArray
		{
			ButtonArray result { static_cast<GamePadAxis>(-1) };

			// We provide stick input as a vector to the user, so both stick x and y map to the same code
			result[GLFW_GAMEPAD_AXIS_LEFT_X] = GP_LeftStick;
			result[GLFW_GAMEPAD_AXIS_LEFT_Y] = GP_LeftStick;
			result[GLFW_GAMEPAD_AXIS_RIGHT_X] = GP_RightStick;
			result[GLFW_GAMEPAD_AXIS_RIGHT_Y] = GP_RightStick;
			result[GLFW_GAMEPAD_AXIS_LEFT_TRIGGER] = GP_LeftTrigger;
			result[GLFW_GAMEPAD_AXIS_RIGHT_TRIGGER] = GP_RightTrigger;

			return result;
		}();

		return table[a_axis];
	}
}
