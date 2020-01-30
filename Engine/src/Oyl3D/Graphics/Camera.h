#pragma once
#include <glm/common.hpp>

namespace oyl
{
    class EditorCamera
    {
    public:
        explicit EditorCamera()  = default;
        virtual  ~EditorCamera() = default;

        const glm::mat4& getViewMatrix() const;
        glm::mat4        getViewProjectionMatrix() const;

        inline const glm::mat4& getProjectionMatrix() const { return m_projection; }

        inline const glm::vec3& getPosition() const { return m_position; };
        void                    setPosition(glm::vec3 position);

        inline glm::quat getRotation() const { return glm::quat_cast(glm::mat3(m_view)); }

        void setRotation(const glm::quat& rot);
        void setRotation(glm::vec3 rot);

        inline void setProjection(glm::mat4 proj) { m_projection = proj; }

        inline glm::vec3 getForward() const { return glm::vec3(frontX, frontY, -frontZ); }
        inline glm::vec3 getUp() const { return glm::vec3(upX, upY, -upZ); }
        inline glm::vec3 getRight() const { return glm::vec3(rightX, rightY, -rightZ); }

        void lookAt(const glm::vec3& target, const glm::vec3& up = glm::vec3(0, 1, 0));

        void rotate(const glm::quat& rot);
        void rotate(glm::vec3 rot);
        
        void move(const glm::vec3& local);

    protected:
        glm::mat4         m_projection     = glm::mat4(1.0f);
        mutable glm::mat4 m_viewProjection = glm::mat4(1.0f);

        glm::vec3 m_position = glm::vec3(0.0f);

        union
        {
            mutable glm::mat4 m_view = glm::mat4(1.0f);

            struct
            {
                float rightX, rightY, rightZ, m03;
                float upX,    upY,    upZ,    m13;
                float frontX, frontY, frontZ, m23;
                float transX, transY, transZ, m33;
            };
        };
    };
}
