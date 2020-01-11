#include "oylpch.h"
#include "GraphicsContext.h"

#include "Rendering/Renderer.h"
#include "Platform/OpenGL/OpenGLContext.h"

namespace oyl
{
    Ref<GraphicsContext> GraphicsContext::create(void* window)
    {
        switch (Renderer::getAPI())
        {
            case RendererAPI::OpenGL:
                return Ref<OpenGLContext>::create((GLFWwindow*) window);
        }
        return nullptr;
    }
}
