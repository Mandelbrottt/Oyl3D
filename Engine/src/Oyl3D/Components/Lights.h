#pragma once

#include "Oyl3D/oylpch.h"

namespace oyl
{
    class FrameBuffer;
    
    namespace internal
    {
        class PreRenderSystem;
        class ForwardRenderSystem;
        class DeferredRenderSystem;
        class ShadowRenderSystem;
        class EditorRenderSystem;

        struct ColorLight
        {
            glm::vec3 ambient  = glm::vec3(0.0f);
            glm::vec3 diffuse  = glm::vec3(0.5f);
            glm::vec3 specular = glm::vec3(1.0f);

            f32 intensity = 1.0f;
        };

        struct AttenuationLight
        {
            f32 range = 5.0f;
        };

        struct ShadowLight
        {
            ShadowLight() : m_frameBuffer(nullptr) {}
            ~ShadowLight() { m_frameBuffer = nullptr; }
            
            bool castShadows = false;

            glm::vec2 resolution = glm::vec2(1024.0f);

            glm::vec2 lowerBounds = glm::vec2(-10.0f, -10.0f);
            glm::vec2 upperBounds = glm::vec2(10.0f, 10.0f);

            float clipLength = 100.0f;

            float biasMin = 0.001f;
            float biasMax = 0.005f;
            
        private:
            glm::mat4 m_lightSpaceMatrix{};

            Ref<FrameBuffer> m_frameBuffer;

            friend PreRenderSystem;
            friend ForwardRenderSystem;
            friend DeferredRenderSystem;
            friend ShadowRenderSystem;
            friend EditorRenderSystem;
        };
    }
    
    enum class LightType
    {
        Point,
        Directional,
        Spot
    };

    namespace component
    {
        struct PointLight
            : ::oyl::internal::AttenuationLight,
              ::oyl::internal::ColorLight,
              ::oyl::internal::ShadowLight {};

        struct DirectionalLight
            : ::oyl::internal::ColorLight,
              ::oyl::internal::ShadowLight {};

        struct SpotLight
            : ::oyl::internal::AttenuationLight,
              DirectionalLight
        {
            f32 innerCutoff = 10.0f;
            f32 outerCutoff = 15.0f;
        };
    }
}