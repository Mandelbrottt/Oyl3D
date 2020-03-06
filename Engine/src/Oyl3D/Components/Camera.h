#pragma once

#include "Transform.h"

#include "Oyl3D/Graphics/Texture.h"

namespace oyl
{
    class FrameBuffer;
    class Shader;

    namespace internal
    {      
        class PreRenderSystem;        
        class ForwardRenderSystem;        
        class DeferredRenderSystem;        
        class PostRenderSystem;        
        class GuiRenderSystem;        
        class UserPostRenderSystem;        
    }

    enum class PlayerNumber : u32
    {
        One = 0, Two, Three, Four
    };

    class PostProcessingPass
    {
    public:
        PostProcessingPass() = default;
        explicit PostProcessingPass(Ref<Shader> a_shader);

        Ref<Shader> shader = nullptr;
        
        void setUniform1i(const std::string& name, i32 value) { m_uniformInts[name] = value; }
        void setUniform1f(const std::string& name, f32 value) { m_uniformFloats[name] = value; }
        void setUniform3f(const std::string& name, glm::vec3 value) { m_uniformVec3s[name] = value; }
        void setUniform2f(const std::string& name, glm::vec2 value) { m_uniformVec2s[name] = value; }
        void setUniform4f(const std::string& name, glm::vec4 value) { m_uniformVec4s[name] = value; }
        void setUniformMat4(const std::string& name, glm::mat4 value) { m_uniformMat4s[name] = value; }
        void setUniformMat3(const std::string& name, glm::mat3 value) { m_uniformMat3s[name] = value; }
        void setUniformTexture1D(const std::string& name, Ref<Texture1D> value) { m_uniformTex1Ds[name] = std::move(value); }
        void setUniformTexture2D(const std::string& name, Ref<Texture2D> value) { m_uniformTex2Ds[name] = std::move(value); }
        void setUniformTexture3D(const std::string& name, Ref<Texture3D> value) { m_uniformTex3Ds[name] = std::move(value); }
        void setUniformCubeMap(const std::string& name, Ref<TextureCubeMap> value) { m_uniformTexCMs[name] = std::move(value); }

        void applyUniforms();

    private:
        std::unordered_map<std::string, i32>                 m_uniformInts;
        std::unordered_map<std::string, f32>                 m_uniformFloats;
        std::unordered_map<std::string, glm::vec2>           m_uniformVec2s;
        std::unordered_map<std::string, glm::vec3>           m_uniformVec3s;
        std::unordered_map<std::string, glm::vec4>           m_uniformVec4s;
        std::unordered_map<std::string, glm::mat3>           m_uniformMat3s;
        std::unordered_map<std::string, glm::mat4>           m_uniformMat4s;
        std::unordered_map<std::string, Ref<Texture1D>>      m_uniformTex1Ds;
        std::unordered_map<std::string, Ref<Texture2D>>      m_uniformTex2Ds;
        std::unordered_map<std::string, Ref<Texture3D>>      m_uniformTex3Ds;
        std::unordered_map<std::string, Ref<TextureCubeMap>> m_uniformTexCMs;
    };

    namespace component
    {
        class Camera
        {
        public:
            PlayerNumber player = PlayerNumber::One;

            u32 cullingMask = 0x0F;
    
            Ref<TextureCubeMap> skybox;

            std::vector<PostProcessingPass> postProcessingPasses;
            
            bool enabled = true;

            bool renderShadows = true; 

            bool doHDR = true;

            float exposure   = 1.0f;
            float brightness = 0.15f;
            float contrast   = 1.2f;
            float saturation = 1.5f;

            // Returns the current field of view
            float fov() const;

            // Sets the current field of view
            float fov(float a_fov);

            // Returns the current aspect ratio
            float aspect() const;

            // Sets the current aspect ratio
            float aspect(float a_aspect);

            // Returns the current near clipping distance
            float nearClipping() const;

            // Sets the current near clipping distance
            float nearClipping(float a_nearClipping);

            // Returns the current far clipping distance
            float farClipping() const;

            // Sets the current far clipping distance
            float farClipping(float a_farClipping);

            // Returns the current lower left corner of the camera's screen space coordinate system
            glm::vec2 lowerCoords() const;

            // Sets the current lower left corner of the camera's screen space coordinate system
            glm::vec2 lowerCoords(glm::vec2 a_lowerCoords);

            // Returns the current upper right corner of the camera's screen space coordinate system
            glm::vec2 upperCoords() const;

            // Sets the current upper right corner of the camera's screen space coordinate system
            glm::vec2 upperCoords(glm::vec2 a_upperCoords);

            Ref<const FrameBuffer> forwardFrameBuffer() { return m_mainFrameBuffer; }

            const glm::mat4& viewMatrix() const;
            const glm::mat4& projectionMatrix() const;
            const glm::mat4& viewProjectionMatrix() const;

            const glm::mat4& orthoMatrix() const;
            
            glm::vec3 worldToScreenSpace(const glm::vec3& a_pos) const;

            static void on_construct(entt::entity entity, entt::registry& registry, Camera& camera);
            
        private:
            float m_fov          = 60.0f;
            float m_aspect       = 16.0f / 9.0f;
            float m_nearClipping = 0.03f;
            float m_farClipping  = 1000.0f;

