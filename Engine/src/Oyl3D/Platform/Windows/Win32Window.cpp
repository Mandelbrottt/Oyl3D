#include "oylpch.h"

#include "Win32Window.h"

namespace oyl {

bool Win32Window::s_GLFWInitialized = false;

static void GLFWErrorCallback(int error, const char* description) {
	LOG_ERROR("GLFW ERROR ({0}): {1}", error, description);
}

Window* Window::create(const WindowProps& props) {
	return new Win32Window(props);
}

Win32Window::Win32Window(const WindowProps& props) {
	init(props);
}

Win32Window::~Win32Window() {
	shutdown();
}

void Win32Window::init(const WindowProps& props) {
	m_data.title = props.title;
	m_data.width = props.width;
	m_data.height = props.height;
	m_data.vsync = props.vsync;

	LOG_INFO("Creating Window \"{0}\" with size ({1}, {2})", props.title, props.width, props.height);

	if (!s_GLFWInitialized)
	{
		// TODO: glfwTerminate on system shutdown
		int success = glfwInit();
		ASSERT(success, "Could not intialize GLFW!");
		glfwSetErrorCallback(GLFWErrorCallback);
		s_GLFWInitialized = true;
	}

	GLFWmonitor* monitor = nullptr;

	switch (m_data.fullscreenType) {
	case Windowed:
		glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);
		break;
	case Borderless:
		glfwWindowHint(GLFW_DECORATED, m_data.fullscreenType == GLFW_TRUE);
		break;
	case Fullscreen:
		monitor = glfwGetPrimaryMonitor();
		break;
	}

	m_window = glfwCreateWindow((int) props.width,
		(int) props.height,
								m_data.title.c_str(),
								monitor,
								nullptr);

	glfwMakeContextCurrent(m_window);
	glfwSetWindowUserPointer(m_window, &m_data);
	setVsync(true);

	// Set GLFW callbacks
	glfwSetWindowSizeCallback(m_window, [](GLFWwindow * window, int width, int height)
							  {
								  WindowData& data = *(WindowData*) glfwGetWindowUserPointer(window);

								  WindowEvent event;
								  event.type = WindowResize;
								  event.categoryFlags = CATEGORY_FLAGS(EventCategoryWindow);
								  event.x = width;
								  event.y = height;

								  data.width = width;
								  data.height = height;
								  data.eventCallback(event);
							  });

	glfwSetWindowCloseCallback(m_window, [](GLFWwindow * window)
							   {
								   WindowData& data = *(WindowData*) glfwGetWindowUserPointer(window);

								   WindowEvent event;
								   event.type = WindowClose;
								   event.categoryFlags = CATEGORY_FLAGS(EventCategoryWindow);
								   event.shouldClose = true;

								   data.eventCallback(event);
							   });

	glfwSetKeyCallback(m_window, [](GLFWwindow * window, int key, int scancode, int action, int mods)
					   {
						   WindowData& data = *(WindowData*) glfwGetWindowUserPointer(window);

						   KeyEvent event;
						   event.categoryFlags = CATEGORY_FLAGS(EventCategoryKeyboard);
						   event.keycode = key;
						   event.mods = mods;

						   switch (action) {
						   case GLFW_PRESS:
							   event.type = KeyPress;
							   event.pressed = true;
							   event.count = 0;
							   data.eventCallback(event);
							   break;
						   case GLFW_RELEASE:
							   event.type = KeyRelease;
							   event.pressed = false;
							   data.eventCallback(event);
							   break;
						   case GLFW_REPEAT:
							   event.type = KeyPress;
							   event.pressed = true;
							   event.count = 1;
							   break;
						   }
					   });

	glfwSetCharCallback(m_window, [](GLFWwindow * window, unsigned int keycode)
						{
							WindowData& data = *(WindowData*) glfwGetWindowUserPointer(window);

							KeyEvent event;
							event.type = KeyType;
							event.categoryFlags = CATEGORY_FLAGS(EventCategoryKeyboard);
							event.keycode = keycode;

							data.eventCallback(event);
						});

	glfwSetMouseButtonCallback(m_window, [](GLFWwindow * window, int button, int action, int mods)
							   {
								   WindowData& data = *(WindowData*) glfwGetWindowUserPointer(window);

								   MouseEvent event;
								   event.categoryFlags = CATEGORY_FLAGS(EventCategoryMouse);
								   event.button = button;
								   event.mods = mods;

								   switch (action) {
								   case GLFW_PRESS:
									   event.type = MousePress;
									   event.pressed = true;
									   break;
								   case GLFW_RELEASE:
									   event.type = MouseRelease;
									   event.pressed = false;
									   break;
								   }
								   data.eventCallback(event);
							   });

	glfwSetScrollCallback(m_window, [](GLFWwindow * window, double xOffset, double yOffset)
						  {
							  WindowData& data = *(WindowData*) glfwGetWindowUserPointer(window);

							  MouseEvent event;
							  event.type = MouseScroll;
							  event.categoryFlags = CATEGORY_FLAGS(EventCategoryMouse);
							  event.scroll.x = (float) xOffset;
							  event.scroll.y = (float) yOffset;
							  data.eventCallback(event);
						  });

	glfwSetCursorPosCallback(m_window, [](GLFWwindow * window, double xPos, double yPos)
							 {
								 WindowData& data = *(WindowData*) glfwGetWindowUserPointer(window);

								 MouseEvent event;
								 event.type = MouseMove;
								 event.categoryFlags = CATEGORY_FLAGS(EventCategoryMouse);

								 static float lastx = (float) xPos, lasty = (float) yPos;

								 event.position.x = (float) xPos;
								 event.position.y = (float) yPos;
								 event.delta.x = (float) xPos - lastx;
								 event.delta.y = (float) yPos - lasty;

								 lastx = (float) xPos;
								 lasty = (float) yPos;

								 data.eventCallback(event);
							 });
}

void Win32Window::shutdown() {

}


void Win32Window::onUpdate() {
	glfwPollEvents();
	glfwSwapBuffers(m_window);
}

void Win32Window::setVsync(bool enabled) {
	glfwSwapInterval(enabled ? 1 : 0);
	m_data.vsync = enabled;
}

bool Win32Window::isVsync() const {
	return m_data.vsync;
}

void Win32Window::setFullscreenType(FullscreenType type) {
	if (m_data.fullscreenType != type) {
		m_data.fullscreenType = type;
		init(WindowProps(m_data.title, m_data.width, m_data.height, m_data.vsync));
	}
}

bool Win32Window::getFullscreenType() const {
	return m_data.fullscreenType;
}

}

