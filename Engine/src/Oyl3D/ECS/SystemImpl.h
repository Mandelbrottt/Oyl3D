#pragma once

#include "Component.h"
#include "System.h"

#include <btBulletDynamicsCommon.h>
#include <btBulletCollisionCommon.h>

#include "Graphics/Buffer.h"

namespace oyl::internal
{
    class RenderSystem : public System
    {
        OYL_CTOR(RenderSystem, System)

        virtual void onEnter() override;
        virtual void onExit() override;

        virtual void onUpdate(Timestep dt) override;
        virtual void onGuiRender(Timestep dt) override;

        virtual bool onEvent(Ref<Event> event) override;

        void init();
        void shutdown();
    };

    class PhysicsSystem : public System
    {
        OYL_CTOR(PhysicsSystem, System)

        virtual void onEnter() override;
        virtual void onExit() override;

        virtual void onUpdate(Timestep dt) override;
        virtual void onGuiRender(Timestep) override;

        virtual bool onEvent(Ref<Event> event) override;

        void addRigidBody(Entity entity,
                          const component::Transform& transformComponent,
                          const component::RigidBody& bodyComponent);

    private:
        Timestep m_fixedTimeStep;

        struct RigidBodyInfo
        {
            Ref<btRigidBody> body;

            Ref<btCollisionShape> shape;
            Ref<btMotionState>    motion;
        };

        std::unordered_map<Entity, Ref<RigidBodyInfo>> m_rigidBodies;

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

        virtual void onUpdate(Timestep dt) override;
    };

    class EditorCameraSystem : public System
    {
        OYL_CTOR(EditorCameraSystem, System)

        virtual void onEnter() override;
        virtual void onExit() override;

        virtual void onUpdate(Timestep dt) override;
        virtual void onGuiRender(Timestep dt) override;

        virtual bool onEvent(Ref<Event> event) override;

    private:
        void processCameraUpdate(Timestep dt, const Ref<Camera>& camera);

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

        virtual void onUpdate(Timestep dt) override;
        virtual void onGuiRender(Timestep dt) override;

        virtual bool onEvent(Ref<Event> event) override;

        void init();
        void shutdown();

    private:
        Ref<FrameBuffer> m_editorViewportBuffer;
    };
}
