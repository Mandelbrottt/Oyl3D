#pragma once

#include "Oyl3D/Utils/AssetCache.h"

namespace oyl
{
    class Shader;
    class Texture;

    class Material
    {
        struct _Material
        {
        };

    public:
        explicit Material(_Material);
        explicit Material(_Material, Ref<Shader> shader, Ref<Texture> texture = nullptr);
        explicit Material(_Material, Ref<Texture> texture);
        
        virtual  ~Material() = default;

        static Ref<Material> create(const Ref<Shader>& shader, const Ref<Texture>& texture = nullptr);
        static Ref<Material> create(const Ref<Texture>& texture);
        static Ref<Material> create();

        static const Ref<Material>& cache(const Ref<Material>& material,
                                          const CacheAlias& alias,
                                          bool overwrite = false);

        static const Ref<Material>& cache(const Ref<Shader>& shader,
                                          const Ref<Texture>& texture,
                                          const CacheAlias& alias,
                                          bool overwrite = false);

        static void discard(const CacheAlias& alias);

        static const Ref<Material>& get(const CacheAlias& alias);

        static const Ref<Material>& rename(const CacheAlias& currentAlias,
                                           const CacheAlias& newAlias,
                                           bool overwrite = false);

        virtual void bind();
        virtual void unbind();

        virtual void applyUniforms();

        void loadTexture(const std::string& filename);
        void loadTexture(Ref<Texture> texture);
        void unloadTexture();

        const Ref<Shader>& getShader() const { return m_shader; }
        const Ref<Texture>& getTexture() const { return m_texture; }

        void setShader(Ref<Shader> shader) { m_shader = std::move(shader); }
        void setTexture(Ref<Texture> texture) { m_texture = std::move(texture); }
        
        void setUniform1i(const std::string& name, i32 value) { m_uniformInts[name] = value; }
        void setUniform1f(const std::string& name, f32 value) { m_uniformFloats[name] = value; }
        void setUniform3f(const std::string& name, glm::vec3 value) { m_uniformVec3s[name] = value; }
        void setUniform2f(const std::string& name, glm::vec2 value) { m_uniformVec2s[name] = value; }
        void setUniform4f(const std::string& name, glm::vec4 value) { m_uniformVec4s[name] = value; }
        void setUniformMat4(const std::string& name, glm::mat4 value) { m_uniformMat4s[name] = value; }
    private:
        Ref<Shader>  m_shader;
        Ref<Texture> m_texture;
        
        // TODO: Implement in custom data structure
        std::unordered_map<std::string, glm::mat4> m_uniformMat4s;
        std::unordered_map<std::string, glm::vec4> m_uniformVec4s;
        std::unordered_map<std::string, glm::vec3> m_uniformVec3s;
        std::unordered_map<std::string, glm::vec2> m_uniformVec2s;
        std::unordered_map<std::string, f32>       m_uniformFloats;
        std::unordered_map<std::string, i32>       m_uniformInts;
        
        static internal::AssetCache<Material> s_cache;
    };
}
