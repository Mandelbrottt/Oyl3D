#pragma once

#include "Shader.h"

namespace oyl
{
    class Texture2D;

    class Material
    {
        struct _Material
        {
        };

    public:
        explicit Material(_Material);
        explicit Material(_Material, Ref<Shader> shader, Ref<Texture2D> texture = nullptr);
        explicit Material(_Material, Ref<Texture2D> texture);
        virtual  ~Material() = default;

        static Ref<Material> create(Ref<Shader> shader) { return Ref<Material>::create(_Material{}, std::move(shader)); }
        static Ref<Material> create() { return Ref<Material>::create(_Material{}); }

        virtual void bind();
        virtual void unbind();

        virtual void applyUniforms();

        void loadTexture(const std::string& filename);
        void loadTexture(Ref<Texture2D> texture);
        void unloadTexture();

        const Ref<Shader>& getShader() const { return m_shader; }
        const Ref<Texture2D>& getTexture() const { return m_texture; }

        void setShader(Ref<Shader> shader) { m_shader = shader; }
        void setTexture(Ref<Texture2D> texture) { m_texture = texture; }
        
        void setUniform1i(const std::string& name, i32 value) { m_uniformInts[name] = std::move(value); }
        void setUniform1f(const std::string& name, f32 value) { m_uniformFloats[name] = std::move(value); }
        void setUniform3f(const std::string& name, glm::vec3 value) { m_uniformVec3s[name] = std::move(value); }
        void setUniform2f(const std::string& name, glm::vec2 value) { m_uniformVec2s[name] = std::move(value); }
        void setUniform4f(const std::string& name, glm::vec4 value) { m_uniformVec4s[name] = std::move(value); }
        void setUniformMat4(const std::string& name, glm::mat4 value) { m_uniformMat4s[name] = std::move(value); }
    private:
        Ref<Shader>    m_shader;
        Ref<Texture2D> m_texture;
        
        // TODO: Implement in custom data structure
        std::unordered_map<std::string, glm::mat4> m_uniformMat4s;
        std::unordered_map<std::string, glm::vec4> m_uniformVec4s;
        std::unordered_map<std::string, glm::vec3> m_uniformVec3s;
        std::unordered_map<std::string, glm::vec2> m_uniformVec2s;
        std::unordered_map<std::string, f32>       m_uniformFloats;
        std::unordered_map<std::string, i32>       m_uniformInts;
    };
}
