#pragma once

#include "Oyl3D/oylpch.h"

namespace oyl::internal
{
    class PhysicsSystem;
    class TransformUpdateSystem;
    class GuiLayer;
}

namespace oyl::component
{

    class Transform
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

        glm::bvec3 getMirror()  const;
        bool       getMirrorX() const;
        bool       getMirrorY() const;
        bool       getMirrorZ() const;

        glm::bvec3 getMirrorGlobal()  const;
        bool       getMirrorXGlobal() const;
        bool       getMirrorYGlobal() const;
        bool       getMirrorZGlobal() const;
    
        const glm::mat4& getMatrix()       const;
        glm::mat4        getMatrixGlobal() const;
    
        glm::vec3 getForward() const;
        glm::vec3 getRight()   const;
        glm::vec3 getUp()      const;
    
        glm::vec3 getForwardGlobal() const;
        glm::vec3 getRightGlobal()   const;
        glm::vec3 getUpGlobal()      const;

        // TODO: add setXGlobal functions
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

        void setMirror(glm::bvec3 mirror);
        void setMirrorX(bool mirror);
        void setMirrorY(bool mirror);
        void setMirrorZ(bool mirror);
        
        bool isLocalDirty() const;
    
    private:
        friend oyl::internal::PhysicsSystem;
        friend oyl::internal::TransformUpdateSystem;
        friend oyl::internal::GuiLayer;
    
        glm::vec3 m_localPosition = glm::vec3(0.0f);
        glm::quat m_localRotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
        glm::vec3 m_localScale    = glm::vec3(1.0f);
        glm::bvec3 m_mirror       = glm::bvec3(false);
    
        glm::vec3 m_deltaPosition = glm::vec3(0.0f);
        glm::vec3 m_deltaRotation = glm::vec3(0.0f);
        glm::vec3 m_deltaScale    = glm::vec3(0.0f);
    
        mutable glm::mat4 m_localMatrix = glm::mat4(1.0f);
    
        Ref<Transform>     m_localRef;
        WeakRef<Transform> m_parentRef = WeakRef<Transform>{};
    
        mutable bool m_isLocalDirty = true;
    
        bool m_isPositionOverridden = true;
        bool m_isRotationOverridden = true;
        bool m_isScaleOverridden    = true;
    };
}

#include "Transform.inl"