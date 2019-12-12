#include "oylpch.h"

#include "Win32Window.h"
#include "Win32Input.h"

#include "Events/Event.h"

#include <glfw/glfw3.h>
#include <glad/glad.h>

#include <glm/gtx/norm.hpp>

#include <Xinput.h>

#pragma comment(lib, "XInput.lib");

// HACK:
static void* _oylGamepadUserPointer = 0;

static glm::vec2 _radialDeadZone(glm::vec2 rawInput, float deadZone);
static glm::vec2 _crossDeadZone(glm::vec2 rawInput, float deadZone);

namespace oyl
{
    bool Win32Window::s_GLFWInitialized = false;

    static void GLFWErrorCallback(int error, const char* description)
    {
        OYL_LOG_ERROR("GLFW ERROR ({0}): {1}", error, description);
    }

    Win32Window::Win32Window(const WindowProps& props)
    {
        Win32Window::init(props);
    }

    Win32Window::~Win32Window()
    {
        Win32Window::shutdown();
    }

    void Win32Window::init(const WindowProps& props)
    {
        m_data.title  = props.title;
        m_data.width  = props.width;
        m_data.height = props.height;
        m_data.vsync  = props.vsync;

        OYL_LOG_INFO("Creating Window \"{0}\" with size ({1}, {2})", props.title, props.width, props.height);

        if (!s_GLFWInitialized)
        {
            OYL_VERIFY(glfwInit(), "Could not intialize GLFW!");
            glfwSetErrorCallback(GLFWErrorCallback);
            s_GLFWInitialized = true;
        }

        m_data.monitor = glfwGetPrimaryMonitor();

        // TODO: Abstract away glfw more

        m_window = glfwCreateWindow(props.width,
                                    props.height,
                                    m_data.title.c_str(),
                                    m_data.windowState == WindowState::Fullscreen ? m_data.monitor : nullptr,
                                    nullptr);

        const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
        m_data.posx             = mode->width / 2 - props.width / 2;
        m_data.posy             = mode->height / 2 - props.height / 2;

        glfwSetWindowAspectRatio(m_window, 16, 9);
        glfwSetWindowSizeLimits(m_window, 1280, 720, GLFW_DONT_CARE, GLFW_DONT_CARE);

        if (m_data.windowState == WindowState::Windowed)
            glfwSetWindowMonitor(m_window, nullptr, m_data.posx, m_data.posy, m_data.width, m_data.height, 0);

        m_context = GraphicsContext::create(m_window);
        m_context->init();

        glfwSetWindowUserPointer(m_window, &m_data);        
        _oylGamepadUserPointer = &m_data;

        setVsync(true);

        // Set GLFW callbacks
        glfwSetWindowSizeCallback(m_window, [](GLFWwindow* window, int width, int height)
        {
            if (width <= 0 || height <= 0) return;

            WindowData& data = *(WindowData*) glfwGetWindowUserPointer(window);

            WindowResizedEvent event{};
            event.width  = width;
            event.height = height;

            data.width  = width;
            data.height = height;
            data.eventCallback(event);
        });

        glfwSetWindowCloseCallback(m_window, [](GLFWwindow* window)
        {
            WindowData& data = *(WindowData*) glfwGetWindowUserPointer(window);

            WindowClosedEvent event{};

            data.eventCallback(event);
        });

        glfwSetWindowFocusCallback(m_window, [](GLFWwindow* window, int focused)
        {
            WindowData& data = *(WindowData*) glfwGetWindowUserPointer(window);

            WindowFocusedEvent event{};

            data.eventCallback(event);
        });

        glfwSetKeyCallback(m_window, [](GLFWwindow* window, int key, int scancode, int action, int mods)
        {
            WindowData& data = *(WindowData*) glfwGetWindowUserPointer(window);

            static int repeats = 0;

            switch (action)
            {
                case GLFW_PRESS:
                {
                    KeyPressedEvent event{};
                    event.keycode     = glfwToOylKeyCode(key);
                    event.mods        = mods;
                    event.repeatCount = 0;
                    data.eventCallback(event);
                    break;
                }
                case GLFW_RELEASE:
                {
                    KeyReleasedEvent event{};
                    event.keycode = glfwToOylKeyCode(key);
                    event.mods    = mods;
                    data.eventCallback(event);
                    break;
                }
                case GLFW_REPEAT:
                {
                    KeyPressedEvent event{};
                    event.keycode     = glfwToOylKeyCode(key);
                    event.mods        = mods;
                    event.repeatCount = ++repeats;
                    data.eventCallback(event);
                    break;
                }
            }
        });

        glfwSetCharCallback(m_window, [](GLFWwindow* window, uint keycode)
        {
            WindowData& data = *(WindowData*) glfwGetWindowUserPointer(window);

            KeyTypedEvent event{};
            event.keycode = glfwToOylKeyCode(keycode);

            data.eventCallback(event);
        });

        glfwSetMouseButtonCallback(m_window, [](GLFWwindow* window, int button, int action, int mods)
        {
            WindowData& data = *(WindowData*) glfwGetWindowUserPointer(window);

            switch (action)
            {
                case GLFW_PRESS:
                {
                    MousePressedEvent event{};
                    event.button = glfwToOylMouseCode(button);
                    event.mods   = mods;
                    data.eventCallback(event);
                    break;
                }
                case GLFW_RELEASE:
                {
                    MouseReleasedEvent event{};
                    event.button = glfwToOylMouseCode(button);
                    event.mods   = mods;
                    data.eventCallback(event);
                    break;
                }
            }
        });

        glfwSetScrollCallback(m_window, [](GLFWwindow* window, double x, double y)
        {
            WindowData& data = *(WindowData*) glfwGetWindowUserPointer(window);

            MouseScrolledEvent event{};
            event.x = (f32) x;
            event.y = (f32) y;

            data.eventCallback(event);
        });

        glfwSetCursorPosCallback(m_window, [](GLFWwindow* window, double x, double y)
        {
            WindowData& data = *(WindowData*) glfwGetWindowUserPointer(window);

            static float lastx = (float) x, lasty = (float) y;

            MouseMovedEvent event{};
            event.x  = (f32) x;
            event.y  = (f32) y;
            event.dx = (f32) x - lastx;
            event.dy = (f32) y - lasty;

            lastx = (float) x;
            lasty = (float) y;

            data.eventCallback(event);
        });

        glfwSetJoystickCallback([](int jid, int event)
        {
            WindowData& data = *(WindowData*) _oylGamepadUserPointer;

            switch (event)
            {
                case GLFW_CONNECTED:
                {
                    GamepadConnectedEvent e;
                    e.gid = jid;
                    data.eventCallback(e);
                    break;
                }
                case GLFW_DISCONNECTED:
                {
                    GamepadDisconnectedEvent e;
                    e.gid = jid;
                    data.eventCallback(e);
                    break;
                }
            }
        });
    }

