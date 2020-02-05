#pragma once

#include "Oyl3D/oylpch.h"

namespace oyl::component
{
    namespace internal
    {
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
    }

    struct PointLight
        : public internal::ColorLight,
          public internal::AttenuationLight {};
    
    struct DirectionalLight : public internal::ColorLight
    {
        glm::vec3 direction = glm::vec3(0.0f);
    };
    
    struct SpotLight
        : public internal::ColorLight,
          public internal::AttenuationLight
    {
        glm::vec3 direction;
        f32       angle;
    };
}