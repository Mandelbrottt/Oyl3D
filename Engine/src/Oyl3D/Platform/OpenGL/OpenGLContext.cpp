#include "oylpch.h"
#include "OpenGLContext.h"

#include <glfw/glfw3.h>
#include <glad/glad.h>

namespace oyl {

static void OpenGLErrorCallback(GLenum source,
								GLenum type,
								GLuint id,
								GLenum severity,
								GLsizei length, 
								const GLchar* message,
								const void* userParam) {
	// Ignore List
	switch (id) {
	case 131185:
		return;
	}
	LOG_ERROR("OpenGL Error({0}) at {1}:{2}\n\t   {3}", id, __FILE__, __LINE__, message);
	BREAKPOINT;
}

OpenGLContext::OpenGLContext(GLFWwindow* windowHandle)
	: m_windowHandle(windowHandle) {
	ASSERT(windowHandle, "Window handle is null!");
}

void OpenGLContext::init() {
	glfwMakeContextCurrent(m_windowHandle);
	int status = gladLoadGLLoader((GLADloadproc) glfwGetProcAddress);
	ASSERT(status, "Failed to initialize Glad!");

	updateViewport();

#if !defined(OYL_DIST)
	glEnable(GL_DEBUG_OUTPUT);
	glDebugMessageCallback(OpenGLErrorCallback, 0);
#endif

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	LOG_INFO("OpenGL Info:");
	LOG_INFO("  Vendor: {0}", glGetString(GL_VENDOR));
	LOG_INFO("  Renderer: {0}", glGetString(GL_RENDERER));
	LOG_INFO("  Version: {0}", glGetString(GL_VERSION));
}

void OpenGLContext::swapBuffers() {
	glfwSwapBuffers(m_windowHandle);
}

void OpenGLContext::updateViewport() {
	glfwGetFramebufferSize(m_windowHandle, &m_vpWidth, &m_vpHeight);
	glViewport(0, 0, m_vpWidth, m_vpHeight);
}

}

