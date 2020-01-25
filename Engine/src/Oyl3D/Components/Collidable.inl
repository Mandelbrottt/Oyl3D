#pragma once

#include "Collidable.h"

namespace oyl::component
{
    inline Collidable::ShapeInfo::ShapeInfo()
    {
        m_selfRef = Ref<ShapeInfo>(this, [](ShapeInfo*) {});

        defaultInit();
    }

    inline Collidable::ShapeInfo::ShapeInfo(ColliderType type)
        : m_type(type)
    {
        m_selfRef = Ref<ShapeInfo>(this, [](ShapeInfo*) {});
        
        defaultInit();
    }
    
    inline Collidable::ShapeInfo::ShapeInfo(const ShapeInfo& shapeInfo)
    {
        *this = shapeInfo;

        m_isDirty = true;
    }

    inline Collidable::ShapeInfo& Collidable::ShapeInfo::operator=(const ShapeInfo& shapeInfo)
    {
        m_type = shapeInfo.m_type;
        m_isTrigger = shapeInfo.m_isTrigger;

        switch (shapeInfo.m_type)
        {
            case ColliderType::Box:
                this->box = shapeInfo.box;
                break;
            case ColliderType::Sphere:
                this->sphere = shapeInfo.sphere;
                break;
            case ColliderType::Capsule:
                this->capsule = shapeInfo.capsule;
                break;
            case ColliderType::Cylinder:
                this->cylinder = shapeInfo.cylinder;
                break;
            //case ColliderType::Mesh:
            //    this->mesh = shapeInfo.mesh;
            //    break;
        }

        m_isDirty = true;

        return *this;
    }

    inline void Collidable::ShapeInfo::defaultInit()
    {
        switch (m_type)
        {
            case ColliderType::Box:
                box = {};
                box.setSize(glm::vec3(1.0f));
                break;
            case ColliderType::Sphere:
                sphere = {};
                sphere.setRadius(0.5f);
                break;
            case ColliderType::Capsule:
                capsule = {};
                capsule.setRadius(0.5f);
                capsule.setHeight(1.0f);
                capsule.setDirection(Direction::Y_AXIS);
                break;
            case ColliderType::Cylinder:
                cylinder = {};
                cylinder.setRadius(0.5f);
                cylinder.setHeight(1.0f);
                cylinder.setDirection(Direction::Y_AXIS);
                break;
            //case ColliderType::Mesh:
            //    mesh = {};
            //    break;
        }

        m_isDirty = true;
    }

    inline ColliderType Collidable::ShapeInfo::getType() const
    {
        return m_type;
    }

    inline void Collidable::ShapeInfo::setType(ColliderType type)
    {
        if (m_type != type)
        {
            m_isDirty = true;
            m_type = type;

            defaultInit();
        }
        
    }

    inline bool Collidable::ShapeInfo::isTrigger() const
    {
        return m_isTrigger;
    }

    inline void Collidable::ShapeInfo::setIsTrigger(bool trigger)
    {
        m_isDirty |= m_isTrigger != trigger;
        m_isTrigger = trigger;
    }

    inline bool Collidable::ShapeInfo::isDirty() const
    {
        return m_isDirty || box.isDirty() || sphere.isDirty() || 
               capsule.isDirty() || cylinder.isDirty() /*|| mesh.isDirty()*/;
    }

    inline Collidable::ShapeInfo& Collidable::pushShape(ShapeInfo shape)
    {
        return m_shapes.emplace_back(shape);
    }

    inline Collidable::ShapeInfo& Collidable::pushShape(ColliderType type)
    {
        return m_shapes.emplace_back(type);
    }

    inline void Collidable::eraseShape(u32 index)
    {   
        m_shapes.erase(m_shapes.begin() + index);
    }

    inline Collidable::ShapeInfo& Collidable::getShape(u32 index)
    {
        return m_shapes.at(index);
    }

    inline const Collidable::ShapeInfo& Collidable::getShape(u32 index) const
    {
        return m_shapes.at(index);
    }

    inline std::vector<Collidable::ShapeInfo>& Collidable::getShapes()
    {
        return m_shapes;
    }

    inline const std::vector<Collidable::ShapeInfo>& Collidable::getShapes() const
    {
        return m_shapes;
    }

    inline std::size_t Collidable::size() const
    {
        return m_shapes.size();
    }

    inline bool Collidable::empty() const
    {
        return m_shapes.empty();
    }

    inline bool Collidable::isDirty() const
    {
        bool dirty = false;
        for (const auto& shape : m_shapes)
            dirty |= shape.isDirty();
        return dirty;
    }

    inline std::vector<Collidable::ShapeInfo>::iterator Collidable::begin()
    {
        return m_shapes.begin();
    }

    inline std::vector<Collidable::ShapeInfo>::iterator Collidable::end()
    {
        return m_shapes.end();
    }

    inline std::vector<Collidable::ShapeInfo>::const_iterator Collidable::begin() const
    {
        return m_shapes.cbegin();
    }

    inline std::vector<Collidable::ShapeInfo>::const_iterator Collidable::end() const
    {
        return m_shapes.cend();
    }

    namespace internal
    {
        inline glm::vec3 BaseCollider::getCenter() const
        {
            return m_center;
        }
        
        inline bool BaseCollider::isDirty() const
        {
            return m_isDirty;    
        }

        inline void BaseCollider::setCenter(glm::vec3 center)
        {
            m_isDirty = m_center != center;

            m_center = center;
        }

        inline BaseCollider& BaseCollider::operator =(const BaseCollider& other)
        {
            m_center  = other.m_center;
            m_isDirty = true;

            return *this;
        }

        inline glm::vec3 BoxCollider::getSize() const
        {
            return m_size;
        }

        inline void BoxCollider::setSize(glm::vec3 size)
        {
            m_isDirty = m_size != size;

            m_size = size;
        }

        inline f32 SphereCollider::getRadius() const
        {
            return m_radius;
        }
        
        inline void SphereCollider::setRadius(f32 radius)
        {
            m_isDirty = m_radius != radius;

            m_radius = radius;
        }

        inline f32 CapsuleCollider::getRadius() const
        {
            return m_radius;
        }

        inline f32 CapsuleCollider::getHeight() const
        {
            return m_height;
        }

        inline Direction CapsuleCollider::getDirection() const
        {
            return m_direction;
        }

        inline void CapsuleCollider::setRadius(f32 radius)
        {
            m_isDirty = m_radius != radius;

            m_radius = radius;
        }

        inline void CapsuleCollider::setHeight(f32 height)
        {
            m_isDirty = m_height = height;

            m_height = height;
        }

        inline void CapsuleCollider::setDirection(Direction direction)
        {
            m_isDirty = m_direction != direction;

            m_direction = direction;
        }

        inline f32 CylinderCollider::getRadius() const
        {
            return m_radius;
        }

        inline f32 CylinderCollider::getHeight() const
        {
            return m_height;
        }

        inline Direction CylinderCollider::getDirection() const
        {
            return m_direction;
        }

        inline void CylinderCollider::setRadius(f32 radius)
        {
            m_isDirty = m_radius != radius;

            m_radius = radius;
        }

        inline void CylinderCollider::setHeight(f32 height)
        {
            m_isDirty = m_height = height;

            m_height = height;
        }

        inline void CylinderCollider::setDirection(Direction direction)
        {
            m_isDirty = m_direction != direction;

            m_direction = direction;
        }
    }
}