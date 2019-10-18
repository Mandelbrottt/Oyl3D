#include "oylpch.h"
#include "Camera.h"

namespace oyl
{
    const glm::mat4& Camera::getViewMatrix() const
    {
        return m_view;
    }

    glm::mat4 Camera::getViewProjectionMatrix() const
    {
        return m_projection * m_view;
    }

    void Camera::setPosition(glm::vec3 position)
    {
        glm::vec3 temp = glm::vec4(-glm::mat3(m_view) * position, 1.0f);
        
        transX = temp.x;
        transY = temp.y;
        transZ = temp.z;

        m_position = position;
    }

    void Camera::lookAt(const glm::vec3& target, const glm::vec3& up)
    {
        m_view = glm::lookAt(m_position, target, up);
    }

    void Camera::rotate(const glm::quat& rot)
    {
        if (rot != glm::quat(glm::vec3(0.0f)))
        {
            m_view = glm::mat4_cast(rot) * m_view;
        }
    }

    void Camera::move(const glm::vec3& local)
    {
        if (local != glm::vec3(0.0f))
        {
            transX -= local.x;
            transY -= local.y;
            transZ -= local.z;

            m_position = -glm::inverse(glm::mat3(m_view)) * glm::vec3(transX, transY, transZ);
        }
    }
}
