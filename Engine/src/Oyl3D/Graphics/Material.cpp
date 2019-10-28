#include "oylpch.h"
#include "Material.h"

#include "Graphics/Texture.h"
#include "Graphics/Shader.h"

namespace oyl
{
    internal::AssetCache<Material> Material::s_cache;
    
    Material::Material(_Material)
        : m_shader(nullptr), m_albedo(nullptr)
    {
    }

    Material::Material(_Material, Ref<Shader> shader, Ref<Texture> texture)
        : m_shader(std::move(shader)), m_albedo(std::move(texture))
    {
    }

    Material::Material(_Material, Ref<Texture> texture)
        : m_shader(nullptr), m_albedo(std::move(texture))
    {
    }

    Ref<Material> Material::create(const Ref<Shader>& shader, const Ref<Texture>& texture)
    {
        return Ref<Material>::create(_Material{}, shader, texture);
    }

    Ref<Material> Material::create(const Ref<Texture>& texture)
    {
        return Ref<Material>::create(_Material{}, std::move(texture));
    }

    Ref<Material> Material::create()
    {
        return Ref<Material>::create(_Material{});
    }

    template<>
    const Ref<Material>& internal::AssetCache<Material>::cache(const Ref<Material>& existing,
                                                               const CacheAlias& alias,
                                                               bool overwrite)
    {
        Ref<Material> alreadyCached = nullptr;

        for (auto& kvp : m_cache)
        {
            if (kvp.second == existing ||
                (kvp.second->getShader() == existing->getShader() &&
                 kvp.second->getAlbedoMap() == existing->getAlbedoMap()))
            {
                alreadyCached = kvp.second;
            }
        }

        auto it = m_cache.find(alias);
        if (it != m_cache.end())
        {
            if (!overwrite)
            {
                OYL_LOG_ERROR("Material '{1}' is already cached!", alias);
                return it->second;
            }

            it->second = alreadyCached;
            return it->second;
        }
        
        m_cache[alias] = existing != nullptr ? existing : Material::create();
        return m_cache[alias];
    }

    const Ref<Material>& Material::cache(const Ref<Material>& material, 
                                         const CacheAlias& alias, 
                                         bool overwrite)
    {
        return s_cache.cache(material, alias, overwrite);
    }

    const Ref<Material>& Material::cache(const Ref<Shader>& shader, 
                                         const Ref<Texture>& texture, 
                                         const CacheAlias& alias, 
                                         bool overwrite)
    {
        auto mat = Material::create(shader, texture);
        return s_cache.cache(mat, alias, overwrite);
    }

    void Material::discard(const CacheAlias& alias)
    {
        s_cache.discard(alias);
    }

    const Ref<Material>& Material::get(const CacheAlias& alias)
    {
        return s_cache.get(alias);
    }

    const Ref<Material>& internal::AssetCache<Material>::rename(const CacheAlias& currentAlias,
                                                                const CacheAlias& newAlias,
                                                                bool overwrite)
    {
        // The mesh has to exist to be renamed
        auto currIt = m_cache.find(currentAlias);
        if (currIt == m_cache.end())
        {
            OYL_LOG_ERROR("Material '{1}' was not found!", currentAlias);
            return this->get(INVALID_ALIAS);
        }

        // Special case if a mesh with alias $newAlias already exists
        auto newIt = m_cache.find(newAlias);
        if (newIt != m_cache.end())
        {
            if (newIt == currIt) return currIt->second;

            if (!overwrite)
            {
                OYL_LOG_ERROR("Failed to rename Material '{1}', alias '{2}' already exists!",
                              currentAlias, newAlias);
                return currIt->second;
            }

            // Warn the user if they are overiding a mesh of a different type
            if (currIt->second->getShader() != newIt->second->getShader() ||
                currIt->second->getAlbedoMap() != newIt->second->getAlbedoMap())
            {
                OYL_LOG_WARN("Material '{0}' was replaced by '{1}'.",
                             newIt->first, currIt->first, newAlias);
            }
        }

        // Delete the existing alias and create the new one
        m_cache[newAlias] = currIt->second;
        m_cache.erase(currIt);

        return m_cache[newAlias];
    }
    
    const Ref<Material>& Material::rename(const CacheAlias& currentAlias, const CacheAlias& newAlias, bool overwrite)
    {
        return s_cache.rename(currentAlias, newAlias, overwrite);
    }

    void Material::bind()
    {
        if (m_shader)
            m_shader->bind();

        if (m_albedo)
            m_albedo->bind(0);
        setUniform1i("u_material.albedo", 0);

        if (m_specular)
            m_specular->bind(1);
        setUniform1i("u_material.specular", 1);

        //if (m_normal)
        //    m_normal->bind(2);
        //setUniform1i("u_material.normal", 2);
    }

    void Material::unbind()
    {
        if (m_shader) 
            m_shader->unbind();

        if (m_albedo)
            m_albedo->unbind();

        if (m_specular)
            m_specular->unbind();

        //if (m_normal)
        //    m_normal->unbind();
    }

    void Material::applyUniforms()
    {
        for (const auto& kvp : m_uniformMat4s)
            m_shader->setUniformMat4(kvp.first, kvp.second);
        for (const auto& kvp : m_uniformMat3s)
            m_shader->setUniformMat3(kvp.first, kvp.second);
        for (const auto& kvp : m_uniformVec4s)
            m_shader->setUniform4f(kvp.first, kvp.second);
        for (const auto& kvp : m_uniformVec3s)
            m_shader->setUniform3f(kvp.first, kvp.second);
        for (const auto& kvp : m_uniformVec2s)
            m_shader->setUniform2f(kvp.first, kvp.second);
        for (const auto& kvp : m_uniformFloats)
            m_shader->setUniform1f(kvp.first, kvp.second);
        for (const auto& kvp : m_uniformInts)
            m_shader->setUniform1i(kvp.first, kvp.second);
    }

    void Material::loadTexture(const std::string& filename)
    {
        unloadTexture();
        m_albedo = Texture2D::create(filename);
    }

    void Material::loadTexture(Ref<Texture> texture)
    {
        unloadTexture();
        m_albedo = std::move(texture);
    }

    void Material::unloadTexture()
    {
        m_albedo.reset();
    }

    template<> [[deprecated]]
    const Ref<Material>& internal::AssetCache<Material>::cache(const std::string& filePath,
                                                               CacheAlias alias,
                                                               bool overwrite)
    {
        OYL_ASSERT("How are you running this");
        return nullptr;
    }
}
