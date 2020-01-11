#include "oylpch.h"
#include "Texture.h"

#include "Rendering/Renderer.h"

#include "Platform/OpenGL/OpenGLTexture.h"

namespace oyl
{
    internal::AssetCache<Texture1D>      Texture1D::s_cache;
    internal::AssetCache<Texture2D>      Texture2D::s_cache;
    internal::AssetCache<Texture3D>      Texture3D::s_cache;
    internal::AssetCache<TextureCubeMap> TextureCubeMap::s_cache;

    const char* internal::AssetCache<Texture1D>::s_typename      = "Texture1D";
    const char* internal::AssetCache<Texture2D>::s_typename      = "Texture2D";
    const char* internal::AssetCache<Texture3D>::s_typename      = "Texture3D";
    const char* internal::AssetCache<TextureCubeMap>::s_typename = "TextureCubeMap";
    
    Ref<Texture1D> Texture1D::create(const std::string& filePath)
    {
        switch (Renderer::getAPI())
        {
            case RendererAPI::OpenGL:
                auto ogl = Ref<OpenGLTexture1D>::create(filePath);
                return ogl->isLoaded() ? ogl : s_cache.get(INVALID_ALIAS);
        }
        return nullptr;
    }

    Ref<Texture2D> Texture2D::create(const std::string& filePath)
    {
        switch (Renderer::getAPI())
        {
            case RendererAPI::OpenGL:
                auto ogl = Ref<OpenGLTexture2D>::create(filePath);
                return ogl->isLoaded() ? ogl : s_cache.get(INVALID_ALIAS);
        }
        return nullptr;
    }

    Ref<Texture3D> Texture3D::create(const std::string& filePath)
    {
        switch (Renderer::getAPI())
        {
            case RendererAPI::OpenGL:
                auto ogl = Ref<OpenGLTexture3D>::create(filePath);
                return ogl->isLoaded() ? ogl : s_cache.get(INVALID_ALIAS);
        }
        return nullptr;
    }

    Ref<TextureCubeMap> TextureCubeMap::create(const std::string& filePath)
    {
        switch (Renderer::getAPI())
        {
            case RendererAPI::OpenGL:
                auto ogl = Ref<OpenGLTextureCubeMap>::create(filePath);
                return ogl->isLoaded() ? ogl : s_cache.get(INVALID_ALIAS);
        }
        return nullptr;
    }

    void Texture::init()
    {
        //Texture1D::cache(INVALID_TEXTURE_PATH, INVALID_ALIAS);
        Texture2D::cache(ENGINE_RES + INVALID_TEXTURE_PATH, INVALID_ALIAS);
        //Texture3D::cache(INVALID_TEXTURE_PATH, INVALID_ALIAS);
    }

    const Ref<Texture1D>& Texture1D::cache(const std::string& filePath, 
                                           const CacheAlias& alias, 
                                           bool overwrite)
    {
        return s_cache.cache(filePath, alias, overwrite);
    }

    const Ref<Texture1D>& Texture1D::cache(const Ref<Texture1D>& existing, 
                                           const CacheAlias&     alias, 
                                           bool  overwrite)
    {
        return s_cache.cache(existing, alias, overwrite);
    }

    void Texture1D::discard(const CacheAlias& alias)
    {
        s_cache.discard(alias);
    }

    const Ref<Texture1D>& Texture1D::get(const CacheAlias& alias)
    {
        return s_cache.get(alias);
    }

    bool Texture1D::isCached(const Ref<Texture1D>& existing)
    {
        return s_cache.isCached(existing);
    }

    bool Texture1D::exists(const CacheAlias& alias)
    {
        return s_cache.exists(alias);
    }

    const CacheAlias& Texture1D::getAlias(const Ref<Texture1D>& existing)
    {
        return s_cache.getAlias(existing);
    }

    const Ref<Texture1D>& Texture1D::rename(const CacheAlias& currentAlias, 
                                            const CacheAlias& newAlias, 
                                            bool overwrite)
    {
        return s_cache.rename(currentAlias, newAlias, overwrite);
    }

