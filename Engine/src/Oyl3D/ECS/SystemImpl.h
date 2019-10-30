#pragma once

#include "System.h"

#include <btBulletDynamicsCommon.h>
#include <btBulletCollisionCommon.h>

namespace oyl::ECS
{
    class RenderSystem : public System
    {
        OYL_CTOR(RenderSystem, System)

        virtual void onEnter() override;
        virtual void onExit() override;

        virtual void onUpdate(Timestep dt) override;
        virtual void onGuiRender(Timestep dt) override;

        virtual bool onEvent(Ref<Event> event) override;
    };

    class PhysicsSystem : public System
    {
        OYL_CTOR(PhysicsSystem, System)

        virtual void onEnter() override;
        virtual void onExit() override;

        virtual void onUpdate(Timestep dt) override;
        virtual void onGuiRender(Timestep) override;

        virtual bool onEvent(Ref<Event> event) override;

    private:
        Timestep m_fixedTimeStep;

        // TODO: Do i need to keep hold of all of these?
        UniqueRef<btDynamicsWorld>          m_world;
        UniqueRef<btDispatcher>             m_dispatcher;
        UniqueRef<btBroadphaseInterface>    m_broadphase;
        UniqueRef<btCollisionConfiguration> m_collisionConfig;
        UniqueRef<btConstraintSolver>       m_solver;

        std::unordered_map<Entity, Ref<btRigidBody>> m_rigidBodies;
    };

    namespace internal
    {
        class OracleCameraSystem : public System
        {
            OYL_CTOR(OracleCameraSystem, System)

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
    }
}