    void Win32Window::shutdown()
    {
        glfwTerminate();
        _oylGamepadUserPointer = nullptr;
    }

    void Win32Window::onUpdate(bool doSwapBuffers)
    {
        glfwPollEvents();

        // TODO: Move into application
        pollGamepadEvents();

        if (doSwapBuffers)
        {
            m_context->swapBuffers();
        }
    }

    void Win32Window::pollGamepadEvents()
    {
        const int numGamepads = 4;

        static GLFWgamepadstate currState[numGamepads];
        static GLFWgamepadstate prevState[numGamepads];

        static int repeatCount[numGamepads][GLFW_GAMEPAD_BUTTON_LAST];

        for (int jid = 0; jid < numGamepads; jid++)
        {
            if (glfwJoystickIsGamepad(jid))
            {
                glfwGetGamepadState(jid, &currState[jid]);
                for (int button = GLFW_GAMEPAD_BUTTON_A; button <= GLFW_GAMEPAD_BUTTON_LAST; button++)
                {
                    // Button Pressed
                    if (currState[jid].buttons[button] == GLFW_PRESS)
                    {
                        GamepadButtonPressedEvent event;
                        event.gid         = jid;
                        event.button      = glfwToOylGamepadCode(button);
                        event.repeatCount = repeatCount[jid][button]++;
                        m_data.eventCallback(event);
                    } // Button released
                    else if (currState[jid].buttons[button] == GLFW_RELEASE &&
                             prevState[jid].buttons[button] == GLFW_PRESS)
                    {
                        GamepadButtonReleasedEvent event;
                        event.gid    = jid;
                        event.button = glfwToOylGamepadCode(button);
                        m_data.eventCallback(event);
                        repeatCount[jid][button] = 0;
                    }
                }

                const float deadZone = 0.3f;

                for (int i = 0; i < 2; i++)
                {
                    float x  = currState[jid].axes[2 * i];
                    float y  = currState[jid].axes[2 * i + 1];
                    float px = prevState[jid].axes[2 * i];
                    float py = prevState[jid].axes[2 * i + 1];

                    glm::vec2 dz  = _radialDeadZone(_crossDeadZone({ x, y }, deadZone * 0.5f), deadZone * 0.5f);
                    glm::vec2 pdz = _radialDeadZone(_crossDeadZone({ px, py }, deadZone * 0.5f), deadZone * 0.5f);

                    if (glm::length2(dz) > 1.0f) dz = glm::normalize(dz);
                    if (glm::length2(pdz) > 1.0f) pdz = glm::normalize(pdz);

                    if (dz.x != pdz.x || dz.y != pdz.y)
                    {
                        GamepadStickMovedEvent event;
                        event.gid   = jid;
                        event.stick = Gamepad(i32(Gamepad::LeftStick) + i);
                        event.x     = dz.x;
                        event.y     = -dz.y;
                        event.dx    = pdz.x - dz.x;
                        event.dy    = -(pdz.y - dz.y);
                        m_data.eventCallback(event);
                    }
                }

                for (int i = 0; i < 2; i++)
                {
                    float x  = currState[jid].axes[GLFW_GAMEPAD_AXIS_LEFT_TRIGGER + i];
                    float px = prevState[jid].axes[GLFW_GAMEPAD_AXIS_LEFT_TRIGGER + i];

                    if (x != px)
                    {
                        GamepadTriggerPressedEvent event;
                        event.gid     = jid;
                        event.trigger = (Gamepad) i;
                        event.x       = x;
                        event.dx      = px - x;
                        m_data.eventCallback(event);
                    }
                }
            }
            prevState[jid] = currState[jid];
        }
    }

