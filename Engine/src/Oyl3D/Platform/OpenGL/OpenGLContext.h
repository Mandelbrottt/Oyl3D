#pragma once

#include "Oyl3D/Graphics/GraphicsContext.h"

class GLFWwindow;

namespace oyl {

class OpenGLContext : public GraphicsContext {
public:
	OpenGLContext(GLFWwindow* windowHandle);

	virtual void init() override;
	virtual void swapBuffers() override;
private:
	GLFWwindow* m_windowHandle;

	static bool s_glfwInitialized;
};

}