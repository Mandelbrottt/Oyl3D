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
}

void OpenGLContext::swapBuffers() {
	glfwSwapBuffers(m_windowHandle);
}

}

