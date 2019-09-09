#include "oylpch.h"
#include "RenderCommand.h"

#include "Platform/OpenGL/OpenGLRendererAPI.h"

namespace oyl
{
    RendererAPI* RenderCommand::s_rendererAPI = new OpenGLRendererAPI();
}
