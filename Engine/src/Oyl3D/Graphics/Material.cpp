#include "oylpch.h"
#include "Material.h"

namespace oyl
{
    Material::Material(_Material)
        : m_shader(nullptr)
    {
    }

    Material::Material(_Material, Ref<Shader> shader)
        : m_shader(std::move(shader))
    {
    }

    void Material::applyUniforms()
    {
        for (auto kvp : m_uniformMat4s)
            m_shader->setUniformMat4(kvp.first, kvp.second);
        for (auto kvp : m_uniformVec4s)
            m_shader->setUniform4f(kvp.first, kvp.second);
        for (auto kvp : m_uniformVec3s)
            m_shader->setUniform3f(kvp.first, kvp.second);
        for (auto kvp : m_uniformVec2s)
            m_shader->setUniform2f(kvp.first, kvp.second);
        for (auto kvp : m_uniformFloats)
            m_shader->setUniform1f(kvp.first, kvp.second);
        for (auto kvp : m_uniformInts)
            m_shader->setUniform1i(kvp.first, kvp.second);
    }
}
