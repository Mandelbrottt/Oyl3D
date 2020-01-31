#include "oylpch.h"
#include "Components/Camera.h"

#include "Graphics/Shader.h"

namespace oyl
{
    PostProcessingPass::PostProcessingPass(Ref<Shader> a_shader)
        : shader(std::move(a_shader)) {}

    void PostProcessingPass::applyUniforms()
    {
        for (const auto& kvp : m_uniformMat4s)
            shader->setUniformMat4(kvp.first, kvp.second);
        for (const auto& kvp : m_uniformMat3s)
            shader->setUniformMat3(kvp.first, kvp.second);
        for (const auto& kvp : m_uniformVec4s)
            shader->setUniform4f(kvp.first, kvp.second);
        for (const auto& kvp : m_uniformVec3s)
            shader->setUniform3f(kvp.first, kvp.second);
        for (const auto& kvp : m_uniformVec2s)
            shader->setUniform2f(kvp.first, kvp.second);
        for (const auto& kvp : m_uniformFloats)
            shader->setUniform1f(kvp.first, kvp.second);
        for (const auto& kvp : m_uniformInts)
            shader->setUniform1i(kvp.first, kvp.second);
    }
}
