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

    // TODO: Distinction between local and global
    // TODO: Change access to functions
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
    class btRigidBody;
    
    struct RigidBody
    {
        Entity id = Entity(-1);
        OylEnum type = OylEnum::None;

        glm::vec3 velocity     = { 0, 0, 0 };
        glm::vec3 acceleration = { 0, 0, 0 };
        glm::vec3 force        = { 0, 0, 0 };

        f32 mass = 1.0f;

        union
        {
            f32 radius;
            
            struct { f32 width, height, length; };
        };
    };

    struct PlayerCamera
    {
        i32 player = -1;
        Ref<Camera> camera;
    };

    // TODO: Attenuation
    struct DirectionalLight
    {
        glm::vec3 direction = glm::vec3(-1.0f);
        
        glm::vec3 ambient = glm::vec3(0.2f);
        glm::vec3 diffuse = glm::vec3(0.5f);
        glm::vec3 specular = glm::vec3(1.0f);
    };

    struct PointLight
    {
        glm::vec3 ambient = glm::vec3(0.2f);
        glm::vec3 diffuse = glm::vec3(0.5f);
        glm::vec3 specular = glm::vec3(1.0f);
    };

    struct SpotLight
    {
        glm::vec3 direction;
        f32       angle;

        glm::vec3 ambient = glm::vec3(0.2f);
        glm::vec3 diffuse = glm::vec3(0.5f);
        glm::vec3 specular = glm::vec3(1.0f);
    };

    namespace internal
    {
        struct EditorCamera
        {
            Ref<Camera> camera;
        };

        struct SceneIntrinsic
        {
            
        };

        struct ExcludeFromHierarchy
        {
            
        };
    }
}
