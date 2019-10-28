#include "oylpch.h"
#include "Component.h"

#include "Graphics/Mesh.h"

namespace oyl::component
{
    glm::mat4 Transform::getMatrix() const
    {
        glm::mat4 ret(1.0f);

        ret = glm::translate(ret, position);
        ret = glm::scale(ret, scale);
        ret = glm::rotate(ret, glm::radians(rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
        ret = glm::rotate(ret, glm::radians(rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
        ret = glm::rotate(ret, glm::radians(rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));

        return ret;
    }
}
