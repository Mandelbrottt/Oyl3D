#pragma once

#include "Oyl3D/oylpch.h"

namespace oyl
{
    class FrameBuffer;
    
    namespace internal
    {
        class RenderSystem;
        class ShadowRenderSystem;
        class EditorRenderSystem;

        struct ColorLight
        {
            glm::vec3 ambient  = glm::vec3(0.2f);
            glm::vec3 diffuse  = glm::vec3(0.5f);
            glm::vec3 specular = glm::vec3(1.0f);
        };

        struct AttenuationLight
        {
            AttenuationLight()
                : attenuation(1.0f, 0.35f, 0.44f) {}

            void setRange(f32 range)
            {
                attenConst = 1.0f;
                attenLin   = 4.5f / range;
                attenQuad  = 75.0f / range;
            }

            union
            {
                struct
                {
                    f32 attenConst;
                    f32 attenLin;
                    f32 attenQuad;
                };
                
                glm::vec3 attenuation;
            };
        };

        struct PositionalLight
        {
            glm::vec3 offset = glm::vec3(0.0f);
        };

        struct ShadowLight
        {
            bool castShadows = true;
            
        private:
            Ref<FrameBuffer> m_frameBuffer;

            glm::mat4 m_lightSpaceMatrix;

            friend RenderSystem;
            friend ShadowRenderSystem;
            friend EditorRenderSystem;
        };
    }

    namespace component
    {
        struct PointLight
            : ::oyl::internal::ColorLight,
              ::oyl::internal::PositionalLight,
              ::oyl::internal::AttenuationLight,
              ::oyl::internal::ShadowLight {};
    
        struct DirectionalLight
            : ::oyl::internal::ColorLight,
              ::oyl::internal::ShadowLight
        {
            glm::vec3 direction = glm::vec3(0.0f);
        };
    
        struct SpotLight
            : DirectionalLight,
              ::oyl::internal::AttenuationLight,
              ::oyl::internal::ShadowLight
        {
            f32 innerCutoff = 10.0f;
            f32 outerCutoff = 15.0f;
        };
    }
}