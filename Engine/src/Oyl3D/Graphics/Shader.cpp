#include "oylpch.h"
#include "Shader.h"

#include "Rendering/Renderer.h"

#include "Platform/OpenGL/OpenGLShader.h"

namespace oyl
{
    Ref<Shader> Shader::create(const std::initializer_list<ShaderInfo>& files)
    {
        switch (Renderer::getAPI())
        {
            case None:
                OYL_ASSERT(false, "None is currently unsupported");
                return nullptr;
            case API_OpenGL:
                return Ref<Shader>(new OpenGLShader(files));
        }
        return nullptr;
    }
}
