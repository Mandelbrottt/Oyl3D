#include "oylpch.h"
#include "Graphics/EditorCamera.h"

namespace oyl
{
    const glm::mat4& EditorCamera::getViewMatrix() const
    {
        return m_view;
    }

    glm::mat4 EditorCamera::getViewProjectionMatrix() const
    {
        return m_projection * m_view;
    }

    void EditorCamera::setPosition(glm::vec3 position)
    {
        glm::vec3 temp = glm::vec4(-glm::mat3(m_view) * position, 1.0f);

        transX = temp.x;
        transY = temp.y;
        transZ = temp.z;

        m_position = position;
    }

    void EditorCamera::setRotation(const glm::quat& rot)
    {
        m_view = glm::mat4(1.0f);

        move(m_position);
        m_view = glm::mat4_cast(rot) * m_view;
    }

    void EditorCamera::setRotation(glm::vec3 rot)
    {
        rot = glm::radians(rot);

        m_view = glm::mat4(1.0f);

        move(m_position);

        glm::mat4 temp = glm::mat4(1.0f);
        temp = glm::rotate(temp, rot.z, glm::vec3(0.0f, 0.0f, 1.0f));
        temp = glm::rotate(temp, rot.x, glm::vec3(1.0f, 0.0f, 0.0f));
        temp = glm::rotate(temp, rot.y, glm::vec3(0.0f, 1.0f, 0.0f));

        m_view = temp * m_view;
    }

    void EditorCamera::lookAt(const glm::vec3& target, const glm::vec3& up)
    {
        m_view = glm::lookAt(m_position, target, up);
    }

    void EditorCamera::rotate(const glm::quat& rot)
    {
        if (rot != glm::quat(glm::vec3(0.0f)))
        {
            m_view = glm::mat4_cast(rot) * m_view;
        }
    }

    void EditorCamera::rotate(glm::vec3 rot)
    {
        rot = glm::radians(rot);
        rotate(glm::quat(rot));
    }
    
    void EditorCamera::move(const glm::vec3& local)
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
