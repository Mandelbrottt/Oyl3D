#pragma once

#include "Oyl3D/oylpch.h"

#include "Transform.h"

namespace oyl
{
    class TextureCubeMap;

    enum class PlayerNumber : u32
    {
        One = 0, Two, Three, Four
    };

    namespace component
    {
        class PlayerCamera
        {
        public:
            bool enabled = true;
            
            PlayerNumber player = PlayerNumber::One;

            u32 cullingMask = 0xFFFFFFFF;
    
            Ref<TextureCubeMap> skybox;

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

            const glm::mat4& viewMatrix() const;
            const glm::mat4& projectionMatrix() const;
            const glm::mat4& viewProjectionMatrix() const;

            const glm::mat4& orthoMatrix() const;
            
            glm::vec3 worldToScreenSpace(const glm::vec3& a_pos) const;

            static void on_construct(entt::entity entity, entt::registry& registry, PlayerCamera& camera);
            
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
        };

        inline float PlayerCamera::fov() const
        {
            return m_fov;
        }

        inline float PlayerCamera::fov(float a_fov)
        {
            m_projectionDirty |= m_fov != a_fov;
            return m_fov = a_fov;
        }

        inline float PlayerCamera::aspect() const
        {
            return m_aspect;
        }

        inline float PlayerCamera::aspect(float a_aspect)
        {
            m_projectionDirty |= m_aspect != a_aspect;
            return m_aspect = a_aspect;
        }

        inline float PlayerCamera::nearClipping() const
        {
            return m_nearClipping;
        }

        inline float PlayerCamera::nearClipping(float a_nearClipping)
        {
            m_projectionDirty |= m_nearClipping != a_nearClipping;
            return m_nearClipping = a_nearClipping;
        }

        inline float PlayerCamera::farClipping() const
        {
            return m_farClipping;
        }

        inline float PlayerCamera::farClipping(float a_farClipping)
        {
            m_projectionDirty |= m_farClipping != a_farClipping;
            return m_farClipping = a_farClipping;
        }

        inline glm::vec2 PlayerCamera::lowerCoords() const
        {
            return m_lowerCoords;
        }

        inline glm::vec2 PlayerCamera::lowerCoords(glm::vec2 a_lowerCoords)
        {
            m_orthoDirty |= m_lowerCoords != a_lowerCoords;
            return m_lowerCoords = a_lowerCoords;
        }

        inline glm::vec2 PlayerCamera::upperCoords() const
        {
            return m_upperCoords;
        }

        inline glm::vec2 PlayerCamera::upperCoords(glm::vec2 a_upperCoords)
        {
            m_orthoDirty |= m_upperCoords != a_upperCoords;
            return m_upperCoords = a_upperCoords;
        }

        inline const glm::mat4& PlayerCamera::viewMatrix() const
        {
            using component::Transform;

            Transform& t = m_registry->get<Transform>(m_owner);

            if (t.isDirty())
            {
                m_view           = inverse(t.getMatrixGlobal());
                m_viewProjection = projectionMatrix() * m_view;
            }

            return m_view;
        }
        
        inline const glm::mat4& PlayerCamera::projectionMatrix() const
        {
            if (m_projectionDirty)
            {
                m_projection = glm::perspective(glm::radians(m_fov), m_aspect, m_nearClipping, m_farClipping);
                m_viewProjection = m_projection * viewMatrix();
                m_projectionDirty = false;
            }

            return m_projection;
        }

        inline const glm::mat4& PlayerCamera::viewProjectionMatrix() const
        {
            using component::Transform;

            Transform& t = m_registry->get<Transform>(m_owner);

            bool isTDirty;
            if ((isTDirty = t.isDirty()))
                m_view = inverse(t.getMatrixGlobal());

            if (m_projectionDirty)
                m_projection = glm::perspective(m_fov, m_aspect, m_nearClipping, m_farClipping);

            if (isTDirty || m_projectionDirty)
                m_viewProjection = m_projection * m_view;
            
            return m_viewProjection;
        }

        inline const glm::mat4& PlayerCamera::orthoMatrix() const
        {
            if (m_orthoDirty)
            {
                m_ortho = glm::ortho(m_lowerCoords.x, m_upperCoords.x, m_upperCoords.y, m_lowerCoords.y);
                m_orthoDirty = false;
            }

            return m_ortho;
        }

        inline glm::vec3 PlayerCamera::worldToScreenSpace(const glm::vec3& a_pos) const
        {
            glm::vec4 ndc = viewProjectionMatrix() * glm::vec4(a_pos, 1.0f);
            ndc /= ndc.w;
            ndc = (ndc + 1.0f) * 0.5f;
            return glm::vec3(m_lowerCoords, 0.0f) + ndc.xyz * glm::vec3(m_upperCoords - m_lowerCoords, 0.0f);
        }
        
        inline void PlayerCamera::on_construct(entt::entity entity, entt::registry& registry, PlayerCamera& camera)
        {
            camera.m_owner = entity;
            camera.m_registry = &registry;
        }
    }
}