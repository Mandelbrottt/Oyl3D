#pragma once

namespace oyl
{
    class Mesh;
    class Material;
    class Camera;
}

namespace oyl::component
{
    struct SceneObject
    {
        std::string name;
    };

    struct Transform
    {
        glm::vec3 position = glm::vec3(0.0f);
        glm::vec3 rotation = glm::vec3(0.0f);
        glm::vec3 scale    = glm::vec3(1.0f);

        glm::mat4 getMatrix() const;
    };

    struct Renderable
    {
        Ref<Mesh>     mesh;
        Ref<Material> material;
    };

    // TODO: Make more robust when adding physics
    struct RigidBody
    {
        Entity id;
    };

    struct PlayerCamera
    {
        int player = -1;
        Ref<Camera> camera;
    };

    namespace internal
    {
        struct EditorCamera
        {
            Ref<Camera> camera;
        };
    }
}
