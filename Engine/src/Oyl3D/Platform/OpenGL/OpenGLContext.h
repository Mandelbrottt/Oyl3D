#pragma once

#include "Oyl3D/Graphics/GraphicsContext.h"

#include "glfw/glfw3.h"

namespace oyl {

class OpenGLContext : public GraphicsContext {
public:
	OpenGLContext(GLFWwindow* windowHandle);

	virtual void init() override;
	virtual void swapBuffers() override;
	virtual void updateViewport() override;
private:
	GLFWwindow* m_windowHandle;

	int m_vpWidth = 0;
	int m_vpHeight = 0;

	static bool s_glfwInitialized;
};

}