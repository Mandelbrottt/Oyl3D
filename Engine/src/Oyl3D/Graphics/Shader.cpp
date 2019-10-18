#include "oylpch.h"
#include "Shader.h"

#include "Platform/OpenGL/OpenGLShader.h"

#include "Rendering/Renderer.h"

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
                return OpenGLShader::create(files);
        }
        return nullptr;
    }
}
