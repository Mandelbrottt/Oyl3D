#pragma once
#include "Component.h"

namespace oyl::component
{
    inline glm::vec3 Transform::getPosition() const { return m_localPosition; }

    inline f32 Transform::getPositionX() const { return m_localPosition.x; }
    inline f32 Transform::getPositionY() const { return m_localPosition.y; }
    inline f32 Transform::getPositionZ() const { return m_localPosition.z; }

    inline glm::vec3 Transform::getPositionGlobal() const
    {
        using wf = WeakRef<Transform>;
        bool isOrphan = true;
        isOrphan = !m_parentRef.owner_before(wf{}) && !wf{}.owner_before(m_parentRef);
        if (isOrphan || m_parentRef.expired())
        {
            return getPosition();
        }
        return m_parentRef.lock()->getMatrixGlobal() * glm::vec4(m_localPosition, 1.0f);
    }

    inline f32 Transform::getPositionXGlobal() const
    {
        using wf = WeakRef<Transform>;
        bool isOrphan = true;
        isOrphan = !m_parentRef.owner_before(wf{}) && !wf{}.owner_before(m_parentRef);
        if (isOrphan || m_parentRef.expired())
        {
            return getPositionX();
        }
        return (m_parentRef.lock()->getMatrixGlobal() * glm::vec4(getPosition(), 1.0f)).x;
    }
    
    inline f32 Transform::getPositionYGlobal() const
    {
        using wf = WeakRef<Transform>;
        bool isOrphan = true;
        isOrphan = !m_parentRef.owner_before(wf{}) && !wf{}.owner_before(m_parentRef);
        if (isOrphan || m_parentRef.expired())
        {
            return getPositionY();
        }
        return (m_parentRef.lock()->getMatrixGlobal() * glm::vec4(getPosition(), 1.0f)).y;
    }
    
    inline f32 Transform::getPositionZGlobal() const
    {
        using wf = WeakRef<Transform>;
        bool isOrphan = true;
        isOrphan = !m_parentRef.owner_before(wf{}) && !wf{}.owner_before(m_parentRef);
        if (isOrphan || m_parentRef.expired())
        {
            return getPositionZ();
        }
        return (m_parentRef.lock()->getMatrixGlobal() * glm::vec4(getPosition(), 1.0f)).x;
    }

    inline glm::vec3 Transform::getRotationEuler() const {  return degrees(glm::eulerAngles(m_localRotation)); }
    inline f32       Transform::getRotationEulerX() const { return degrees(glm::eulerAngles(m_localRotation)).x; }
    inline f32       Transform::getRotationEulerY() const { return degrees(glm::eulerAngles(m_localRotation)).y; }
    inline f32       Transform::getRotationEulerZ() const { return degrees(glm::eulerAngles(m_localRotation)).z; }

    inline glm::quat Transform::getRotation() const { return m_localRotation; }
    
    inline glm::vec3 Transform::getScale() const { return m_localScale; }
    inline f32       Transform::getScaleX() const { return m_localScale.x; }
    inline f32       Transform::getScaleY() const { return m_localScale.y; }
    inline f32       Transform::getScaleZ() const { return m_localScale.z; }

    inline const glm::mat4& Transform::getMatrix() const
    {
        if (m_isLocalDirty)
        {
            glm::mat4 ret(1.0f);

            ret = glm::translate(ret, m_localPosition);
            ret *= glm::mat4_cast(m_localRotation);
            //ret = glm::rotate(ret, glm::radians(m_localEulerRotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
            //ret = glm::rotate(ret, glm::radians(m_localEulerRotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
            //ret = glm::rotate(ret, glm::radians(m_localEulerRotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
            ret = glm::scale(ret, m_localScale);

            m_localMatrix  = ret;
            m_isLocalDirty = false;
        }

        return m_localMatrix;
    }

    inline glm::mat4 Transform::getMatrixGlobal() const
    {
        using wf = WeakRef<Transform>;
        bool isOrphan = true;
        isOrphan = !m_parentRef.owner_before(wf{}) && !wf{}.owner_before(m_parentRef);
        if (isOrphan || m_parentRef.expired())
        {
            return getMatrix();
        } 
        return m_parentRef.lock()->getMatrixGlobal() * getMatrix();
    }

    inline glm::vec3 Transform::getForward() const
    {
        glm::vec3   ret(0.0f);
        const auto& mat = getMatrix();

        ret.x = mat[2].x;
        ret.y = mat[2].y;
        ret.z = mat[2].z;

        return -ret;
    }

    inline glm::vec3 Transform::getRight() const
    {
        glm::vec3   ret(0.0f);
        const auto& mat = getMatrix();

        ret.x = mat[0].x;
        ret.y = mat[0].y;
        ret.z = mat[0].z;

        return ret;
    }

    inline glm::vec3 Transform::getUp() const
    {
        glm::vec3   ret(0.0f);
        const auto& mat = getMatrix();

        ret.x = mat[1].x;
        ret.y = mat[1].y;
        ret.z = mat[1].z;

        return ret;
    }

