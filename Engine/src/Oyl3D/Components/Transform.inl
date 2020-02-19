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
        // Check if the owner has a parent
        if (!m_registry->valid(m_parent))
        {
            return m_localPosition;
        }
        auto& parentTransform = m_registry->get<Transform>(m_parent);
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
        // Check if the owner has a parent
        if (!m_registry->valid(m_parent))
        {
            return getRotationEuler();
        }
        auto& parentTransform = m_registry->get<Transform>(m_parent);
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
        // Check if the owner has a parent
        if (!m_registry->valid(m_parent))
        {
            return getRotation();
        }
        auto& parentTransform = m_registry->get<Transform>(m_parent);
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
        // Check if the owner has a parent
        if (!m_registry->valid(m_parent))
        {
            return getScale();
        }
        auto& parentTransform = m_registry->get<Transform>(m_parent);
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
        // Check if the owner has a parent
        if (!m_registry->valid(m_parent))
        {
            return getMirror();
        }
        auto& parentTransform = m_registry->get<Transform>(m_parent);
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
        // Check if the owner has a parent
        if (!m_registry->valid(m_parent))
        {
            return getMatrix();
        }
        auto& parentTransform = m_registry->get<Transform>(m_parent);
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
        return m_registry->valid(m_parent);
    }
    
    inline Transform* Transform::getParent()
    {
        if (hasParent())
            return &m_registry->get<Transform>(m_parent);
        return nullptr;
    }
    
    inline const Transform* Transform::getParent() const
    {
        if (hasParent())
            return &m_registry->get<Transform>(m_parent);
        return nullptr;
    }

    inline void Transform::setParent(entt::entity entity)
    {
        if (entity == m_parent || entity == m_owner) return;

        if (m_registry->valid(m_parent))
        {
            auto& pc = m_registry->get<Transform>(m_parent).m_childrenEntities;
            pc.erase(remove(pc.begin(), pc.end(), m_owner), pc.end());
        }

        m_parent = entity;
        
        if (m_registry->valid(entity))
        {

            auto& pt = m_registry->get<Transform>(entity);
            auto it = std::find(pt.m_childrenEntities.begin(), pt.m_childrenEntities.end(), m_owner);
            if (it == pt.m_childrenEntities.end())
                pt.m_childrenEntities.push_back(m_owner);
        }
    }
    
    inline entt::entity Transform::entity() const
    {
        return m_owner;
    }
    
    inline entt::entity Transform::getParentEntity() const
    {
        if (m_registry->valid(m_parent))
            return m_parent;
        return entt::null;
    }

    inline const std::vector<Transform*>& Transform::getChildren() const
    {
        m_children.clear();
        for (auto child : getChildrenEntities())
        {
            m_children.push_back(&m_registry->get<Transform>(child));
        }
        return m_children;
    }

    inline const std::vector<entt::entity>& Transform::getChildrenEntities() const
    {
        auto it = m_childrenEntities.begin();
        for (; it != m_childrenEntities.end(); ++it)
        {
            if (!m_registry->valid(*it))
                it = m_childrenEntities.erase(it);
        }
        return m_childrenEntities;
    }

    inline void Transform::clearChildren()
    {
        for (auto child : getChildren())
        {
            if (child->getParentEntity() == m_owner)
                child->setParent(entt::null);
        }
        
        m_childrenEntities.clear();
        m_children.clear();
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
        m_localRotation = m_localRotation * glm::quat(euler);
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

    inline void Transform::scale(glm::vec3 scale)
    {
        m_isLocalDirty |= scale != glm::vec3(1.0f);
        m_localScale *= scale;
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

    inline bool Transform::isDirty() const
    {
        const Transform* p;
        return m_isLocalDirty || ((p = getParent()) != nullptr && p->isDirty());
    }

    inline void Transform::on_construct(entt::entity entity, entt::registry& registry, Transform& transform)
    {
        transform.m_owner = entity;
        transform.m_registry = &registry;
    }
}
