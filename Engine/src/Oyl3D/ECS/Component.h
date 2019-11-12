#pragma once

namespace oyl
{
    class Mesh;
    class Material;
    class Camera;

    namespace internal
    {
        class PhysicsSystem;
        class TransformUpdateSystem;
        class GuiLayer;
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

        glm::vec3 getPositionGlobal()  const;
        f32       getPositionXGlobal() const;
        f32       getPositionYGlobal() const;
        f32       getPositionZGlobal() const;
        
        glm::vec3 getRotationEuler()  const;
        f32       getRotationEulerX() const;
        f32       getRotationEulerY() const;
        f32       getRotationEulerZ() const;

        glm::quat getRotation() const;

        glm::vec3 getRotationEulerGlobal()  const;
        f32       getRotationEulerXGlobal() const;
        f32       getRotationEulerYGlobal() const;
        f32       getRotationEulerZGlobal() const;

        glm::quat getRotationGlobal() const;
        
        glm::vec3 getScale()  const;
        f32       getScaleX() const;
        f32       getScaleY() const;
        f32       getScaleZ() const;

        glm::vec3 getScaleGlobal()  const;
        f32       getScaleXGlobal() const;
        f32       getScaleYGlobal() const;
        f32       getScaleZGlobal() const;

        const glm::mat4& getMatrix()       const;
        glm::mat4        getMatrixGlobal() const;

        glm::vec3 getForward() const;
        glm::vec3 getRight()   const;
        glm::vec3 getUp()      const;

        glm::vec3 getForwardGlobal() const;
        glm::vec3 getRightGlobal()   const;
        glm::vec3 getUpGlobal()      const;

        void setPosition(glm::vec3 position);
        void setPositionX(f32 x);
        void setPositionY(f32 y);
        void setPositionZ(f32 z);

        void translate(glm::vec3 move, bool inLocalSpace = true);

        void setRotationEuler(glm::vec3 rotation);
        void setRotationEulerX(f32 x);
        void setRotationEulerY(f32 y);
        void setRotationEulerZ(f32 z);

        void rotate(glm::vec3 euler);

        void setRotation(glm::quat rotation);
        
        void setScale(glm::vec3 scale);
        void setScaleX(f32 x);
        void setScaleY(f32 y);
        void setScaleZ(f32 z);

        void scale(glm::vec3 scale);

        bool isLocalDirty() const;

    private:
        friend internal::PhysicsSystem;
        friend internal::TransformUpdateSystem;
        friend internal::GuiLayer;
        
        glm::vec3 m_localPosition      = glm::vec3(0.0f);
        glm::quat m_localRotation      = {};
        glm::vec3 m_localScale         = glm::vec3(1.0f);

        glm::vec3 m_deltaPosition = glm::vec3(0.0f);
        glm::vec3 m_deltaRotation = glm::vec3(0.0f);
        glm::vec3 m_deltaScale    = glm::vec3(0.0f);
        
        mutable glm::mat4 m_localMatrix = glm::mat4(1.0f);

        Ref<Transform>     m_localRef  = nullptr;
        WeakRef<Transform> m_parentRef = WeakRef<Transform>{};

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

    struct Collider
    {
        struct ShapeInfo
        {
            OylEnum type;

            bool isTrigger;
            
            union
            {
                struct BoxCollider
                {
                    glm::vec3 center;
                    glm::vec3 size;
                } box;

                struct SphereCollider
                {
                    glm::vec3 center;
                    f32       radius;
                } sphere;

                enum class Direction
                {
                    Y_AXIS, Z_AXIS, X_AXIS
                };

                struct CapsuleCollider
                {
                    glm::vec3 center;
                    f32       radius;
                    f32       height;
                    Direction direction;
                } capsule;

                struct CylinderCollider
                {
                    glm::vec3 center;
                    f32       radius;
                    f32       height;
                    Direction direction;
                } cylinder;

                OYL_DEPRECATED("Not fully implemented.")
                struct MeshCollider
                {
                    glm::vec3 center;
                    Ref<Mesh> mesh;
                } mesh;
            };
        };

        void pushShape(ShapeInfo shape);
        void eraseShape(u32 index);
        ShapeInfo& getShape(u32 index);

        const std::vector<ShapeInfo>& getShapes();

        std::vector<ShapeInfo>::iterator begin();
        std::vector<ShapeInfo>::iterator end();

        std::vector<ShapeInfo>::const_iterator begin() const;
        std::vector<ShapeInfo>::const_iterator end()   const;

    private:
        std::vector<ShapeInfo> m_shapes;
        
        bool m_isDirty = false;
    };

    struct RigidBody
    {
        entt::entity id = entt::null;
        
        OylEnum type = OylEnum::None;

        glm::vec3 velocity     = { 0, 0, 0 };
        glm::vec3 acceleration = { 0, 0, 0 };
        glm::vec3 force        = { 0, 0, 0 };
        glm::vec3 impulse      = { 0, 0, 0 };

        bool isKinematic = false;
        bool isStatic    = false;

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

        // TEMPORARY:
        glm::mat4 projection;
        
        //Ref<Camera> camera;
    };

    struct Parent
    {
        entt::entity parent;
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