#include "oylpch.h"
#include "Shader.h"

#include "Platform/OpenGL/OpenGLShader.h"

#include "Rendering/Renderer.h"

namespace oyl
{
    Ref<Shader> Shader::s_invalid;
    
    std::unordered_map<CacheAlias, Ref<Shader>> Shader::s_cache;

    Ref<Shader> Shader::create(const std::vector<ShaderInfo>& infos)
    {
        switch (Renderer::getAPI())
        {
            case RendererAPI::OpenGL:
                return OpenGLShader::create(infos);
        }
        OYL_ASSERT(false, "Invalid API!");
        return nullptr;
    }

    Ref<Shader> Shader::create(const std::initializer_list<ShaderInfo>& files)
    {
        return create(std::vector<ShaderInfo>(files));
    }

    const Ref<Shader>& Shader::cache(const std::initializer_list<ShaderInfo>& files, 
                                     const CacheAlias& alias, 
                                     bool overwrite)
    {
        return cache(std::vector<ShaderInfo>(files), alias, overwrite);
    }

    const Ref<Shader>& Shader::cache(const std::vector<ShaderInfo>& infos,
                                     const CacheAlias& alias,
                                     bool overwrite)
    {
        auto it = s_cache.find(alias);
        if (it != s_cache.end())
        {
            if (!overwrite)
            {
                OYL_LOG_WARN("Shader {0} is already cached!", alias);
                return it->second;
            } else
            {
                it->second = Shader::create(infos);
                return it->second;
            }
        }

        s_cache[alias] = Shader::create(infos);
        return s_cache[alias];
    }

    const Ref<Shader>& Shader::cache(const Ref<Shader>& shader, const CacheAlias& alias, bool overwrite)
    {
        Ref<Shader> alreadyCached = nullptr;
        
        for (auto& kvp : s_cache)
        {
            if (kvp.second == shader)
            {
                alreadyCached = kvp.second;
            }
        }
        
        auto it = s_cache.find(alias);
        if (it != s_cache.end())
        {
            if (!overwrite)
            {
                OYL_LOG_WARN("Shader '{0}' is already cached!", alias);
                return it->second;
            } else
            {
                it->second = alreadyCached != nullptr ? alreadyCached : shader;
                return it->second;
            }
        }

        s_cache[alias] = shader;
        return shader;
    }

    void Shader::discard(const CacheAlias& alias)
    {
        auto it = s_cache.find(alias);
        if (it == s_cache.end())
        {
            OYL_LOG_WARN("Shader '{1}' could not be discarded because it does not exist.", alias);
        } else s_cache.erase(it);
    }

    const Ref<Shader>& Shader::get(const CacheAlias& alias)
    {
        // If the mesh has not been cached, return the default 'invalid' model
        if (s_cache.find(alias) == s_cache.end())
        {
            OYL_LOG_ERROR("Shader '{1}' not found!", alias);
            return s_invalid;
        }
        return s_cache.at(alias);
    }

    bool Shader::isCached(const Ref<Shader>& existing)
    {
        // Return true if the Ref is already cached, otherwise return false
        for (auto kvp : s_cache)
        {
            if (kvp.second == existing)
                return true;
        }
        return false;
    }
    
    const CacheAlias& Shader::getAlias(const Ref<Shader>& existing)
    {
        // If the Ref is already cached, return the first alias in the list.
        // Otherwise, return the 'invalid' alias
        for (const auto& kvp : s_cache)
        {
            if (kvp.second == existing)
                return kvp.first;
        }
        return INVALID_ALIAS;
    }

    bool Shader::exists(const CacheAlias& alias)
    {
        return s_cache.find(alias) != s_cache.end();
    }

    const Ref<Shader>& Shader::rename(const CacheAlias& currentAlias, 
                                      const CacheAlias& newAlias, 
                                      bool overwrite)
    {
        // The mesh has to exist to be renamed
        auto currIt = s_cache.find(currentAlias);
        if (currIt == s_cache.end())
        {
            OYL_LOG_ERROR("Shader '{1}' was not found!", currentAlias);
            return s_invalid;
        }

        // Special case if a mesh with alias $newAlias already exists
        auto newIt = s_cache.find(newAlias);
        if (newIt != s_cache.end())
        {
            if (newIt == currIt) return currIt->second;

            if (!overwrite)
            {
                OYL_LOG_ERROR("Failed to rename Shader '{1}', alias '{2}' already exists!",
                              currentAlias, newAlias);
                return currIt->second;
            }

            // Warn the user if they are overiding a mesh of a different type
            if (currIt->second != newIt->second)
            {
                OYL_LOG_WARN("Shader '{0}' was replaced by '{1}'.",
                             newAlias, currentAlias);
            }
        }

        // Delete the existing alias and create the new one
        s_cache[newAlias] = currIt->second;
        s_cache.erase(currIt);

        return s_cache[newAlias];
    }
}
