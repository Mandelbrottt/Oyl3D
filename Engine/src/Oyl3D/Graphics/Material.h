#pragma once

#include "Oyl3D/Graphics/Shader.h"
#include "Oyl3D/Graphics/Texture.h"

namespace oyl
{
    class Material
    {
        struct _Material {};

    public:
        explicit Material(_Material);
        explicit Material(_Material, std::string filepath);
        //explicit Material(_Material, Ref<Shader> shader);
        
        virtual ~Material() = default;

        bool operator==(const Material& material);

        static Ref<Material> create(const std::string& filepath);
        static Ref<Material> create();

        // TODO: Add ctor like shader has with ShaderInfo
        static const Ref<Material>& cache(const Ref<Material>& material,
                                          const CacheAlias& alias,
                                          bool overwrite = false);

        static const Ref<Material>& cache(const std::string& filepath,
                                          const CacheAlias& alias = "",
                                          bool overwrite = false);
        
        //static const Ref<Material>& cache(const Ref<Shader>& shader,
        //                                  const CacheAlias& alias,
        //                                  bool overwrite = false);

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
        
        void setUniform1i(const std::string& name, i32 value) { m_uniformInts[name] = value; }
        void setUniform1f(const std::string& name, f32 value) { m_uniformFloats[name] = value; }
        void setUniform3f(const std::string& name, glm::vec3 value) { m_uniformVec3s[name] = value; }
        void setUniform2f(const std::string& name, glm::vec2 value) { m_uniformVec2s[name] = value; }
        void setUniform4f(const std::string& name, glm::vec4 value) { m_uniformVec4s[name] = value; }
        void setUniformMat4(const std::string& name, glm::mat4 value) { m_uniformMat4s[name] = value; }
        void setUniformMat3(const std::string& name, glm::mat3 value) { m_uniformMat3s[name] = value; }
        void setUniformTexture1D(const std::string& name, Ref<Texture1D> value) { m_uniformTex1Ds[name] = std::move(value); }
        void setUniformTexture2D(const std::string& name, Ref<Texture2D> value) { m_uniformTex2Ds[name] = std::move(value); }
        void setUniformTexture3D(const std::string& name, Ref<Texture3D> value) { m_uniformTex3Ds[name] = std::move(value); }
        void setUniformCubeMap(const std::string& name, Ref<TextureCubeMap> value) { m_uniformTexCMs[name] = std::move(value); }

        const std::string& getFilePath() { return m_filepath; }

    public:
        struct TextureProps
        {
            glm::vec2 tiling = glm::vec2(1.0f);
            glm::vec2 offset = glm::vec2(0.0f);
        };
        
        Ref<Shader> shader;

        Ref<Texture2D> albedoMap;
        Ref<Texture2D> specularMap;
        Ref<Texture2D> normalMap;
        Ref<Texture2D> emissionMap;

        TextureProps mainTextureProps;

    private:

        std::string m_filepath;
        
        // TODO: Implement in custom data structure
        // TODO: Add textures
        std::unordered_map<std::string, Ref<Texture1D>>      m_uniformTex1Ds;
        std::unordered_map<std::string, Ref<Texture2D>>      m_uniformTex2Ds;
        std::unordered_map<std::string, Ref<Texture3D>>      m_uniformTex3Ds;
        std::unordered_map<std::string, Ref<TextureCubeMap>> m_uniformTexCMs;
        std::unordered_map<std::string, glm::mat4>           m_uniformMat4s;
        std::unordered_map<std::string, glm::mat3>           m_uniformMat3s;
        std::unordered_map<std::string, glm::vec4>           m_uniformVec4s;
        std::unordered_map<std::string, glm::vec3>           m_uniformVec3s;
        std::unordered_map<std::string, glm::vec2>           m_uniformVec2s;
        std::unordered_map<std::string, f32>                 m_uniformFloats;
        std::unordered_map<std::string, i32>                 m_uniformInts;

        static void init();
        
        static internal::AssetCache<Material> s_cache;

        friend Application;
    };
}
