#pragma once

#include "Transform.h"

#include "Oyl3D/Components/Misc.h"

namespace oyl::component
{
    inline glm::vec3 Transform::getPosition() const
    {
        return m_localPosition;
    }

    inline f32 Transform::getPositionX() const
    {
        return m_localPosition.x;
    }

    inline f32 Transform::getPositionY() const
    {
        return m_localPosition.y;
    }

    inline f32 Transform::getPositionZ() const
    {
        return m_localPosition.z;
    }

    inline glm::vec3 Transform::getPositionGlobal() const
    {
        using component::Parent;

        entt::entity parent;
        
        // Check if the owner has a parent
        if (!m_registry->has<Parent>(m_owner) || 
            (parent = m_registry->get<Parent>(m_owner).parent) == entt::null)
        {
            return m_localPosition;
        }
        auto& parentTransform = m_registry->get<Transform>(parent);
        return parentTransform.getMatrixGlobal() * glm::vec4(m_localPosition, 1.0f);
    }

    inline f32 Transform::getPositionXGlobal() const
    {
        return getPositionGlobal().x;
    }

    inline f32 Transform::getPositionYGlobal() const
    {
        return getPositionGlobal().y;
    }

    inline f32 Transform::getPositionZGlobal() const
    {
        return getPositionGlobal().z;
    }

    inline glm::vec3 Transform::getRotationEuler() const
    {
        return degrees(eulerAngles(m_localRotation));
    }

    inline f32 Transform::getRotationEulerX() const
    {
        return getRotationEuler().x;
    }

    inline f32 Transform::getRotationEulerY() const
    {
        return getRotationEuler().y;
    }
    
    inline f32 Transform::getRotationEulerZ() const
    {
        return getRotationEuler().z;
    }

    inline glm::quat Transform::getRotation() const
    {
        return m_localRotation;
    }

    inline glm::vec3 Transform::getRotationEulerGlobal() const
    {
        using component::Parent;

        entt::entity parent;

        // Check if the owner has a parent
        if (!m_registry->has<Parent>(m_owner) ||
            (parent = m_registry->get<Parent>(m_owner).parent) == entt::null)
        {
            return getRotationEuler();
        }
        auto& parentTransform = m_registry->get<Transform>(parent);
        glm::quat globalQuat = parentTransform.getRotationGlobal() * m_localRotation;
        return degrees(eulerAngles(globalQuat));
    }

    inline f32 Transform::getRotationEulerXGlobal() const
    {
        return getRotationEulerGlobal().x;
    }

    inline f32 Transform::getRotationEulerYGlobal() const
    {
        return getRotationEulerGlobal().y;
    }

    inline f32 Transform::getRotationEulerZGlobal() const
    {
        return getRotationEulerGlobal().z;
    }

    inline glm::quat Transform::getRotationGlobal() const
    {
        using component::Parent;

        entt::entity parent;

        // Check if the owner has a parent
        if (!m_registry->has<Parent>(m_owner) ||
            (parent = m_registry->get<Parent>(m_owner).parent) == entt::null)
        {
            return getRotation();
        }
        auto& parentTransform = m_registry->get<Transform>(parent);
        return parentTransform.getRotationGlobal() * m_localRotation;
    }

    inline glm::vec3 Transform::getScale() const
    {
        return m_localScale;
    }

    inline f32 Transform::getScaleX() const
    {
        return m_localScale.x;
    }

    inline f32 Transform::getScaleY() const
    {
        return m_localScale.y;
    }

    inline f32 Transform::getScaleZ() const
    {
        return m_localScale.z;
    }

    inline glm::vec3 Transform::getScaleGlobal() const
    {
        using component::Parent;

        entt::entity parent;

        // Check if the owner has a parent
        if (!m_registry->has<Parent>(m_owner) ||
            (parent = m_registry->get<Parent>(m_owner).parent) == entt::null)
        {
            return getScale();
        }
        auto& parentTransform = m_registry->get<Transform>(parent);
        return parentTransform.getScaleGlobal() * m_localScale;
    }

    inline f32 Transform::getScaleXGlobal() const
    {
        return getScaleGlobal().x;
    }

