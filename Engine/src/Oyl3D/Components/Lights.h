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
            ShadowLight() : m_frameBuffer(nullptr) {}
            ~ShadowLight() { m_frameBuffer = nullptr; }
            
            bool castShadows = true;
            
        private:
            glm::mat4 m_lightSpaceMatrix{};

            Ref<FrameBuffer> m_frameBuffer;

            friend RenderSystem;
            friend ShadowRenderSystem;
            friend EditorRenderSystem;
        };
    }

    struct PointLight
        : internal::ColorLight,
          internal::ShadowLight,
          internal::PositionalLight,
          internal::AttenuationLight {};
    
    struct DirectionalLight
        : internal::ColorLight,
          internal::ShadowLight
    {
        glm::vec3 direction = glm::vec3(0.0f);
    };
    
    struct SpotLight
        : DirectionalLight,
        internal::AttenuationLight
    {
        f32 innerCutoff = 10.0f;
        f32 outerCutoff = 15.0f;
    };
        

    enum class LightType
    {
        Point,
        Directional,
        Spot
    };

    namespace component
    {
        class LightSource
        {
        public:
            struct LightInfo
            {
                LightInfo();
                explicit LightInfo(LightType type);
                LightInfo(const LightInfo& info);
                ~LightInfo() {}

                LightInfo& operator=(const LightInfo& info);
                
                union
                {
                    PointLight       point;
                    DirectionalLight directional;
                    SpotLight        spot;
                };

                LightType type = LightType::Point;

            private:
                void defaultInit();
            };

            LightInfo& pushLight(LightInfo info);
            LightInfo& pushLight(LightType type);
            void eraseShape(u32 index);
            
            LightInfo& getLight(u32 index);
            const LightInfo& getLight(u32 index) const;
            
            std::vector<LightInfo>& getLights();
            const std::vector<LightInfo>& getLights() const;
            
            std::size_t size() const;
            bool empty() const;
            
            std::vector<LightInfo>::iterator begin();
            std::vector<LightInfo>::iterator end();
                        
            std::vector<LightInfo>::const_iterator begin() const;
            std::vector<LightInfo>::const_iterator end()   const;

        private:
            std::vector<LightInfo> m_lights;
        };
    }
}