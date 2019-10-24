#include "oylpch.h"
#include "Texture.h"

#include "Rendering/Renderer.h"

#include "Platform/OpenGL/OpenGLTexture.h"

namespace oyl
{
    internal::AssetCache<Texture1D> Texture1D::s_cache;
    internal::AssetCache<Texture2D> Texture2D::s_cache;
    internal::AssetCache<Texture3D> Texture3D::s_cache;

    const char* internal::AssetCache<Texture1D>::s_typename = "Texture1D";
    const char* internal::AssetCache<Texture2D>::s_typename = "Texture2D";
    const char* internal::AssetCache<Texture3D>::s_typename = "Texture3D";
    
    Ref<Texture1D> Texture1D::create(const std::string& filePath)
    {
        switch (Renderer::getAPI())
        {
            case None:
                OYL_ASSERT(false, "None is currently unsupported");
                return nullptr;
            case API_OpenGL:
                return Ref<OpenGLTexture1D>::create(filePath);
        }
        return nullptr;
    }

    Ref<Texture2D> Texture2D::create(const std::string& filePath)
    {
        switch (Renderer::getAPI())
        {
            case None:
                OYL_ASSERT(false, "None is currently unsupported");
                return nullptr;
            case API_OpenGL:
                return Ref<OpenGLTexture2D>::create(filePath);
        }
        return nullptr;
    }

    Ref<Texture3D> Texture3D::create(const std::string& filePath)
    {
        switch (Renderer::getAPI())
        {
            case None:
                OYL_ASSERT(false, "None is currently unsupported");
                return nullptr;
            case API_OpenGL:
                return Ref<OpenGLTexture3D>::create(filePath);
        }
        return nullptr;
    }

    void Texture::init()
    {
        //Texture1D::s_invalid = 
        //Texture2D::s_invalid = 
        //Texture3D::s_invalid = 
    }

    const Ref<Texture1D>& Texture1D::cache(const std::string& filePath, 
                                           const CacheAlias& alias, 
                                           bool overwrite)
    {
        return s_cache.cache(filePath, alias, overwrite);
    }

    void Texture1D::discard(const CacheAlias& alias)
    {
        s_cache.discard(alias);
    }

    const Ref<Texture1D>& Texture1D::get(const CacheAlias& alias)
    {
        return s_cache.get(alias);
    }

    const Ref<Texture1D>& Texture1D::rename(const CacheAlias& currentAlias, 
                                            const CacheAlias& newAlias, 
                                            bool overwrite)
    {
        return s_cache.rename(currentAlias, newAlias, overwrite);
    }

    const Ref<Texture2D>& Texture2D::cache(const std::string& filePath, 
                                           const CacheAlias& alias, 
                                           bool overwrite)
    {
        return s_cache.cache(filePath, alias, overwrite);
    }

    void Texture2D::discard(const CacheAlias& alias)
    {
        s_cache.discard(alias);
    }

    const Ref<Texture2D>& Texture2D::get(const CacheAlias& alias)
    {
        return s_cache.get(alias);
    }

    const Ref<Texture2D>& Texture2D::rename(const CacheAlias& currentAlias, 
                                            const CacheAlias& newAlias, 
                                            bool overwrite)
    {
        return s_cache.rename(currentAlias, newAlias, overwrite);
    }

    const Ref<Texture3D>& Texture3D::cache(const std::string& filePath, 
                                           const CacheAlias& alias, 
                                           bool overwrite)
    {
        return s_cache.cache(filePath, alias, overwrite);
    }

    void Texture3D::discard(const CacheAlias& alias)
    {
        s_cache.discard(alias);
    }

    const Ref<Texture3D>& Texture3D::get(const CacheAlias& alias)
    {
        return s_cache.get(alias);
    }

    const Ref<Texture3D>& Texture3D::rename(const CacheAlias& currentAlias, 
                                            const CacheAlias& newAlias, 
                                            bool overwrite)
    {
        return s_cache.rename(currentAlias, newAlias, overwrite);
    }
}