    inline f32 Transform::getScaleYGlobal() const
    {
        return getScaleGlobal().y;
    }

    inline f32 Transform::getScaleZGlobal() const
    {
        return getScaleGlobal().z;
    }

    inline glm::bvec3 Transform::getMirror() const
    {
        return m_mirror;
    }
    
    inline bool Transform::getMirrorX() const
    {
        return m_mirror.x;
    }

    inline bool Transform::getMirrorY() const
    {
        return m_mirror.y;
    }

    inline bool Transform::getMirrorZ() const
    {
        return m_mirror.z;
    }

    inline glm::bvec3 Transform::getMirrorGlobal() const
    {
        using component::Parent;

        entt::entity parent;

        // Check if the owner has a parent
        if (!m_registry->has<Parent>(m_owner) ||
            (parent = m_registry->get<Parent>(m_owner).parent) == entt::null)
        {
            return getMirror();
        }
        auto& parentTransform = m_registry->get<Transform>(parent);
        glm::bvec3 ret = parentTransform.getMirrorGlobal();

        for (int i = 0; i < 3; ++i)
            if (m_mirror[i]) ret[i] ^= 1;
        return ret;
    }

    inline bool Transform::getMirrorXGlobal() const
    {
        return getMirrorGlobal().x;
    }

    inline bool Transform::getMirrorYGlobal() const
    {
        return getMirrorGlobal().y;
    }

    inline bool Transform::getMirrorZGlobal() const
    {
        return getMirrorGlobal().z;
    }

    inline const glm::mat4& Transform::getMatrix() const
    {
        if (m_isLocalDirty)
        {
            glm::mat4 ret(1.0f);

            glm::vec3 mirror = glm::vec3(1.0f);
            for (int i = 0; i < 3; i++)
                if (m_mirror[i]) mirror[i] = -1.0f;

            ret = glm::translate(ret, m_localPosition);
            ret *= glm::mat4_cast(m_localRotation);
            ret = glm::scale(ret, m_localScale * mirror);

            m_localMatrix = ret;
            m_isLocalDirty = false;
        }

        return m_localMatrix;
    }

    inline glm::mat4 Transform::getMatrixGlobal() const
    {
        using component::Parent;

        // Check if the owner has a parent
        Parent* p = m_registry->try_get<Parent>(m_owner);
        if (!p || !m_registry->valid(p->parent))
        {
            return getMatrix();
        }
        auto& parentTransform = m_registry->get<Transform>(p->parent);
        return parentTransform.getMatrixGlobal() * getMatrix();
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

    inline bool Transform::hasParent() const
    {
        using component::Parent;

        return m_registry->has<Parent>(m_owner) && m_registry->get<Parent>(m_owner).parent != entt::null;
    }
    
    inline Transform* Transform::getParent()
    {
        using component::Parent;

        if (hasParent())
            return &m_registry->get<Transform>(m_registry->get<Parent>(m_owner).parent);
        return nullptr;
    }
    
    inline const Transform* Transform::getParent() const
    {
        using component::Parent;

        if (hasParent())
            return &m_registry->get<Transform>(m_registry->get<Parent>(m_owner).parent);
        return nullptr;
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

    inline void Transform::setMirror(glm::bvec3 mirror)
    {
        m_isLocalDirty |= m_mirror != mirror;
        m_mirror = mirror;
    }

    inline void Transform::setMirrorX(bool mirror)
    {
        m_isLocalDirty |= m_mirror.x != mirror;
        m_mirror.x = mirror;
    }

    inline void Transform::setMirrorY(bool mirror)
    {
        m_isLocalDirty |= m_mirror.y != mirror;
        m_mirror.y = mirror;
    }

    inline void Transform::setMirrorZ(bool mirror)
    {
        m_isLocalDirty |= m_mirror.z != mirror;
        m_mirror.z = mirror;
    }

    inline bool Transform::isLocalDirty() const
    {
        return m_isLocalDirty;
    }

    inline void Transform::on_construct(entt::entity entity, entt::registry& registry, Transform& transform)
    {
        transform.m_owner = entity;
        transform.m_registry = &registry;
    }
}
