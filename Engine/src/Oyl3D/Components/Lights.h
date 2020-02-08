#pragma once

#include "Oyl3D/oylpch.h"

namespace oyl
{
    class FrameBuffer;
    
    namespace internal
    {
        class RenderSystem;
        class ShadowRenderSystem;

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
            glm::vec3 offset;
        };

        struct FrameBufferLight
        {
            bool castShadows = true;
            
        private:
            bool m_didCastShadows = false;
            
            Ref<FrameBuffer> m_frameBuffer;

            friend RenderSystem;
            friend ShadowRenderSystem;
        };
    }

    namespace component
    {
        struct PointLight
            : internal::ColorLight,
              internal::PositionalLight,
              internal::AttenuationLight,
              internal::FrameBufferLight {};
    
        struct DirectionalLight
            : internal::ColorLight,
              internal::FrameBufferLight
        {
            glm::vec3 direction = glm::vec3(0.0f);
        };
    
        struct SpotLight
            : DirectionalLight,
              internal::AttenuationLight,
              internal::FrameBufferLight
        {
            f32 angle;
        };
    }
}