    const Ref<Texture2D>& Texture2D::cache(const std::string& filePath, 
                                           const CacheAlias&  alias, 
                                           bool overwrite)
    {
        return s_cache.cache(filePath, alias, overwrite);
    }

    const Ref<Texture2D>& Texture2D::cache(const Ref<Texture2D>& existing, 
                                           const CacheAlias&     alias, 
                                           bool  overwrite)
    {
        return s_cache.cache(existing, alias, overwrite);
    }

    void Texture2D::discard(const CacheAlias& alias)
    {
        s_cache.discard(alias);
    }

    const Ref<Texture2D>& Texture2D::get(const CacheAlias& alias)
    {
        return s_cache.get(alias);
    }

    bool Texture2D::isCached(const Ref<Texture2D>& existing)
    {
        return s_cache.isCached(existing);
    }

    bool Texture2D::exists(const CacheAlias& alias)
    {
        return s_cache.exists(alias);
    }

    const CacheAlias& Texture2D::getAlias(const Ref<Texture2D>& existing)
    {
        return s_cache.getAlias(existing);
    }

    const Ref<Texture2D>& Texture2D::rename(const CacheAlias& currentAlias, 
                                            const CacheAlias& newAlias, 
                                            bool overwrite)
    {
        return s_cache.rename(currentAlias, newAlias, overwrite);
    }

    const Ref<Texture3D>& Texture3D::cache(const std::string& filePath, 
                                           const CacheAlias&  alias, 
                                           bool overwrite)
    {
        return s_cache.cache(filePath, alias, overwrite);
    }

    const Ref<Texture3D>& Texture3D::cache(const Ref<Texture3D>& existing, 
                                           const CacheAlias&     alias, 
                                           bool  overwrite)
    {
        return s_cache.cache(existing, alias, overwrite);
    }

    void Texture3D::discard(const CacheAlias& alias)
    {
        s_cache.discard(alias);
    }

    const Ref<Texture3D>& Texture3D::get(const CacheAlias& alias)
    {
        return s_cache.get(alias);
    }

    bool Texture3D::isCached(const Ref<Texture3D>& existing)
    {
        return s_cache.isCached(existing);
    }

    bool Texture3D::exists(const CacheAlias& alias)
    {
        return s_cache.exists(alias);
    }

    const CacheAlias& Texture3D::getAlias(const Ref<Texture3D>& existing)
    {
        return s_cache.getAlias(existing);
    }

    const Ref<Texture3D>& Texture3D::rename(const CacheAlias& currentAlias, 
                                            const CacheAlias& newAlias, 
                                            bool overwrite)
    {
        return s_cache.rename(currentAlias, newAlias, overwrite);
    }
    
    const Ref<TextureCubeMap>& TextureCubeMap::cache(const std::string& filePath, 
                                                     const CacheAlias&  alias, 
                                                     bool overwrite)
    {
        return s_cache.cache(filePath, alias, overwrite);
    }

    const Ref<TextureCubeMap>& TextureCubeMap::cache(const Ref<TextureCubeMap>& existing, 
                                                     const CacheAlias&          alias, 
                                                     bool overwrite)
    {
        return s_cache.cache(existing, alias, overwrite);
    }

    void TextureCubeMap::discard(const CacheAlias& alias)
    {
        s_cache.discard(alias);
    }

    const Ref<TextureCubeMap>& TextureCubeMap::get(const CacheAlias& alias)
    {
        return s_cache.get(alias);
    }

    bool TextureCubeMap::isCached(const Ref<TextureCubeMap>& existing)
    {
        return s_cache.isCached(existing);
    }

    bool TextureCubeMap::exists(const CacheAlias& alias)
    {
        return s_cache.exists(alias);
    }

    const CacheAlias& TextureCubeMap::getAlias(const Ref<TextureCubeMap>& existing)
    {
        return s_cache.getAlias(existing);
    }

    const Ref<TextureCubeMap>& TextureCubeMap::rename(const CacheAlias& currentAlias, 
                                                      const CacheAlias& newAlias, 
                                                      bool overwrite)
    {
        return s_cache.rename(currentAlias, newAlias, overwrite);
    }
}
