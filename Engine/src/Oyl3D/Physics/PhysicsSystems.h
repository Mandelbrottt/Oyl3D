#pragma once

#include "Scenes/System.h"
#include "Components/Collidable.h"

#include <btBulletDynamicsCommon.h>
#include <btBulletCollisionCommon.h>

namespace oyl
{
    struct ClosestRaycastResult;

    namespace component
    {
        class RigidBody;
        class Transform;
    }
    
    namespace internal
    {
        class PhysicsSystem : public oyl::System
        {
            OYL_CTOR(PhysicsSystem, System)

        public:
            virtual void onEnter() override;
            virtual void onExit() override;

            virtual void onUpdate() override;
            virtual void onGuiRender() override;

            virtual bool onEvent(const Event& event) override;

            static Ref<ClosestRaycastResult> raytestClosest(glm::vec3 position, glm::vec3 direction, f32 distance, u16 mask);

        public:
            struct RigidBodyInfo
            {
                entt::entity entity = entt::null;

                Ref<btRigidBody>      body;
                Ref<btCollisionShape> shape;
                Ref<btMotionState>    motion;

                struct ChildShapeInfo
                {
                    WeakRef<component::Collidable::ShapeInfo> shapeInfo;
                    Ref<btCollisionShape>                     btShape;
                };

                std::vector<ChildShapeInfo> children;
            };

        private:
            void processIncomingRigidBody(entt::entity                 entity,
                                          const component::Transform&  transformComponent,
                                          const component::Collidable& colliderComponent,
                                          const component::RigidBody&  rigidBodyComponent);

        private:
            Timestep m_fixedTimeStep;

            std::unordered_map<entt::entity, Ref<RigidBodyInfo>> m_rigidBodies;

            // TODO: Do i need to keep hold of all of these?
            UniqueRef<btBroadphaseInterface>    m_btBroadphase;
            UniqueRef<btDispatcher>             m_btDispatcher;
            UniqueRef<btCollisionConfiguration> m_btCollisionConfig;
            UniqueRef<btConstraintSolver>       m_btSolver;
            UniqueRef<btDynamicsWorld>          m_btWorld;
        };
    }
}
