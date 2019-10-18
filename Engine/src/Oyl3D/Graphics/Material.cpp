#include "oylpch.h"
#include "Material.h"

#include "Graphics/Texture.h"

namespace oyl
{
    Material::Material(_Material)
        : m_shader(nullptr), m_texture(nullptr)
    {
    }

    Material::Material(_Material, Ref<Shader> shader, Ref<Texture2D> texture)
        : m_shader(std::move(shader)), m_texture(std::move(texture))
    {
    }

    Material::Material(_Material, Ref<Texture2D> texture)
        : m_shader(nullptr), m_texture(std::move(texture))
    {
    }

    void Material::bind()
    {
        m_shader->bind();
        m_texture->bind();
    }

    void Material::unbind()
    {
        m_shader->unbind();
        if (m_texture) m_texture->unbind();
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

    void Material::loadTexture(const std::string& filename)
    {
        unloadTexture();
        m_texture = Texture2D::create(filename);
    }

    void Material::loadTexture(Ref<Texture2D> texture)
    {
        unloadTexture();
        m_texture = std::move(texture);
    }

    void Material::unloadTexture()
    {
        m_texture.reset();
    }
}
