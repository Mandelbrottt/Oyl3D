#pragma once
#include "Component.h"

namespace oyl::component
{
    inline glm::vec3 Transform::getPosition() const { return m_localPosition; }

    inline f32 Transform::getPositionX() const { return m_localPosition.x; }
    inline f32 Transform::getPositionY() const { return m_localPosition.y; }
    inline f32 Transform::getPositionZ() const { return m_localPosition.z; }

    inline glm::vec3 Transform::getRotationEuler() const { return m_localEulerRotation; }
    inline f32       Transform::getRotationEulerX() const { return m_localEulerRotation.x; }
    inline f32       Transform::getRotationEulerY() const { return m_localEulerRotation.y; }
    inline f32       Transform::getRotationEulerZ() const { return m_localEulerRotation.z; }

    OYL_DEPRECATED("Not implemented, use getRotationEuler() instead.")
    inline glm::quat Transform::getRotationQuat() const
    {
        OYL_ASSERT(false, "getRotationQuat() is not implemented! Use getRotationEuler() instead!");
        return {};
    }

    inline glm::vec3 Transform::getScale() const { return m_localScale; }
    inline f32       Transform::getScaleX() const { return m_localScale.x; }
    inline f32       Transform::getScaleY() const { return m_localScale.y; }
    inline f32       Transform::getScaleZ() const { return m_localScale.z; }

    inline const glm::mat4& Transform::getMatrixLocal() const
    {
        if (m_isLocalDirty)
        {
            glm::mat4 ret(1.0f);

            ret = glm::translate(ret, m_localPosition);
            ret = glm::scale(ret, m_localScale);
            ret = glm::rotate(ret, glm::radians(m_localEulerRotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
            ret = glm::rotate(ret, glm::radians(m_localEulerRotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
            ret = glm::rotate(ret, glm::radians(m_localEulerRotation.x), glm::vec3(1.0f, 0.0f, 0.0f));

            m_localMatrix  = ret;
            m_isLocalDirty = false;
        }

        return m_localMatrix;
    }

    inline glm::mat4 Transform::getMatrixGlobal() const
    {
        return m_globalMatrix * getMatrixLocal();
    }

    inline glm::vec3 Transform::getForwardLocal() const
    {
        glm::vec3   ret(0.0f);
        const auto& mat = getMatrixLocal();

        ret.x = mat[2].x;
        ret.y = mat[2].y;
        ret.z = mat[2].z;

        return -ret;
    }

    inline glm::vec3 Transform::getRightLocal() const
    {
        glm::vec3   ret(0.0f);
        const auto& mat = getMatrixLocal();

        ret.x = mat[0].x;
        ret.y = mat[0].y;
        ret.z = mat[0].z;

        return ret;
    }

    inline glm::vec3 Transform::getUpLocal() const
    {
        glm::vec3   ret(0.0f);
        const auto& mat = getMatrixLocal();

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

    inline void Transform::move(glm::vec3 move)
    {
        m_isLocalDirty |= move != glm::vec3(0.0f);
        m_localPosition += move;
        m_isPositionOverridden = true;
    }

    inline void Transform::setRotationEuler(glm::vec3 rotation)
    {
        m_isLocalDirty |= m_localEulerRotation != rotation;
        m_localEulerRotation = rotation;
        m_isRotationOverridden = true;
    }

    inline void Transform::setRotationEulerX(f32 x)
    {
        m_isLocalDirty |= m_localEulerRotation.x != x;
        m_localEulerRotation.x = x;
        m_isRotationOverridden = true;
    }

    inline void Transform::setRotationEulerY(f32 y)
    {
        m_isLocalDirty |= m_localEulerRotation.y != y;
        m_localEulerRotation.y = y;
        m_isRotationOverridden = true;
    }

    inline void Transform::setRotationEulerZ(f32 z)
    {
        m_isLocalDirty |= m_localEulerRotation.z != z;
        m_localEulerRotation.z = z;
        m_isRotationOverridden = true;
    }

    OYL_DEPRECATED("Not implemented, use setRotationEuler instead.")
    inline void Transform::setRotationQuat(glm::quat rotation)
    {
        OYL_ASSERT(false, "setRotationQuat() is not implemented! Use setRotationEuler() instead!");
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
}
