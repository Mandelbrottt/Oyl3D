#pragma once

#include "Shader.h"

namespace oyl
{
    class OYLAPI Material
    {
    public:
        explicit Material(Ref<Shader> shader);
        virtual ~Material() = default;

        const Ref<Shader>& getShader() { return m_shader; }

        virtual void applyUniforms();

        void setUniform1i(const std::string& name, i32 value) { m_uniformInts[name] = std::move(value); }
        void setUniform1f(const std::string& name, f32 value) { m_uniformFloats[name] = std::move(value); }
        void setUniform3f(const std::string& name, glm::vec3 value) { m_uniformVec3s[name] = std::move(value); }
        void setUniform2f(const std::string& name, glm::vec2 value) { m_uniformVec2s[name] = std::move(value); }
        void setUniform4f(const std::string& name, glm::vec4 value) { m_uniformVec4s[name] = std::move(value); }
        void setUniformMat4(const std::string& name, glm::mat4 value) { m_uniformMat4s[name] = std::move(value); }
    private:
        Ref<Shader> m_shader;

        // TODO: Implement in custom data structure
        std::unordered_map<std::string, glm::mat4> m_uniformMat4s;
        std::unordered_map<std::string, glm::vec4> m_uniformVec4s;
        std::unordered_map<std::string, glm::vec3> m_uniformVec3s;
        std::unordered_map<std::string, glm::vec2> m_uniformVec2s;
        std::unordered_map<std::string, f32> m_uniformFloats;
        std::unordered_map<std::string, i32> m_uniformInts;
    };
}