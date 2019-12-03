#pragma once

#include "Oyl3D/Utils/AssetCache.h"

namespace oyl
{
    class Shader;
    class Texture2D;

    class Material
    {
        struct _Material {};

    public:
        explicit Material(_Material);
        explicit Material(_Material, Ref<Shader> shader);
        
        virtual ~Material() = default;

        static Ref<Material> create(const Ref<Shader>& shader);
        static Ref<Material> create();

        // TODO: Add ctor like shader has with ShaderInfo
        static const Ref<Material>& cache(const Ref<Material>& material,
                                          const CacheAlias& alias,
                                          bool overwrite = false);
        
        static const Ref<Material>& cache(const Ref<Shader>& shader,
                                          const CacheAlias& alias,
                                          bool overwrite = false);

        static void discard(const CacheAlias& alias);

        static const Ref<Material>& get(const CacheAlias& alias);

        static bool isCached(const Ref<Material>& existing);

        static bool exists(const CacheAlias& alias);

        static const CacheAlias& getAlias(const Ref<Material>& existing);
        
        static const Ref<Material>& rename(const CacheAlias& currentAlias,
                                           const CacheAlias& newAlias,
                                           bool overwrite = false);

        static const auto& getCache() { return s_cache.m_cache; }

        virtual void bind();
        virtual void unbind();

        virtual void applyUniforms();

        OYL_DEPRECATED("Access 'shader' directly instead.")
        const Ref<Shader>& getShader() const { return shader; }

        OYL_DEPRECATED("Access 'shader' directly instead.")
        void setShader(Ref<Shader> shader) { this->shader = std::move(shader); }

        OYL_DEPRECATED("Access 'albedoMap' directly instead")
        const Ref<Texture2D>& getAlbedoMap() const { return albedoMap; }

        OYL_DEPRECATED("Access 'specularMap' directly instead")
        const Ref<Texture2D>& getSpecularMap() const { return specularMap; }

        OYL_DEPRECATED("Access 'normalMap' directly instead")
        const Ref<Texture2D>& getNormalMap() const { return normalMap; }

        OYL_DEPRECATED("Access 'albedoMap' directly instead")
        void setAlbedoMap(Ref<Texture2D> albedo) { albedoMap = std::move(albedo); }

        OYL_DEPRECATED("Access 'specularMap' directly instead")
        void setSpecularMap(Ref<Texture2D> specular) { specularMap = std::move(specular); }

        OYL_DEPRECATED("Access 'normalMap' directly instead")
        void setNormalMap(Ref<Texture2D> normal) { normalMap = std::move(normal); }

        OYL_DEPRECATED("Access 'shader' directly instead")
        float getSpecularScalar() const { return specularScalar; }

        OYL_DEPRECATED("Access 'shader' directly instead")
        void  setSpecularScalar(float specular) { specularScalar = specular; }
        
        void setUniform1i(const std::string& name, i32 value) { m_uniformInts[name] = value; }
        void setUniform1f(const std::string& name, f32 value) { m_uniformFloats[name] = value; }
        void setUniform3f(const std::string& name, glm::vec3 value) { m_uniformVec3s[name] = value; }
        void setUniform2f(const std::string& name, glm::vec2 value) { m_uniformVec2s[name] = value; }
        void setUniform4f(const std::string& name, glm::vec4 value) { m_uniformVec4s[name] = value; }
        void setUniformMat4(const std::string& name, glm::mat4 value) { m_uniformMat4s[name] = value; }
        void setUniformMat3(const std::string& name, glm::mat3 value) { m_uniformMat3s[name] = value; }

    public:
        Ref<Shader> shader;

        Ref<Texture2D> albedoMap;
        Ref<Texture2D> specularMap;
        Ref<Texture2D> normalMap;
        
        float specularScalar = 0.5f;

    private:
        
        // TODO: Implement in custom data structure
        // TODO: Add textures
        std::unordered_map<std::string, glm::mat4> m_uniformMat4s;
        std::unordered_map<std::string, glm::mat3> m_uniformMat3s;
        std::unordered_map<std::string, glm::vec4> m_uniformVec4s;
        std::unordered_map<std::string, glm::vec3> m_uniformVec3s;
        std::unordered_map<std::string, glm::vec2> m_uniformVec2s;
        std::unordered_map<std::string, f32>       m_uniformFloats;
        std::unordered_map<std::string, i32>       m_uniformInts;
        
        static internal::AssetCache<Material> s_cache;
    };
}
