#pragma once

#include "RigidBody.h"

namespace oyl::component
{
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

    inline u32 RigidBody::getPropertyFlags() const
    {
        return m_propertyFlags;
    }

    inline u16 RigidBody::getCollisionGroup() const
    {
        return m_collisionGroup;
    }
    
    inline u16 RigidBody::getCollisionMask() const
    {
        return m_collisionMask;
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

    inline void RigidBody::setPropertyFlags(u32 flags)
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

    inline void RigidBody::setCollisionGroup(u16 group)
    {
        m_isDirty = true;
        m_collisionGroup = group;
    }

    inline void RigidBody::setCollisionMask(u16 mask)
    {
        m_isDirty = true;
        m_collisionMask = mask;
    }
}
