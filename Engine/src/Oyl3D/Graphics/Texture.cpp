#include "oylpch.h"
#include "Texture.h"

#include "Rendering/Renderer.h"

#include "Platform/OpenGL/OpenGLTexture.h"

namespace oyl
{
    Ref<Texture1D> Texture1D::create(const std::string& filename)
    {
        switch (Renderer::getAPI())
        {
            case None:
                OYL_ASSERT(false, "None is currently unsupported");
                return nullptr;
            case API_OpenGL:
                return Ref<Texture1D>(new OpenGLTexture1D(filename));
        }
        return nullptr;
    }

    Ref<Texture2D> Texture2D::create(const std::string& filename)
    {
        switch (Renderer::getAPI())
        {
            case None:
                OYL_ASSERT(false, "None is currently unsupported");
                return nullptr;
            case API_OpenGL:
                return Ref<Texture2D>(new OpenGLTexture2D(filename));
        }
        return nullptr;
    }

    Ref<Texture3D> Texture3D::create(const std::string& filename)
    {
        switch (Renderer::getAPI())
        {
            case None:
                OYL_ASSERT(false, "None is currently unsupported");
                return nullptr;
            case API_OpenGL:
                return Ref<Texture3D>(new OpenGLTexture3D(filename));
        }
        return nullptr;
    }
}
