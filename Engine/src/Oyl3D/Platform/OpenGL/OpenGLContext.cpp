#include "oylpch.h"
#include "OpenGLContext.h"

#include <glfw/glfw3.h>
#include <glad/glad.h>

namespace oyl {

OpenGLContext::OpenGLContext(GLFWwindow* windowHandle)
	: m_windowHandle(windowHandle) {
	ASSERT(windowHandle, "Window handle is null!");
}

void OpenGLContext::init() {
	glfwMakeContextCurrent(m_windowHandle);
	int status = gladLoadGLLoader((GLADloadproc) glfwGetProcAddress);
	ASSERT(status, "Failed to initialize Glad!");

	LOG_INFO("OpenGL Info:");
	LOG_INFO("  Vendor: {0}", glGetString(GL_VENDOR));
	LOG_INFO("  Renderer: {0}", glGetString(GL_RENDERER));
	LOG_INFO("  Version: {0}", glGetString(GL_VERSION));
}

void OpenGLContext::swapBuffers() {
	glfwSwapBuffers(m_windowHandle);
}

}

