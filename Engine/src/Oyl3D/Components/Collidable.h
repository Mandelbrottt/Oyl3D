#pragma once

#include "Oyl3D/oylpch.h"

namespace oyl
{
    class Mesh;
    
    namespace internal
    {
        class GuiLayer;
        class PhysicsSystem;
    }

    enum class Direction
    {
        X_AXIS,
        Y_AXIS,
        Z_AXIS
    };

    enum class ColliderType
    {
        Box,
        Sphere,
        Capsule,
        Cylinder,
        Mesh
    };
}

namespace oyl::component
{
    namespace internal
    {
        class BaseCollider
        {
        public:
            
            glm::vec3 getCenter() const;

            void setCenter(glm::vec3 center);

            bool isDirty() const;

        protected:
            glm::vec3 m_center = glm::vec3(0.0f);

            bool m_isDirty = true;

            BaseCollider() = default;
            BaseCollider& operator =(const BaseCollider& other);

        private:
            friend oyl::internal::PhysicsSystem;
        };

        class BoxCollider : public BaseCollider
        {
        public:
            // TODO: Add getWidth() and so on
            glm::vec3 getSize() const;

            void setSize(glm::vec3 size);

        private:
            glm::vec3 m_size = glm::vec3(1.0f);
        };

        class SphereCollider : public BaseCollider
        {
        public:
            f32 getRadius() const;

            void setRadius(f32 radius);

        private:
            f32 m_radius = 0.5f;
        };

        class CapsuleCollider : public BaseCollider
        {
        public:
            f32       getRadius()    const;
            f32       getHeight()    const;
            Direction getDirection() const;

            void setRadius(f32 radius);
            void setHeight(f32 height);
            void setDirection(Direction direction);

        private:
            f32       m_radius    = 0.5f;
            f32       m_height    = 1.0f;
            Direction m_direction = Direction::Y_AXIS;
        };

        class CylinderCollider : public BaseCollider
        {
        public:
            f32       getRadius()    const;
            f32       getHeight()    const;
            Direction getDirection() const;

            void setRadius(f32 radius);
            void setHeight(f32 height);
            void setDirection(Direction direction);

        private:
            f32       m_radius = 0.5f;
            f32       m_height = 1.0f;
            Direction m_direction = Direction::Y_AXIS;
        };

        class OYL_DEPRECATED("Not fully implemented.")
            MeshCollider : public BaseCollider
        {
        public:
            MeshCollider() = default;
            ~MeshCollider() = default;
            MeshCollider(const MeshCollider& other) : BaseCollider(other), m_mesh(other.m_mesh) {}

            const Ref<Mesh>& getMesh() const;

            void setMesh(Ref<Mesh> mesh);

        private:
            Ref<Mesh> m_mesh = nullptr;
        };
    }

    class Collidable
    {
    public:
        // TODO: Inherit from bullet3 classes
        struct ShapeInfo
        {
            ShapeInfo();
            explicit ShapeInfo(ColliderType type);
            ShapeInfo(const ShapeInfo& shapeInfo);
            ~ShapeInfo() {}

            ShapeInfo& operator=(const ShapeInfo& shapeInfo);

            ColliderType getType() const;
            void setType(ColliderType type);
            
            bool isTrigger() const;
            void setIsTrigger(bool trigger);

            bool isDirty() const;
            
            union
            {
                internal::BoxCollider      box;
                internal::SphereCollider   sphere;
                internal::CapsuleCollider  capsule;
                internal::CylinderCollider cylinder;
                internal::MeshCollider     mesh;
            };

        private:
            friend oyl::internal::PhysicsSystem;
            friend oyl::internal::GuiLayer;

            void defaultInit();

            Ref<ShapeInfo> m_selfRef;

            ColliderType m_type = ColliderType::Box;

            bool m_isTrigger = false;
            bool m_isDirty   = true;
        };

        ShapeInfo& pushShape(ShapeInfo shape);
        ShapeInfo& pushShape(ColliderType type);
        void eraseShape(u32 index);
        
        ShapeInfo& getShape(u32 index);
        const ShapeInfo& getShape(u32 index) const;

        std::vector<ShapeInfo>& getShapes();
        const std::vector<ShapeInfo>& getShapes() const;

        std::size_t size() const;
        bool empty() const;

        bool isDirty() const;

        std::vector<ShapeInfo>::iterator begin();
        std::vector<ShapeInfo>::iterator end();

        std::vector<ShapeInfo>::const_iterator begin() const;
        std::vector<ShapeInfo>::const_iterator end()   const;

    private:
        friend oyl::internal::PhysicsSystem;
        friend oyl::internal::GuiLayer;
        
        std::vector<ShapeInfo> m_shapes;
    };
}

#include "Collidable.inl"