    void Win32Window::setVsync(bool enabled)
    {
        glfwSwapInterval(enabled ? 1 : 0);
        m_data.vsync = enabled;
    }

    bool Win32Window::isVsync() const
    {
        return m_data.vsync;
    }

    void Win32Window::setWindowState(WindowState state)
    {
        if (m_data.windowState == state) return;
        m_data.windowState = state;

        static int lastWindowSize[2] = { 1280, 720 };

        if (state == WindowState::Fullscreen || state == WindowState::Borderless)
        {
            // backup windwo position and window size
            glfwGetWindowPos(m_window, &m_data.posx, &m_data.posy);
            glfwGetWindowSize(m_window, lastWindowSize, lastWindowSize + 1);

            // get reolution of monitor
            const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());

            // switch to full screen
            glfwSetWindowMonitor(m_window, m_data.monitor,
                                 0, 0,
                                 mode->width, mode->height,
                                 mode->refreshRate);
            m_desiredWidth  = mode->width;
            m_desiredHeight = mode->height;
        }
        else if (state == WindowState::Windowed)
        {
            glfwSetWindowMonitor(m_window, nullptr,
                                 m_data.posx, m_data.posy,
                                 lastWindowSize[0], lastWindowSize[1],
                                 0);
            m_desiredWidth  = lastWindowSize[0];
            m_desiredHeight = lastWindowSize[1];
        }
        m_context->updateViewport(m_desiredWidth, m_desiredHeight);
    }

    WindowState Win32Window::getWindowState() const
    {
        return m_data.windowState;
    }

    void Win32Window::setCursorState(CursorState state)
    {
        uint glfwState = 0;
        switch (state)
        {
            case CursorState::Normal:
                glfwState = GLFW_CURSOR_NORMAL;
                break;
            case CursorState::Disabled:
                glfwState = GLFW_CURSOR_DISABLED;
                break;
            case CursorState::Hidden:
                glfwState = GLFW_CURSOR_HIDDEN;
                break;
            default:
                OYL_ASSERT(false, "Invalid cursor state!");
                glfwState = GLFW_CURSOR_NORMAL;
        }
        glfwSetInputMode(m_window, GLFW_CURSOR, glfwState);
    }

    CursorState Win32Window::getCursorState() const
    {
        auto currState = glfwGetInputMode(m_window, GLFW_CURSOR);
        switch (currState)
        {
            case GLFW_CURSOR_NORMAL:
                return CursorState::Normal;
            case GLFW_CURSOR_DISABLED:
                return CursorState::Disabled;
            case GLFW_CURSOR_HIDDEN:
                return CursorState::Hidden;
        }
        // Redundant, here to suppress compiler warnings
        return CursorState::Normal;
    }

    void Win32Window::updateViewport(int width, int height)
    {
        m_context->updateViewport(width, height);
    }
}

glm::vec2 _radialDeadZone(glm::vec2 rawInput, float deadZone)
{
    if (glm::length2(rawInput) < deadZone * deadZone)
        return glm::vec2(0.0f);
    else
        return glm::normalize(rawInput) * (glm::length(rawInput) - deadZone) / (1 - deadZone);
}

glm::vec2 _crossDeadZone(glm::vec2 rawInput, float deadZone)
{
    glm::vec2 newInput = rawInput;

    if (abs(rawInput.x) < deadZone)
        newInput.x = 0;
    else
        newInput.x = glm::sign(rawInput.x) * (abs(rawInput.x) - deadZone) / (1 - deadZone);

    if (abs(rawInput.y) < deadZone)
        newInput.y = 0;
    else
        newInput.y = glm::sign(rawInput.y) * (abs(rawInput.y) - deadZone) / (1 - deadZone);

    return newInput;
}
