#pragma once

#include "Components/Transform.h"
#include "Components/Collidable.h"
#include "Components/RigidBody.h"

#include "Graphics/Buffer.h"

#include "System.h"

#include <btBulletDynamicsCommon.h>
#include <btBulletCollisionCommon.h>


namespace oyl
{
    class Shader;
    class Camera;
}

namespace oyl::internal
{
    class RenderSystem : public System
    {
        OYL_CTOR(RenderSystem, System)

        virtual void onEnter() override;
        virtual void onExit() override;

        virtual void onUpdate() override;
        virtual void onGuiRender() override;

        virtual bool onEvent(const Event& event) override;

    private:
        glm::ivec2 m_windowSize;
    };

    class GuiRenderSystem : public System
    {
        OYL_CTOR(GuiRenderSystem, System)

        virtual void onEnter() override;
        virtual void onExit() override;

        virtual void onUpdate() override;
        virtual void onGuiRender() override;

        virtual bool onEvent(const Event& event) override;

    private:
        Ref<Shader> m_shader;

        Ref<VertexArray> m_vao;
    };

    class AnimationSystem : public System
    {
        OYL_CTOR(AnimationSystem, System)

        virtual void onEnter() override;
        virtual void onExit() override;

        virtual void onUpdate() override;
        virtual void onGuiRender() override;

        virtual bool onEvent(const Event& event) override;
    };
    
    class PhysicsSystem : public System
    {
        OYL_CTOR(PhysicsSystem, System)

        virtual void onEnter() override;
        virtual void onExit() override;

        virtual void onUpdate() override;
        virtual void onGuiRender() override;

        virtual bool onEvent(const Event& event) override;

    private:
        void processIncomingRigidBody(entt::entity entity,
                                      const component::Transform&  transformComponent,
                                      const component::Collidable& colliderComponent,
                                      const component::RigidBody&  rigidBodyComponent);
        
    private:
        Timestep m_fixedTimeStep;

        struct RigidBodyInfo
        {
            Ref<btRigidBody> body;

            Ref<btCollisionShape> shape;
            Ref<btMotionState>    motion;

            struct ChildShapeInfo
            {
                WeakRef<component::Collidable::ShapeInfo> shapeInfo;
                Ref<btCollisionShape>                   btShape;
            };

            std::vector<ChildShapeInfo> children;
        };

        std::unordered_map<entt::entity, Ref<RigidBodyInfo>> m_rigidBodies;

        // TODO: Do i need to keep hold of all of these?
        UniqueRef<btBroadphaseInterface>    m_broadphase;
        UniqueRef<btDispatcher>             m_dispatcher;
        UniqueRef<btCollisionConfiguration> m_collisionConfig;
        UniqueRef<btConstraintSolver>       m_solver;
        UniqueRef<btDynamicsWorld>          m_world;
    };

    class TransformUpdateSystem : public System
    {
        OYL_CTOR(TransformUpdateSystem, System)

        virtual void onUpdate() override;
    };

    class EditorCameraSystem : public System
    {
        OYL_CTOR(EditorCameraSystem, System)

        virtual void onEnter() override;
        virtual void onExit() override;

        virtual void onUpdate() override;
        virtual void onGuiRender() override;

        virtual bool onEvent(const Event& event) override;

    private:
        Ref<Camera> m_camera;

        glm::vec3 m_cameraMove        = glm::vec3(0.0f);
        float     m_cameraMoveSpeed   = 15.0f;
        glm::vec3 m_cameraRotate      = glm::vec3(0.0f);
        float     m_cameraRotateSpeed = 25.0f;

        bool m_doMoveCamera = false;
    };

    class EditorRenderSystem : public System
    {
        OYL_CTOR(EditorRenderSystem, System)

        virtual void onEnter() override;
        virtual void onExit() override;

        virtual void onUpdate() override;
        virtual void onGuiRender() override;

        virtual bool onEvent(const Event& event) override;

        void init();
        void shutdown();

    private:
        Ref<FrameBuffer> m_editorViewportBuffer;

        Ref<Camera> m_targetCamera;

        glm::ivec2 m_windowSize;
    };
}