            glm::vec2 m_lowerCoords = { m_aspect * -5.0f, -5.0f };
            glm::vec2 m_upperCoords = { m_aspect *  5.0f,  5.0f };
            
            mutable glm::mat4 m_view           = glm::mat4(1.0f);
            mutable glm::mat4 m_projection     = glm::mat4(1.0f);
            mutable glm::mat4 m_viewProjection = glm::mat4(1.0f);

            mutable glm::mat4 m_ortho = glm::mat4(1.0f);

            entt::entity    m_owner    = entt::null;
            entt::registry* m_registry = nullptr;
            
            mutable bool m_projectionDirty = true;
            mutable bool m_orthoDirty = true;

            Ref<FrameBuffer> m_mainFrameBuffer = nullptr;
            Ref<FrameBuffer> m_deferredFrameBuffer = nullptr;

            friend ::oyl::internal::PreRenderSystem; 
            friend ::oyl::internal::PostRenderSystem; 
            friend ::oyl::internal::ForwardRenderSystem; 
            friend ::oyl::internal::DeferredRenderSystem; 
            friend ::oyl::internal::GuiRenderSystem; 
            friend ::oyl::internal::UserPostRenderSystem; 
        };

        inline float Camera::fov() const
        {
            return m_fov;
        }

        inline float Camera::fov(float a_fov)
        {
            m_projectionDirty |= m_fov != a_fov;
            return m_fov = a_fov;
        }

        inline float Camera::aspect() const
        {
            return m_aspect;
        }

        inline float Camera::aspect(float a_aspect)
        {
            m_projectionDirty |= m_aspect != a_aspect;
            return m_aspect = a_aspect;
        }

        inline float Camera::nearClipping() const
        {
            return m_nearClipping;
        }

        inline float Camera::nearClipping(float a_nearClipping)
        {
            m_projectionDirty |= m_nearClipping != a_nearClipping;
            return m_nearClipping = a_nearClipping;
        }

        inline float Camera::farClipping() const
        {
            return m_farClipping;
        }

        inline float Camera::farClipping(float a_farClipping)
        {
            m_projectionDirty |= m_farClipping != a_farClipping;
            return m_farClipping = a_farClipping;
        }

        inline glm::vec2 Camera::lowerCoords() const
        {
            return m_lowerCoords;
        }

        inline glm::vec2 Camera::lowerCoords(glm::vec2 a_lowerCoords)
        {
            m_orthoDirty |= m_lowerCoords != a_lowerCoords;
            return m_lowerCoords = a_lowerCoords;
        }

        inline glm::vec2 Camera::upperCoords() const
        {
            return m_upperCoords;
        }

        inline glm::vec2 Camera::upperCoords(glm::vec2 a_upperCoords)
        {
            m_orthoDirty |= m_upperCoords != a_upperCoords;
            return m_upperCoords = a_upperCoords;
        }

        inline const glm::mat4& Camera::viewMatrix() const
        {
            using component::Transform;

            Transform& t = m_registry->get<Transform>(m_owner);

            //if (t.isDirty())
            {
                m_view           = inverse(t.getMatrixGlobal());
                //m_viewProjection = projectionMatrix() * m_view;
            }

            return m_view;
        }
        
        inline const glm::mat4& Camera::projectionMatrix() const
        {
            if (m_projectionDirty)
            {
                m_projectionDirty = false;
                m_projection = glm::perspective(glm::radians(m_fov), m_aspect, m_nearClipping, m_farClipping);
                //m_viewProjection = m_projection * viewMatrix();
            }

            return m_projection;
        }

        inline const glm::mat4& Camera::viewProjectionMatrix() const
        {
            using component::Transform;

            Transform& t = m_registry->get<Transform>(m_owner);

            //bool isTDirty;
            //if ((isTDirty = t.isDirty()))
                m_view = inverse(t.getMatrixGlobal());

            if (m_projectionDirty)
                m_projection = glm::perspective(glm::radians(m_fov), m_aspect, m_nearClipping, m_farClipping);

            //if (m_projectionDirty)
                m_viewProjection = projectionMatrix() * viewMatrix();
            
            return m_viewProjection;
        }

        inline const glm::mat4& Camera::orthoMatrix() const
        {
            if (m_orthoDirty)
            {
                m_ortho = glm::ortho(m_lowerCoords.x, m_upperCoords.x, m_upperCoords.y, m_lowerCoords.y);
                m_orthoDirty = false;
            }

            return m_ortho;
        }

        inline glm::vec3 Camera::worldToScreenSpace(const glm::vec3& a_pos) const
        {
            glm::vec4 ndc = viewProjectionMatrix() * glm::vec4(a_pos, 1.0f);
            ndc /= ndc.w;
            ndc = (ndc + 1.0f) * 0.5f;
            return glm::vec3(m_lowerCoords, 0.0f) + ndc.xyz * glm::vec3(m_upperCoords - m_lowerCoords, 1.0f);
        }
        
        inline void Camera::on_construct(entt::entity entity, entt::registry& registry, Camera& camera)
        {
            camera.m_owner = entity;
            camera.m_registry = &registry;
        }
    }
}