    inline glm::vec3 Transform::getForwardGlobal() const
    {
        glm::vec3   ret(0.0f);
        const auto& mat = getMatrixGlobal();

        ret.x = mat[2].x;
        ret.y = mat[2].y;
        ret.z = mat[2].z;

        return -ret;
    }

    inline glm::vec3 Transform::getRightGlobal() const
    {
        glm::vec3   ret(0.0f);
        const auto& mat = getMatrixGlobal();

        ret.x = mat[0].x;
        ret.y = mat[0].y;
        ret.z = mat[0].z;

        return ret;
    }

    inline glm::vec3 Transform::getUpGlobal() const
    {
        glm::vec3   ret(0.0f);
        const auto& mat = getMatrixGlobal();

        ret.x = mat[1].x;
        ret.y = mat[1].y;
        ret.z = mat[1].z;

        return ret;
    }

    inline void Transform::setPosition(glm::vec3 position)
    {
        m_isLocalDirty |= m_localPosition != position;
        m_localPosition = position;
        m_isPositionOverridden = true;
    }

    inline void Transform::setPositionX(f32 x)
    {
        m_isLocalDirty |= m_localPosition.x != x;
        m_localPosition.x = x;
        m_isPositionOverridden = true;
    }

    inline void Transform::setPositionY(f32 y)
    {
        m_isLocalDirty |= m_localPosition.y != y;
        m_localPosition.y = y;
        m_isPositionOverridden = true;
    }

    inline void Transform::setPositionZ(f32 z)
    {
        m_isLocalDirty |= m_localPosition.z != z;
        m_localPosition.z = z;
        m_isPositionOverridden = true;
    }

    inline void Transform::translate(glm::vec3 move, bool inLocalSpace)
    {
        m_isLocalDirty |= move != glm::vec3(0.0f);
        if (inLocalSpace)
            m_localPosition += m_localRotation * move;
        else
            m_localPosition += move;
        m_isPositionOverridden = true;
    }

    inline void Transform::setRotationEuler(glm::vec3 rotation)
    {
        rotation = radians(rotation);
        m_isLocalDirty |= glm::eulerAngles(m_localRotation) != rotation;
        m_localRotation = glm::quat(rotation);
        m_isRotationOverridden = true;
    }

    inline void Transform::setRotationEulerX(f32 x)
    {
        glm::vec3 angles = glm::eulerAngles(m_localRotation);
        angles = degrees(angles);
        m_isLocalDirty |= angles.x != x;
        angles.x = x;
        angles = radians(angles);
        m_localRotation = glm::quat(angles);
        m_isRotationOverridden = true;
    }

    inline void Transform::setRotationEulerY(f32 y)
    {
        glm::vec3 angles = glm::eulerAngles(m_localRotation);
        angles = degrees(angles);
        m_isLocalDirty |= angles.y != y;
        angles.y = y;
        angles = radians(angles);
        m_localRotation = glm::quat(angles);
        m_isRotationOverridden = true;
    }

    inline void Transform::setRotationEulerZ(f32 z)
    {
        glm::vec3 angles = glm::eulerAngles(m_localRotation);
        angles = degrees(angles);
        m_isLocalDirty |= angles.z != z;
        angles.z = z;
        angles = radians(angles);
        m_localRotation = glm::quat(angles);
        m_isRotationOverridden = true;
    }

    inline void Transform::setRotation(glm::quat rotation)
    {
        m_isLocalDirty |= m_localRotation != rotation;
        m_localRotation = rotation;
        m_isRotationOverridden = true;
    }

    inline void Transform::rotate(glm::vec3 euler)
    {
        m_isLocalDirty |= euler != glm::vec3(0.0f);
        euler = radians(euler);
        m_localRotation = glm::quat(euler) * m_localRotation;
        m_isRotationOverridden = true;
    }

    inline void Transform::setScale(glm::vec3 scale)
    {
        m_isLocalDirty |= m_localScale != scale;
        m_localScale = scale;
        m_isScaleOverridden = true;
    }

    inline void Transform::setScaleX(f32 x)
    {
        m_isLocalDirty |= m_localScale.x != x;
        m_localScale.x = x;
        m_isScaleOverridden = true;
    }

    inline void Transform::setScaleY(f32 y)
    {
        m_isLocalDirty |= m_localScale.y != y;
        m_localScale.y = y;
        m_isScaleOverridden = true;
    }

    inline void Transform::setScaleZ(f32 z)
    {
        m_isLocalDirty |= m_localScale.z != z;
        m_localScale.z = z;
        m_isScaleOverridden = true;
    }

    inline bool Transform::isLocalDirty() const
    {
        return m_isLocalDirty;
    }

    // ^^^ Transform ^^^ //

    // vvv Collider vvv ///

    inline Collider::ShapeInfo::ShapeInfo(const ShapeInfo& shapeInfo)
    {
        *this = shapeInfo;
    }

