#pragma once

namespace oyl
{
    class Mesh;
}

namespace oyl::Component
{
    struct Transform
    {
        glm::vec3 position;
        glm::vec3 rotation;
        glm::vec3 scale;

        glm::mat4 getMatrix() const;
    };

    struct Model
    {
        Ref<Mesh> mesh;
    };
}
