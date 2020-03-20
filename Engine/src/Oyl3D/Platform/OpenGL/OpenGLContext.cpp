#include "oylpch.h"
#include "OpenGLContext.h"

#include <glfw/glfw3.h>
#include <glad/glad.h>

namespace oyl
{
    static void OpenGLErrorCallback(GLenum        source,
                                    GLenum        type,
                                    GLuint        id,
                                    GLenum        severity,
                                    GLsizei       length,
                                    const GLchar* message,
                                    const void*   userParam)
    {
        const char* finalMessage = "[OpenGL Error] ID {0}: {1}";
        switch (severity)
        {
            case GL_DEBUG_SEVERITY_NOTIFICATION:
                //OYL_LOG_INFO(finalMessage, id, message); 
                break;
            case GL_DEBUG_SEVERITY_LOW:
            case GL_DEBUG_SEVERITY_MEDIUM:
                OYL_LOG_WARN(finalMessage, id, message);
                break;
            case GL_DEBUG_SEVERITY_HIGH:
                OYL_LOG_ERROR(finalMessage, id, message);
                OYL_BREAKPOINT;
                break;
        }
    }

    OpenGLContext::OpenGLContext(GLFWwindow* windowHandle)
        : m_windowHandle(windowHandle)
    {
        OYL_ASSERT(windowHandle, "Window handle is null!");
    }

    void OpenGLContext::init()
    {
        glfwMakeContextCurrent(m_windowHandle);
        int status = gladLoadGLLoader((GLADloadproc) glfwGetProcAddress);
        OYL_ASSERT(status, "Failed to initialize Glad!");

    #if defined(OYL_LOG_CONSOLE)
        glEnable(GL_DEBUG_OUTPUT);
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
        glDebugMessageCallback(OpenGLErrorCallback, 0);
        glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, true);
    #endif

        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        glDepthFunc(GL_LEQUAL);
        
        OYL_LOG_INFO("OpenGL Info:");
        OYL_LOG_INFO("  Vendor: {0}", glGetString(GL_VENDOR));
        OYL_LOG_INFO("  Renderer: {0}", glGetString(GL_RENDERER));
        OYL_LOG_INFO("  Version: {0}", glGetString(GL_VERSION));
    }

    void OpenGLContext::swapBuffers()
    {
        glfwSwapBuffers(m_windowHandle);

        if (m_isViewportDirty)
        {
            glViewport(0, 0, m_vpWidth, m_vpHeight);
            m_isViewportDirty = false;
        }
    }

    void OpenGLContext::updateViewport(int width, int height)
    {
        if (m_vpWidth == width && m_vpHeight == height) return;

        m_vpWidth         = width;
        m_vpHeight        = height;
        m_isViewportDirty = true;
    }
}
