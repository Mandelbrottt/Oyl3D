#pragma once

namespace oyl
{
    class Mesh;
    class Material;
    class Camera;

    namespace ECS
    {
        class PhysicsSystem;
    }
}

namespace oyl::component
{
    struct SceneObject
    {
        std::string name;
    };

    struct Transform
    {
    public:
        glm::vec3 getPosition()  const;
        f32       getPositionX() const;
        f32       getPositionY() const;
        f32       getPositionZ() const;
        
        glm::vec3 getRotationEuler()  const;
        f32       getRotationEulerX() const;
        f32       getRotationEulerY() const;
        f32       getRotationEulerZ() const;
        
        OYL_DEPRECATED("Not implemented, use getRotationEuler() instead.")
        glm::quat getRotationQuat() const;
        
        glm::vec3 getScale()  const;
        f32       getScaleX() const;
        f32       getScaleY() const;
        f32       getScaleZ() const;

        const glm::mat4& getMatrixLocal()  const;
        glm::mat4        getMatrixGlobal() const;

        glm::vec3 getForwardLocal() const;
        glm::vec3 getRightLocal()   const;
        glm::vec3 getUpLocal()      const;

        glm::vec3 getForwardGlobal() const;
        glm::vec3 getRightGlobal()   const;
        glm::vec3 getUpGlobal()      const;

        void setPosition(glm::vec3 position);
        void setPositionX(f32 x);
        void setPositionY(f32 y);
        void setPositionZ(f32 z);

        void setRotationEuler(glm::vec3 rotation);
        void setRotationEulerX(f32 x);
        void setRotationEulerY(f32 y);
        void setRotationEulerZ(f32 z);

        OYL_DEPRECATED("Not implemented, use setRotationEuler instead.")
        void setRotationQuat(glm::quat rotation);
        
        void setScale(glm::vec3 scale);
        void setScaleX(f32 x);
        void setScaleY(f32 y);
        void setScaleZ(f32 z);

        bool isLocalDirty() const;

    private:
        friend ECS::PhysicsSystem;
        
        glm::vec3 m_localPosition      = glm::vec3(0.0f);
        glm::vec3 m_localEulerRotation = glm::vec3(0.0f);
        glm::quat m_localQuatRotaiton  = glm::quat(glm::vec3(0.0f));
        glm::vec3 m_localScale         = glm::vec3(1.0f);
        
        mutable glm::mat4 m_localMatrix = glm::mat4(1.0f);

        mutable glm::mat4 m_globalMatrix = glm::mat4(1.0f);

        mutable bool m_isLocalDirty = true;
        
        bool m_isPositionOverridden = true;
        bool m_isRotationOverridden = true;
        bool m_isScaleOverridden    = true;
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

        struct ExcludeFromHierarchy { };
    }
}

#include "Component.inl"