    inline Collider::ShapeInfo& Collider::ShapeInfo::operator=(const ShapeInfo& shapeInfo)
    {
        this->type = shapeInfo.type;
        this->isTrigger = shapeInfo.isTrigger;

        switch (shapeInfo.type)
        {
            case Collider_Box:
                this->box = shapeInfo.box;
                break;
            case Collider_Sphere:
                this->sphere = shapeInfo.sphere;
                break;
            case Collider_Capsule:
                this->capsule = shapeInfo.capsule;
                break;
            case Collider_Cylinder:
                this->cylinder = shapeInfo.cylinder;
                break;
            case Collider_Mesh:
                this->mesh = shapeInfo.mesh;
                break;
        }

        return *this;
    }

    inline Collider::ShapeInfo& Collider::pushShape(ShapeInfo shape)
    {
        m_isDirty = true;

        return m_shapes.emplace_back(std::move(shape));
    }

    inline Collider::ShapeInfo& Collider::pushShape(OylEnum type)
    {
        m_isDirty = true;

        return m_shapes.emplace_back(type);
    }

    inline void Collider::eraseShape(u32 index)
    {
        m_isDirty = true;
        
        m_shapes.erase(m_shapes.begin() + index);
    }

    inline Collider::ShapeInfo& Collider::getShape(u32 index)
    {
        return m_shapes.at(index);
    }

    inline const Collider::ShapeInfo& Collider::getShape(u32 index) const
    {
        return m_shapes.at(index);
    }

    inline std::vector<Collider::ShapeInfo>& Collider::getShapes()
    {
        return m_shapes;
    }

    inline const std::vector<Collider::ShapeInfo>& Collider::getShapes() const
    {
        return m_shapes;
    }

    inline std::size_t Collider::size() const
    {
        return m_shapes.size();
    }

    inline bool Collider::empty() const
    {
        return m_shapes.empty();
    }

    inline std::vector<Collider::ShapeInfo>::iterator Collider::begin()
    {
        return m_shapes.begin();
    }

    inline std::vector<Collider::ShapeInfo>::iterator Collider::end()
    {
        return m_shapes.end();
    }

    inline std::vector<Collider::ShapeInfo>::const_iterator Collider::begin() const
    {
        return m_shapes.cbegin();
    }

    inline std::vector<Collider::ShapeInfo>::const_iterator Collider::end() const
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
    
    // ^^^ Collider ^^^ //

    // vvv RigidBody vvv //

    inline glm::vec3 RigidBody::getVelocity() const
    {
        return m_velocity;
    }

    inline glm::vec3 RigidBody::getAcceleration() const
    {
        return m_force / m_mass;
    }

    inline glm::vec3 RigidBody::getForce() const
    {
        return m_force;
    }

    inline glm::vec3 RigidBody::getImpulse() const
    {
        return m_impulse;
    }

    inline f32 RigidBody::getMass() const
    {
        return m_mass;
    }

    inline f32 RigidBody::getFriction() const
    {
        return m_friction;
    }
    
    inline bool RigidBody::getProperty(Property prop) const
    {
        // Check if only one bit is set
        OYL_ASSERT(!(prop & (prop - 1)), "Only one property can be gotten at once!")
        return m_propertyFlags & prop;
    }

    inline bool RigidBody::getPropertyFlags() const
    {
        return m_propertyFlags;
    }

    inline void RigidBody::setVelocity(glm::vec3 velocity)
    {
        m_isDirty |= m_velocity != velocity;

        m_velocity = velocity;
    }

    inline void RigidBody::addVelocity(glm::vec3 velocity)
    {
        m_isDirty |= velocity != glm::vec3(0.0f);

        m_velocity += velocity;
    }

    inline void RigidBody::setAcceleration(glm::vec3 acceleration)
    {
        setForce(acceleration * m_mass);
    }

    inline void RigidBody::addAcceleration(glm::vec3 acceleration)
    {
        m_isDirty |= acceleration != glm::vec3(0.0f);

        m_force += acceleration * m_mass;
    }

    inline void RigidBody::setForce(glm::vec3 force)
    {
        m_isDirty |= m_force != force;

        m_force = force;
    }

    inline void RigidBody::addForce(glm::vec3 force)
    {
        m_force += force;
    }

    inline void RigidBody::addImpulse(glm::vec3 impulse)
    {
        m_impulse += impulse;
    }

    inline void RigidBody::setMass(f32 mass)
    {
        m_isDirty |= m_mass != mass;

        m_mass = mass;
    }

    inline void RigidBody::setFriction(f32 friction)
    {
        m_isDirty |= m_friction != friction;

        m_friction = friction;
    }

    inline void RigidBody::overwritePropertyFlags(u32 flags)
    {
        m_isDirty |= m_propertyFlags != flags;
        
        m_propertyFlags = { flags };
    }
    
    inline void RigidBody::setProperties(u32 flags, bool value)
    {
        m_isDirty = true;
        
        if (value)
            m_propertyFlags |= flags;
        else
            m_propertyFlags &= ~flags;
    }

    // ^^^ RigidBody ^^^ //
}
