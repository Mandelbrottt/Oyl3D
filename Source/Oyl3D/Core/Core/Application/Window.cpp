#include "Window.h"

#include <vulkan/vulkan.hpp>

#include <GLFW/glfw3.h>

#include "Core/Events/EventDispatcher.h"
#include "Core/Input/InputCodes.h"
#include "Core/Logging/Logging.h"

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
	struct Window::Impl
	{
		std::string title;

		Vector2i position;
		Vector2i size;

		WindowStateFlags windowState;
		CursorState cursorState;

		GLFWwindow* glfwWindow = nullptr;

		EventDelegate postEventCallback;
	};

	Window::Window()
		: m_impl(new Impl) {}

	Window::Window(const WindowParams& a_params)
		: Window()
	{
		Init(a_params);
	}

	Window::Window(Window&& a_other) noexcept
	{
		*this = std::move(a_other);
	}

	Window&
	Window::operator=(Window&& a_other) noexcept
	{
		std::swap(m_impl, a_other.m_impl);
		return *this;
	}

	Window::~Window()
	{
		if (!m_impl)
			return;

		Destroy();

		std::memset(m_impl, 0, sizeof(Impl));
		delete m_impl;
		m_impl = nullptr;
	}

	void
	Window::Init(const WindowParams& a_params)
	{
		OYL_PROFILE_FUNCTION();

		m_impl->postEventCallback = a_params.onEventCallback;

		m_impl->title = a_params.title;

		m_impl->position = a_params.position;
		m_impl->size = a_params.size;

		m_impl->windowState = a_params.windowState;
		m_impl->cursorState = a_params.cursorState;

		auto monitor = glfwGetPrimaryMonitor();

		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		auto size = m_impl->size;
		m_impl->glfwWindow = glfwCreateWindow(
			size.x,
			size.y,
			m_impl->title.c_str(),
			m_impl->windowState & WS_Fullscreen ? monitor : nullptr,
			nullptr
		);

		auto* mode = glfwGetVideoMode(monitor);
		m_impl->position.x = mode->width / 2 - a_params.size.x / 2;
		m_impl->position.y = mode->height / 2 - a_params.size.y / 2;

		glfwSetWindowAspectRatio(m_impl->glfwWindow, 16, 9);

		if (m_impl->windowState ^ WS_Fullscreen)
		{
			glfwSetWindowMonitor(
				m_impl->glfwWindow,
				nullptr,
				m_impl->position.x,
				m_impl->position.y,
				m_impl->size.x,
				m_impl->size.y,
				mode->refreshRate
			);
		}

		glfwSetWindowUserPointer(m_impl->glfwWindow, m_impl);

		glfwSetWindowSizeCallback(
			m_impl->glfwWindow,
			[](GLFWwindow* a_window, int a_width, int a_height)
			{
				if (a_width <= 0 || a_height <= 0)
					return;

				Impl* impl = reinterpret_cast<Impl*>(glfwGetWindowUserPointer(a_window));
				impl->size = { a_width, a_height };

				if (!impl->postEventCallback)
					return;

				WindowResizeEvent event;
				event.nativeWindow = a_window;
				event.size = impl->size;
				impl->postEventCallback(event);
			}
		);

		glfwSetWindowPosCallback(
			m_impl->glfwWindow,
			[](GLFWwindow* a_window, int a_posX, int a_posY)
			{
				if (a_posX <= 0 || a_posY <= 0)
					return;

				Impl* impl = reinterpret_cast<Impl*>(glfwGetWindowUserPointer(a_window));
				impl->position = { a_posX, a_posY };

				if (!impl->postEventCallback)
					return;

				WindowMoveEvent event;
				event.nativeWindow = a_window;
				event.position = impl->position;
				impl->postEventCallback(event);
			}
		);

		glfwSetWindowCloseCallback(
			m_impl->glfwWindow,
			[](GLFWwindow* a_window)
			{
				Impl* impl = reinterpret_cast<Impl*>(glfwGetWindowUserPointer(a_window));

				if (!impl->postEventCallback)
					return;

				WindowCloseRequestEvent event;
				event.nativeWindow = a_window;
				impl->postEventCallback(event);
			}
		);

		glfwSetWindowFocusCallback(
			m_impl->glfwWindow,
			[](GLFWwindow* a_window, int a_focused)
			{
				Impl* impl = reinterpret_cast<Impl*>(glfwGetWindowUserPointer(a_window));

				if (!impl->postEventCallback)
					return;

				WindowFocusEvent event;
				event.nativeWindow = a_window;
				event.focused = a_focused;
				impl->postEventCallback(event);
			}
		);

		glfwSetKeyCallback(
			m_impl->glfwWindow,
			[](GLFWwindow* a_window, int a_key, int a_scanCode, int a_action, int a_mods)
			{
				OYL_UNUSED(a_scanCode);

				Impl* impl = reinterpret_cast<Impl*>(glfwGetWindowUserPointer(a_window));

				if (!impl->postEventCallback)
					return;

				Input::KeyboardKey key = GetKeyCodeFromGlfw(a_key);

				switch (a_action)
				{
					case GLFW_PRESS:
					{
						WindowKeyPressEvent event;
						event.nativeWindow = a_window;
						event.key = key;
						event.mods = a_mods;
						impl->postEventCallback(event);
						break;
					}
					case GLFW_RELEASE:
					{
						WindowKeyReleaseEvent event;
						event.nativeWindow = a_window;
						event.key = key;
						event.mods = a_mods;
						impl->postEventCallback(event);
						break;
					}
					default:
						break;
				}
			}
		);

		glfwSetMouseButtonCallback(
			m_impl->glfwWindow,
			[](GLFWwindow* a_window, int a_button, int a_action, int a_mods)
			{
				Impl* impl = reinterpret_cast<Impl*>(glfwGetWindowUserPointer(a_window));

				if (!impl->postEventCallback)
					return;

				Input::MouseButton button = GetMouseButtonFromGlfw(a_button);

				switch (a_action)
				{
					case GLFW_PRESS:
					{
						WindowMousePressEvent event;
						event.nativeWindow = a_window;
						event.button = button;
						event.mods = a_mods;
						impl->postEventCallback(event);
						break;
					}
					case GLFW_RELEASE:
					{
						WindowMouseReleaseEvent event;
						event.nativeWindow = a_window;
						event.button = button;
						event.mods = a_mods;
						impl->postEventCallback(event);
						break;
					}
					default:
						break;
				}
			}
		);

		glfwSetScrollCallback(
			m_impl->glfwWindow,
			[](GLFWwindow* a_window, double a_scrollX, double a_scrollY)
			{
				Impl* impl = reinterpret_cast<Impl*>(glfwGetWindowUserPointer(a_window));

				if (!impl->postEventCallback)
					return;

				WindowMouseScrollEvent event;
				event.nativeWindow = a_window;
				event.scroll = { (float32) a_scrollX, (float32) a_scrollY };
				impl->postEventCallback(event);
			}
		);

		glfwSetCursorPosCallback(
			m_impl->glfwWindow,
			[](GLFWwindow* a_window, double a_posX, double a_posY)
			{
				Impl* impl = reinterpret_cast<Impl*>(glfwGetWindowUserPointer(a_window));

				if (!impl->postEventCallback)
					return;

				WindowCursorMoveEvent event;
				event.nativeWindow = a_window;
				event.position = { (float32) a_posX, (float32) a_posY };
				impl->postEventCallback(event);
			}
		);
	}

	void
	Window::Destroy()
	{
		if (!m_impl->glfwWindow)
			return;

		OYL_PROFILE_FUNCTION();

		// TODO: Destroy context alongside window
		glfwDestroyWindow(m_impl->glfwWindow);
		m_impl->glfwWindow = nullptr;
	}

	void
	Window::Update()
	{
		OYL_PROFILE_FUNCTION();

		glfwPollEvents();
	}

	bool
	Window::IsValid() const
	{
		return m_impl->glfwWindow != nullptr;
	}

	void
	Window::SetEventCallback(EventDelegate a_delegate)
	{
		m_impl->postEventCallback = std::move(a_delegate);
	}

	Vector2i
	Window::GetSize() const
	{
		return m_impl->size;
	}

	void
	Window::SetSize(Vector2i a_size)
	{
		auto monitor = glfwGetWindowMonitor(m_impl->glfwWindow);
		auto mode = glfwGetVideoMode(monitor);

		glfwSetWindowMonitor(
			m_impl->glfwWindow,
			monitor,
			m_impl->position.x,
			m_impl->position.y,
			a_size.x,
			a_size.y,
			mode->refreshRate
		);

		m_impl->size = a_size;
	}

	Vector2i
	Window::GetPosition() const
	{
		return m_impl->position;
	}

	void
	Window::SetPosition(Vector2i a_position)
	{
		auto monitor = glfwGetWindowMonitor(m_impl->glfwWindow);
		auto mode = glfwGetVideoMode(monitor);

		glfwSetWindowMonitor(
			m_impl->glfwWindow,
			monitor,
			a_position.x,
			a_position.y,
			m_impl->size.x,
			m_impl->size.y,
			mode->refreshRate
		);

		m_impl->position = a_position;
	}

	std::string_view
	Window::GetTitle() const
	{
		return m_impl->title;
	}

	void
	Window::SetTitle(std::string_view a_title)
	{
		m_impl->title = std::string(a_title);
		glfwSetWindowTitle(m_impl->glfwWindow, m_impl->title.c_str());
	}

	WindowStateFlags
	Window::GetWindowStateFlags() const
	{
		return m_impl->windowState;
	}

	void
	Window::SetWindowStateFlags(WindowStateFlags a_flags)
	{
		if (m_impl->windowState == a_flags)
		{
			return;
		}

		OYL_PROFILE_FUNCTION();

		Vector2i desiredSize;

		// get resolution of monitor
		const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());

		if (a_flags & (WS_Fullscreen | WS_Borderless))
		{
			// backup window position and window size
			glfwGetWindowPos(m_impl->glfwWindow, &m_impl->position.x, &m_impl->position.y);
			glfwGetWindowSize(m_impl->glfwWindow, &m_impl->size.y, &m_impl->size.y);

			// switch to full screen
			glfwSetWindowMonitor(
				m_impl->glfwWindow,
				glfwGetWindowMonitor(m_impl->glfwWindow),
				0,
				0,
				mode->width,
				mode->height,
				mode->refreshRate
			);

			desiredSize = { mode->width, mode->height };
		} else if (a_flags ^ (WS_Fullscreen | WS_Borderless))
		{
			glfwSetWindowMonitor(
				m_impl->glfwWindow,
				nullptr,
				m_impl->position.x,
				m_impl->position.y,
				m_impl->size.x,
				m_impl->size.y,
				mode->refreshRate
			);
			desiredSize = m_impl->size;
		}

		// Set Context Size
		OYL_UNUSED(desiredSize);
		m_impl->windowState = a_flags;
	}

	CursorState
	Window::GetCursorStateFlags() const
	{
		return m_impl->cursorState;
	}

	void
	Window::SetCursorStateFlags(CursorState a_state)
	{
		if (m_impl->cursorState == a_state)
		{
			return;
		}

		int table[CS_Last];
		table[CS_Normal] = GLFW_CURSOR_NORMAL;
		table[CS_Hidden] = GLFW_CURSOR_HIDDEN;
		table[CS_Disabled] = GLFW_CURSOR_DISABLED;
		table[CS_Locked] = GLFW_CURSOR_CAPTURED;

		uint glfwState = table[a_state];
		glfwSetInputMode(m_impl->glfwWindow, GLFW_CURSOR, glfwState);

		m_impl->cursorState = a_state;
	}

	void*
	Window::GetNativeWindowHandle() const
	{
		return m_impl->glfwWindow;
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
