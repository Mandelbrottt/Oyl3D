#pragma once

namespace oyl
{
    class Mesh;
    class Material;
}

namespace oyl::Component
{
    struct Transform
    {
        glm::vec3 position = glm::vec3(0.0f);
        glm::vec3 rotation = glm::vec3(0.0f);
        glm::vec3 scale    = glm::vec3(1.0f);

        glm::mat4 getMatrix() const;
    };

    struct MeshRenderer
    {
        Ref<Mesh>     mesh;
        Ref<Material> material;
    };
}
