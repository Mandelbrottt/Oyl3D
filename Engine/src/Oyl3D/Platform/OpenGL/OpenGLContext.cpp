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
	const char* finalMessage = "[OpenGL Error] ID {0}: {1}";
	switch (severity) {
	case GL_DEBUG_SEVERITY_NOTIFICATION:
		//LOG_INFO(finalMessage, id, message); 
		break;
	case GL_DEBUG_SEVERITY_LOW:
	case GL_DEBUG_SEVERITY_MEDIUM:
		LOG_WARN(finalMessage, id, message); 
		break;
	case GL_DEBUG_SEVERITY_HIGH:
		LOG_ERROR(finalMessage, id, message); 
		BREAKPOINT; 
		break;
	}
}

OpenGLContext::OpenGLContext(GLFWwindow* windowHandle)
	: m_windowHandle(windowHandle) {
	ASSERT(windowHandle, "Window handle is null!");
}

void OpenGLContext::init() {
	glfwMakeContextCurrent(m_windowHandle);
	int status = gladLoadGLLoader((GLADloadproc) glfwGetProcAddress);
	ASSERT(status, "Failed to initialize Glad!");

#if defined(OYL_LOG_CONSOLE)
	glEnable(GL_DEBUG_OUTPUT);
	glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
	glDebugMessageCallback(OpenGLErrorCallback, 0);
	glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, true);
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

	if (m_isViewportDirty) {
		glViewport(0, 0, m_vpWidth, m_vpHeight);
		m_isViewportDirty = false;
	}
}

void OpenGLContext::updateViewport(int width, int height) {
	if (m_vpWidth == width && m_vpHeight == height) return;

	m_vpWidth = width;
	m_vpHeight = height;
	m_isViewportDirty = true;
}

}

