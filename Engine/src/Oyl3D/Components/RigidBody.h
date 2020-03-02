#pragma once

#include "Oyl3D/oylpch.h"

namespace oyl
{
    namespace internal
    {
        class PhysicsSystem;
    }
}

namespace oyl::component
{    
    class RigidBody
    {
        friend oyl::internal::PhysicsSystem;
    public:
        enum Property : u32
        {
            IS_KINEMATIC      = 0x01,
            DETECT_COLLISIONS = 0x02,
            FREEZE_ROTATION_X = 0x04,
            FREEZE_ROTATION_Y = 0x08,
            FREEZE_ROTATION_Z = 0x10,
            DO_INTERPOLATION  = 0x20,
            USE_GRAVITY       = 0x40,
        };
        
        // TODO: Add getVelocityX() and so on
        glm::vec3 getVelocity() const;
        glm::vec3 getAcceleration() const;
        glm::vec3 getForce() const;
        glm::vec3 getImpulse() const;

        f32 getMass() const;
        f32 getFriction() const;

        bool getProperty(Property prop) const;
        u32 getPropertyFlags() const;

        u16 getCollisionGroup() const;
        u16 getCollisionMask() const;

        void setVelocity(glm::vec3 velocity);
        void addVelocity(glm::vec3 velocity);

        void setAcceleration(glm::vec3 acceleration);
        void addAcceleration(glm::vec3 acceleration);

        void setForce(glm::vec3 force);
        void addForce(glm::vec3 force);

        void addImpulse(glm::vec3 impulse);

        void setMass(f32 mass);
        void setFriction(f32 friction);

        void setPropertyFlags(u32 flags);
        void setProperties(u32 flags, bool value);

        void setCollisionGroup(u16 group);
        void setCollisionMask(u16 mask);

    private:
        // TODO: Add more stuff
        glm::vec3 m_velocity     = { 0, 0, 0 };
        glm::vec3 m_force        = { 0, 0, 0 };
        glm::vec3 m_impulse      = { 0, 0, 0 };

        f32 m_mass = 1.0f;
        f32 m_friction = 0.5f;

        u32 m_propertyFlags = DETECT_COLLISIONS | 
                              DO_INTERPOLATION | 
                              USE_GRAVITY;

        u16 m_collisionGroup = 1u;
        u16 m_collisionMask  = 1u;

        bool m_isDirty = true;
    };
}

#include "RigidBody.inl"