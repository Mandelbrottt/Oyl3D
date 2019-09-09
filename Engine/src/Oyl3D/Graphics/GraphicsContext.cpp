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
            case None:
                OYL_ASSERT(false, "None is currently unsupported");
                return nullptr;
            case API_OpenGL:
                return Ref<GraphicsContext>(new OpenGLContext((GLFWwindow*) window));
        }
        return nullptr;
    }
}
