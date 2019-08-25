#include "oylpch.h"

#include "Win32Window.h"
#include "Win32Input.h"

#include <glfw/glfw3.h>
#include <glad/glad.h>

namespace oyl {

bool Win32Window::s_GLFWInitialized = false;

static void GLFWErrorCallback(int error, const char* description) {
	LOG_ERROR("GLFW ERROR ({0}): {1}", error, description);
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
		VERIFY(glfwInit(), "Could not intialize GLFW!");
		glfwSetErrorCallback(GLFWErrorCallback);
		s_GLFWInitialized = true;
	}

	m_data.monitor = glfwGetPrimaryMonitor();

	// TODO: Abstract away glfw more

	m_window = glfwCreateWindow(props.width,
								props.height,
								m_data.title.c_str(),
								m_data.fullscreenType == Fullscreen ? m_data.monitor : nullptr,
								nullptr);

	const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
	m_data.posx = mode->width / 2 - props.width / 2;
	m_data.posy = mode->height / 2 - props.height / 2;

	glfwSetWindowAspectRatio(m_window, 16, 9);
	glfwSetWindowSizeLimits(m_window, 1280, 720, GLFW_DONT_CARE, GLFW_DONT_CARE);

	if (m_data.fullscreenType == Windowed)
		glfwSetWindowMonitor(m_window, nullptr, m_data.posx, m_data.posy, m_data.width, m_data.height, 0);

	m_context = GraphicsContext::create(m_window);
	m_context->init();

	glfwSetWindowUserPointer(m_window, &m_data);
	setVsync(true);

	// Set GLFW callbacks
	glfwSetWindowSizeCallback(m_window, [](GLFWwindow * window, int width, int height)
							  {
								  if (width <= 0 || height <= 0) return;

								  WindowData& data = *(WindowData*) glfwGetWindowUserPointer(window);

								  WindowResizeEvent event(width, height);

								  data.width = width;
								  data.height = height;
								  data.eventCallback(event);
							  });

	glfwSetWindowCloseCallback(m_window, [](GLFWwindow * window)
							   {
								   WindowData& data = *(WindowData*) glfwGetWindowUserPointer(window);

								   WindowCloseEvent event;

								   data.eventCallback(event);
							   });

	glfwSetWindowFocusCallback(m_window, [](GLFWwindow * window, int focused) 
							   {
								   WindowData& data = *(WindowData*) glfwGetWindowUserPointer(window);

								   WindowFocusEvent focusEvent(focused);

								   data.eventCallback(focusEvent);
							   });

	glfwSetKeyCallback(m_window, [](GLFWwindow * window, int key, int scancode, int action, int mods)
					   {
						   WindowData& data = *(WindowData*) glfwGetWindowUserPointer(window);

						   static int repeats = 0;

 						   switch (action) {
						   case GLFW_PRESS: {
							   KeyPressEvent pressEvent(glfwToOylCode(key));
							   data.eventCallback(pressEvent);
							   break;
						   } case GLFW_RELEASE: {
							   KeyReleaseEvent releaseEvent(glfwToOylCode(key));
							   data.eventCallback(releaseEvent);
							   repeats = 0;
							   break;
						   } case GLFW_REPEAT: {
							   KeyPressEvent repeatEvent(glfwToOylCode(key), ++repeats);
							   data.eventCallback(repeatEvent);
							   break;
						   }
						   }
					   });

	glfwSetCharCallback(m_window, [](GLFWwindow * window, uint keycode)
						{
							WindowData& data = *(WindowData*) glfwGetWindowUserPointer(window);

							KeyTypeEvent event(glfwToOylCode(keycode));

							data.eventCallback(event);
						});

	glfwSetMouseButtonCallback(m_window, [](GLFWwindow * window, int button, int action, int mods)
							   {
								   WindowData& data = *(WindowData*) glfwGetWindowUserPointer(window);

								   switch (action) {
								   case GLFW_PRESS: {
									   MousePressEvent pressEvent(glfwToOylCode(button));
									   data.eventCallback(pressEvent);
									   break;
								   } case GLFW_RELEASE: {
									   MouseReleaseEvent releaseEvent(glfwToOylCode(button));
									   data.eventCallback(releaseEvent);
									   break;
								   }
								   }
							   });

	glfwSetScrollCallback(m_window, [](GLFWwindow * window, double x, double y)
						  {
							  WindowData& data = *(WindowData*) glfwGetWindowUserPointer(window);

							  MouseScrollEvent event((float) x, (float) y);
							  data.eventCallback(event);
						  });

	glfwSetCursorPosCallback(m_window, [](GLFWwindow * window, double x, double y)
							 {
								 WindowData& data = *(WindowData*) glfwGetWindowUserPointer(window);

								 static float lastx = (float) x, lasty = (float) y;

								 MouseMoveEvent event((float) x, (float) y, (float) x - lastx, (float) y - lasty);

								 lastx = (float) x;
								 lasty = (float) y;

								 data.eventCallback(event);
							 });
}

void Win32Window::shutdown() {
	glfwTerminate();
}


void Win32Window::onUpdate(bool doSwapBuffers /*= true*/)
{
	glfwPollEvents();
	if (doSwapBuffers) {
		m_context->swapBuffers();
	}
}

void Win32Window::setVsync(bool enabled) {
	glfwSwapInterval(enabled ? 1 : 0);
	m_data.vsync = enabled;
}

bool Win32Window::isVsync() const {
	return m_data.vsync;
}

void Win32Window::setFullscreenType(OylEnum type) {

	if (m_data.fullscreenType == type) return;
	m_data.fullscreenType = type;

	static int lastWindowSize[2] = { 1280, 720 };

	if (type == Fullscreen || type == Borderless) {
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
		m_desiredWidth = mode->width;
		m_desiredHeight = mode->height;
	} else if (type == Windowed) {
		glfwSetWindowMonitor(m_window, nullptr, 
							 m_data.posx, m_data.posy, 
							 lastWindowSize[0], lastWindowSize[1], 
							 0);
		m_desiredWidth = lastWindowSize[0];
		m_desiredHeight = lastWindowSize[1];
	}
	m_context->updateViewport(m_desiredWidth, m_desiredHeight);
}

OylEnum Win32Window::getFullscreenType() const {
	return m_data.fullscreenType;
}

void Win32Window::updateViewport(int width, int height) {
	m_context->updateViewport(width, height);
}

}

