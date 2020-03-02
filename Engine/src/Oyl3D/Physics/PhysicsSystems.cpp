#include "oylpch.h"
#include "PhysicsSystems.h"

#include "App/Window.h"

#include "Components/Misc.h"
#include "Components/RigidBody.h"
#include "Components/Transform.h"

#include "Events/Event.h"
#include "Events/EventListener.h"
#include "Events/EventDispatcher.h"

#include "Physics/Raycast.h"

#include <btBulletDynamicsCommon.h>
#include <btBulletCollisionCommon.h>

namespace std
{
    template<>
    struct hash<std::pair<entt::entity, entt::entity>>
    {
        std::size_t operator()(const std::pair<entt::entity, entt::entity>& k) const noexcept
        {
            using std::size_t;
            using std::hash;
            using oyl::u32;

            // Compute individual hash values for first,
            // second and third and combine them using XOR
            // and bit shifting:

            return ((hash<u32>()(static_cast<u32>(k.first))
                     ^ (hash<u32>()(static_cast<u32>(k.second)) << 1)) >> 1);
        }
    };
}

namespace oyl::internal
{
    static PhysicsSystem* g_currentPhysicsSystem = nullptr;

    static Ref<EventDispatcher> g_dispatcher;
    static Ref<entt::registry>  g_currentRegistry;

    static std::unordered_map<std::pair<entt::entity, entt::entity>, std::pair<int, glm::vec3>> g_contactMap;

    static int   g_phase = 0;
    static void* g_obj1  = 0;
    static void* g_obj2  = 0;

    static void contactStartedCallback(btPersistentManifold* const & manifold)
    {
        auto body1 = manifold->getBody0();
        auto body2 = manifold->getBody1();

        if (g_phase == 0 && body1 == g_obj1 && body2 == g_obj2)
            return;

        g_phase = 0;
        g_obj1  = (void*) body1;
        g_obj2  = (void*) body2;

        auto entity1 = (entt::entity) reinterpret_cast<ENTT_ID_TYPE>(body1->getUserPointer());
        auto entity2 = (entt::entity) reinterpret_cast<ENTT_ID_TYPE>(body2->getUserPointer());

        if (!g_currentRegistry->valid(entity1) || !g_currentRegistry->valid(entity2))
            return;

        PhysicsCollisionEnterEvent event;
        event.entity1 = entity1;
        event.entity2 = entity2;

        if (body1->getCollisionFlags() & btCollisionObject::CF_NO_CONTACT_RESPONSE ||
            body2->getCollisionFlags() & btCollisionObject::CF_NO_CONTACT_RESPONSE)
        {
            event.type = EventType::PhysicsTriggerEnter;
        }

        g_dispatcher->postEvent(event);
    }

    static void contactEndedCallback(btPersistentManifold* const & manifold)
    {
        auto body1 = manifold->getBody0();
        auto body2 = manifold->getBody1();

        if (g_phase == 1 && body1 == g_obj1 && body2 == g_obj2)
            return;

        g_phase = 1;
        g_obj1  = (void*) body1;
        g_obj2  = (void*) body2;

        auto entity1 = (entt::entity) reinterpret_cast<ENTT_ID_TYPE>(body1->getUserPointer());
        auto entity2 = (entt::entity) reinterpret_cast<ENTT_ID_TYPE>(body2->getUserPointer());

        if (!g_currentRegistry->valid(entity1) || !g_currentRegistry->valid(entity2))
            return;

        PhysicsCollisionExitEvent event;
        event.entity1 = entity1;
        event.entity2 = entity2;

        if (body1->getCollisionFlags() & btCollisionObject::CF_NO_CONTACT_RESPONSE ||
            body2->getCollisionFlags() & btCollisionObject::CF_NO_CONTACT_RESPONSE)
        {
            event.type = EventType::PhysicsTriggerExit;
        }

        g_dispatcher->postEvent(event);
    }

    static bool contactProcessedCallback(btManifoldPoint& cp, void* obj1, void* obj2)
    {
        auto body1 = reinterpret_cast<btCollisionObject*>(obj1);
        auto body2 = reinterpret_cast<btCollisionObject*>(obj2);

        auto entity1 = (entt::entity) reinterpret_cast<ENTT_ID_TYPE>(body1->getUserPointer());
        auto entity2 = (entt::entity) reinterpret_cast<ENTT_ID_TYPE>(body2->getUserPointer());

        if (!g_currentRegistry->valid(entity1) || !g_currentRegistry->valid(entity2))
            return false;

        //static PhysicsCollisionStayEvent event;

        //if (g_phase == 2 && obj1 == g_obj1 && obj2 == g_obj2)
        //    numPoints++;
        //else
        //{
        //    event.entity1 = entity1;
        //    event.entity2 = entity2;
        //    
        //    event.contactPoint = avgContactPoint / static_cast<float>(numPoints);
        //    
        //    if (body1->getCollisionFlags() & btCollisionObject::CF_NO_CONTACT_RESPONSE ||
        //        body2->getCollisionFlags() & btCollisionObject::CF_NO_CONTACT_RESPONSE)
        //    {
        //        event.type = EventType::PhysicsTriggerStay;
        //    }

        //    g_dispatcher->postEvent(event);
        //    
        //    avgContactPoint = glm::vec3(0.0f);
        //    numPoints = 1;
        //}

        auto avgCp = (cp.getPositionWorldOnA() + cp.getPositionWorldOnB()) * 0.5f;

        auto pair = std::make_pair(entity1, entity2);
        if (auto it = g_contactMap.find(pair); it != g_contactMap.end())
            it->second.first++, it->second.second += glm::vec3(avgCp.x(), avgCp.y(), avgCp.z());
        else
            g_contactMap[pair] = std::make_pair(1, glm::vec3(avgCp.x(), avgCp.y(), avgCp.z()));

        return false;
    }

    void PhysicsSystem::onEnter()
    {
        g_currentPhysicsSystem = this;

        listenForEventType(EventType::PhysicsResetWorld);

        gContactStartedCallback   = contactStartedCallback;
        gContactEndedCallback     = contactEndedCallback;
        gContactProcessedCallback = contactProcessedCallback;

        g_dispatcher = m_dispatcher;

        g_currentRegistry = registry;

        m_fixedTimeStep = 1.0f / 60.0f;

        m_btCollisionConfig = UniqueRef<btDefaultCollisionConfiguration>::create();
        m_btDispatcher      = UniqueRef<btCollisionDispatcher>::create(m_btCollisionConfig.get());
        m_btBroadphase      = UniqueRef<btDbvtBroadphase>::create();
        m_btSolver          = UniqueRef<btSequentialImpulseConstraintSolver>::create();
        m_btWorld           = UniqueRef<btDiscreteDynamicsWorld>::create(m_btDispatcher.get(),
                                                                         m_btBroadphase.get(),
                                                                         m_btSolver.get(),
                                                                         m_btCollisionConfig.get());

        m_rigidBodies.clear();

        //m_world->setGravity(btVector3(0.0f, -9.81f, 0.0f));
        m_btWorld->setGravity(btVector3(0.0f, -9.81f, 0.0f));
    }

    void PhysicsSystem::onExit()
    {
        m_btWorld.reset();
        m_btSolver.reset();
        m_btBroadphase.reset();
        m_btDispatcher.reset();
        m_btCollisionConfig.reset();

        m_rigidBodies.clear();
    }

    void PhysicsSystem::onUpdate()
    {
        using component::Transform;
        using component::RigidBody;
        using component::Collidable;

        // TEMPORARY: Change on proper collider integration
        for (auto it = m_rigidBodies.begin(); it != m_rigidBodies.end(); ++it)
        {
            if (registry->valid(it->first) &&
                !registry->has<RigidBody>(it->first))
            {
                m_btWorld->removeRigidBody(it->second->body.get());
                it = m_rigidBodies.erase(it);
            }
        }

        auto view = registry->view<Transform, RigidBody>();
        for (auto entity : view)
        {
            auto& transform = view.get<Transform>(entity);
            auto& rigidBody = view.get<RigidBody>(entity);
            auto& collider  = registry->get_or_assign<Collidable>(entity);

            processIncomingRigidBody(entity, transform, collider, rigidBody);

            RigidBodyInfo& cachedBody = *m_rigidBodies[entity];

            // TODO: Separate lazy calculations into functions?
            if (transform.m_isPositionOverridden)
            {
                //m_world->removeRigidBody(m_rigidBodies[entity]->body.get());
                //this->addRigidBody(entity, transform, rigidBody);

                btTransform t;
                if (cachedBody.body->getCollisionFlags() & btCollisionObject::CF_KINEMATIC_OBJECT)
                    cachedBody.motion->getWorldTransform(t);
                else
                    t = cachedBody.body->getWorldTransform();

                t.setOrigin(btVector3(transform.getPositionXGlobal(),
                                      transform.getPositionYGlobal(),
                                      transform.getPositionZGlobal()));

                if (cachedBody.body->getCollisionFlags() & btCollisionObject::CF_KINEMATIC_OBJECT)
                    cachedBody.motion->setWorldTransform(t);
                else
                    cachedBody.body->setWorldTransform(t);

                transform.m_isPositionOverridden = false;

                // TODO: Recursively recalculate every child transform
            }

            if (transform.m_isRotationOverridden)
            {
                btTransform t = cachedBody.body->getWorldTransform();

                glm::quat rotation = transform.getRotationGlobal();
                t.setRotation(btQuaternion(rotation.x, rotation.y, rotation.z, rotation.w));

                cachedBody.body->setWorldTransform(t);

                transform.m_isRotationOverridden = false;
            }
            if (transform.m_isScaleOverridden)
            {
                cachedBody.shape->setLocalScaling(btVector3(transform.getScaleXGlobal(),
                                                            transform.getScaleYGlobal(),
                                                            transform.getScaleZGlobal()));

                transform.m_isScaleOverridden = false;
            }
            //if (rigidBody.m_isDirty)
            {
                // Velocity
                cachedBody.body->setLinearVelocity(btVector3(rigidBody.m_velocity.x,
                                                             rigidBody.m_velocity.y,
                                                             rigidBody.m_velocity.z));

                // Forces
                //cachedBody.body->clearForces();

                m_btWorld->removeRigidBody(cachedBody.body.get());

                // Friction
                cachedBody.body->setFriction(rigidBody.m_friction);
                //cachedBody.body->setSpinningFriction(rigidBody.m_friction);
                //cachedBody.body->setAnisotropicFriction(btVector3(rigidBody.m_friction, 
                //                                                  rigidBody.m_friction, 
                //                                                  rigidBody.m_friction), 
                //                                        btCollisionObject::CF_ANISOTROPIC_FRICTION);
                //cachedBody.body->setRollingFriction(rigidBody.m_friction);

                // Flags
                int flags = cachedBody.body->getCollisionFlags();

                if (rigidBody.getProperty(RigidBody::IS_KINEMATIC) || rigidBody.getMass() == 0.0f)
                {
                    flags |= btRigidBody::CF_KINEMATIC_OBJECT;
                    cachedBody.body->setActivationState(DISABLE_DEACTIVATION);
                    cachedBody.body->activate(true);
                    cachedBody.body->setMassProps(0.0f, btVector3(0.0f, 0.0f, 0.0f));
                    cachedBody.body->setLinearVelocity({ 0.0f, 0.0f, 0.0f });
                    cachedBody.body->setAngularVelocity({ 0.0f, 0.0f, 0.0f });
                }
                else
                {
                    flags &= ~btRigidBody::CF_KINEMATIC_OBJECT;
                    cachedBody.body->setActivationState(ACTIVE_TAG);

                    btVector3 inertia = { 0, 0, 0 };
                    if (rigidBody.m_mass != 0.0f)
                        cachedBody.shape->calculateLocalInertia(rigidBody.m_mass, inertia);

                    cachedBody.body->setMassProps(rigidBody.m_mass, inertia);
                }

                cachedBody.body->updateInertiaTensor();

                flags &= ~btRigidBody::CF_NO_CONTACT_RESPONSE;
                flags |= rigidBody.getProperty(RigidBody::DETECT_COLLISIONS)
                             ? 0
                             : btRigidBody::CF_NO_CONTACT_RESPONSE;

                cachedBody.body->setCollisionFlags(flags);

                //m_btWorld->addRigidBody(cachedBody.body.get());

                //// Rotation Locking
                //btVector3 inertiaTensor = {};

                //inertiaTensor.setX(rigidBody.getProperty(RigidBody::FREEZE_ROTATION_X) ? 0.0f : 1.0f);
                //inertiaTensor.setY(rigidBody.getProperty(RigidBody::FREEZE_ROTATION_Y) ? 0.0f : 1.0f);
                //inertiaTensor.setZ(rigidBody.getProperty(RigidBody::FREEZE_ROTATION_Z) ? 0.0f : 1.0f);

                //cachedBody.body->setInvInertiaDiagLocal(inertiaTensor);

                //cachedBody.body->updateInertiaTensor();

                // Gravity
                if (rigidBody.getProperty(RigidBody::USE_GRAVITY))
                {
                    cachedBody.body->setGravity(m_btWorld->getGravity());
                }
                else
                {
                    cachedBody.body->setGravity({ 0.0f, 0.0f, 0.0f });
                }
            }

            cachedBody.body->setFriction(rigidBody.m_friction);

            float x = rigidBody.getProperty(RigidBody::FREEZE_ROTATION_X) ? 0.0f : 1.0f;
            float y = rigidBody.getProperty(RigidBody::FREEZE_ROTATION_Y) ? 0.0f : 1.0f;
            float z = rigidBody.getProperty(RigidBody::FREEZE_ROTATION_Z) ? 0.0f : 1.0f;

            //cachedBody.body->setLinearFactor(btVector3(1, 1, 1));
            cachedBody.body->setAngularFactor(btVector3(x, y, z));

            m_btWorld->addRigidBody(cachedBody.body.get(), rigidBody.getCollisionGroup(), rigidBody.getCollisionMask());

            if (transform.m_isPositionOverridden ||
                transform.m_isRotationOverridden ||
                transform.m_isScaleOverridden ||
                rigidBody.m_force != glm::vec3(0.0f) ||
                rigidBody.m_impulse != glm::vec3(0.0f))
                cachedBody.body->activate();

            cachedBody.body->applyCentralForce(btVector3(rigidBody.m_force.x,
                                                         rigidBody.m_force.y,
                                                         rigidBody.m_force.z));

            cachedBody.body->applyCentralImpulse(btVector3(rigidBody.m_impulse.x,
                                                           rigidBody.m_impulse.y,
                                                           rigidBody.m_impulse.z));
        }

        // TODO: Iterate over ghost objects with tick callback
        m_btWorld->stepSimulation(Time::deltaTime(), 10, m_fixedTimeStep);

        for (auto& [entities, point] : g_contactMap)
        {
            PhysicsCollisionStayEvent event;
            auto                      rb1 = registry->try_get<RigidBody>(entities.first);
            auto                      rb2 = registry->try_get<RigidBody>(entities.second);
            if (rb1 && !rb1->getProperty(RigidBody::DETECT_COLLISIONS) ||
                rb2 && !rb2->getProperty(RigidBody::DETECT_COLLISIONS))
            {
                event.type = EventType::PhysicsTriggerStay;
            }

            event.entity1 = entities.first;
            event.entity2 = entities.second;

            event.contactPoint = point.second / static_cast<float>(point.first);

            postEvent(event);
        }

        g_obj1 = g_obj2 = 0;
        g_contactMap.clear();

        for (auto entity : view)
        {
            auto& transform = view.get<Transform>(entity);
            auto& rigidBody = view.get<RigidBody>(entity);

            if (rigidBody.getProperty(RigidBody::IS_KINEMATIC))
                continue;

            RigidBodyInfo& cachedBody = *m_rigidBodies[entity];

            btVector3    _pos = {};
            btQuaternion _rot = {};

            btTransform t;
            cachedBody.motion->getWorldTransform(t);

            _pos = t.getOrigin();
            _rot = t.getRotation();

            if (auto p = transform.getParent(); p == nullptr)
            {
                transform.m_localPosition = { _pos.x(), _pos.y(), _pos.z() };
                transform.m_localRotation = glm::quat(_rot.w(), _rot.x(), _rot.y(), _rot.z());
            }
            else
            {
                auto _t                   = p->getMatrixGlobal();
                transform.m_localPosition =
                    inverse(_t) * glm::vec4(_pos.x(), _pos.y(), _pos.z(), 1.0f);
                transform.m_localRotation =
                    inverse(quat_cast(_t)) * glm::quat(_rot.w(), _rot.x(), _rot.y(), _rot.z());
            }

            transform.m_isLocalDirty = true;

            rigidBody.m_velocity = glm::vec3(cachedBody.body->getLinearVelocity().x(),
                                             cachedBody.body->getLinearVelocity().y(),
                                             cachedBody.body->getLinearVelocity().z());

            rigidBody.m_force = glm::vec3(cachedBody.body->getTotalForce().x(),
                                          cachedBody.body->getTotalForce().y(),
                                          cachedBody.body->getTotalForce().z());

            rigidBody.m_impulse = glm::vec3(0.0f);
        }
    }

    void PhysicsSystem::onGuiRender()
    {
        using component::Transform;
        using component::RigidBody;
        using component::EntityInfo;
        auto view = registry->view<EntityInfo, RigidBody>();

        ImGui::Begin("Physics");

        for (auto entity : view)
        {
            auto& rb = view.get<RigidBody>(entity);
            ImGui::Text("%s: X(%.3f) Y(%.3f) Z(%.3f)",
                        view.get<EntityInfo>(entity).name.c_str(),
                        rb.getVelocity().x,
                        rb.getVelocity().y,
                        rb.getVelocity().z);
        }

        ImGui::End();
    }

    bool PhysicsSystem::onEvent(const Event& event)
    {
        switch (event.type)
        {
            case EventType::PhysicsResetWorld:
            {
                onExit();
                onEnter();

                return true;
            }
        }
        return false;
    }

// TODO: Currently need rigidbody for collider to register in world, make mutually exclusive
    void PhysicsSystem::processIncomingRigidBody(entt::entity                 entity,
                                                 const component::Transform&  transformComponent,
                                                 const component::Collidable& colliderComponent,
                                                 const component::RigidBody&  rigidBodyComponent)
    {
        // Check if collider was emptied past frame
        if (colliderComponent.empty())
        {
            if (m_rigidBodies.find(entity) != m_rigidBodies.end())
            {
                m_btWorld->removeRigidBody(m_rigidBodies.at(entity)->body.get());
                m_rigidBodies.erase(entity);
            }
            return;
        }

        // TODO: Don't get rid of the body from the world, just update it
        if (colliderComponent.isDirty() &&
            m_rigidBodies.find(entity) != m_rigidBodies.end())
        {
            m_btWorld->removeRigidBody(m_rigidBodies.at(entity)->body.get());
            m_rigidBodies.erase(entity);

            for (auto& shape : const_cast<component::Collidable&>(colliderComponent))
            {
                shape.m_isDirty          = false;
                shape.box.m_isDirty      = false;
                shape.sphere.m_isDirty   = false;
                shape.capsule.m_isDirty  = false;
                shape.cylinder.m_isDirty = false;
                //shape.mesh.m_isDirty     = false;
            }
        }

        if (m_rigidBodies.find(entity) == m_rigidBodies.end())
        {
            // Add RigidBody to World

            Ref<btCollisionShape> shape  = nullptr;
            Ref<btMotionState>    motion = nullptr;
            Ref<btRigidBody>      body   = nullptr;

            m_rigidBodies[entity] = Ref<RigidBodyInfo>::create();

            //if (colliderComponent.size() == 1)
            //{
            //    btTransform t;

            //    const auto& shapeThing = colliderComponent.getShape(0);
            //    switch (shapeThing.m_type)
            //    {
            //        case ColliderType::Box:
            //        {
            //            t.setIdentity();

            //            t.setOrigin({
            //                shapeThing.box.getCenter().x,
            //                shapeThing.box.getCenter().y,
            //                shapeThing.box.getCenter().z
            //            });

            //            btVector3 halfExtents = {
            //                shapeThing.box.getSize().x / 2.0f,
            //                shapeThing.box.getSize().y / 2.0f,
            //                shapeThing.box.getSize().z / 2.0f
            //            };

            //            shape = Ref<btBoxShape>::create(halfExtents);
            //            break;
            //        }
            //        case ColliderType::Sphere:
            //        {
            //            t.setIdentity();

            //            t.setOrigin({
            //                shapeThing.sphere.getCenter().x,
            //                shapeThing.sphere.getCenter().y,
            //                shapeThing.sphere.getCenter().z
            //            });

            //            shape = Ref<btSphereShape>::create(shapeThing.sphere.getRadius());
            //            break;
            //        }
            //        case ColliderType::Capsule:
            //        {
            //            t.setIdentity();

            //            t.setOrigin({
            //                shapeThing.capsule.getCenter().x,
            //                shapeThing.capsule.getCenter().y,
            //                shapeThing.capsule.getCenter().z
            //            });

            //            switch (shapeThing.capsule.getDirection())
            //            {
            //                case Direction::X_AXIS:
            //                    shape = Ref<btCapsuleShapeX>::create(shapeThing.capsule.getRadius(),
            //                                                         shapeThing.capsule.getHeight());
            //                    break;
            //                case Direction::Y_AXIS:
            //                    shape = Ref<btCapsuleShape>::create(shapeThing.capsule.getRadius(),
            //                                                        shapeThing.capsule.getHeight());
            //                    break;
            //                case Direction::Z_AXIS:
            //                    shape = Ref<btCapsuleShapeZ>::create(shapeThing.capsule.getRadius(),
            //                                                         shapeThing.capsule.getHeight());
            //                    break;
            //            }
            //            break;
            //        }
            //    }

            //    RigidBodyInfo::ChildShapeInfo cInfo;

            //    cInfo.btShape = shape;

            //    cInfo.shapeInfo = shapeThing.m_selfRef;

            //    m_rigidBodies[entity]->children.emplace_back(cInfo);

            //    // TEMPORARY: Make relative to collider
            //    t.setIdentity();

            //    //t.setFromOpenGLMatrix(value_ptr(transformComponent.getMatrixGlobal()));

            //    t.setOrigin(btVector3(transformComponent.getPositionXGlobal(),
            //                          transformComponent.getPositionYGlobal(),
            //                          transformComponent.getPositionZGlobal()));

            //    glm::quat q = transformComponent.getRotationGlobal();

            //    btQuaternion btq = btQuaternion(q.x, q.y, q.z, q.w);
            //    t.setRotation(btq);

            //    btVector3 inertia = { 0, 0, 0 };
            //    if (rigidBodyComponent.getMass() != 0.0f)
            //        shape->calculateLocalInertia(rigidBodyComponent.getMass(), inertia);

            //    shape->setLocalScaling({
            //        transformComponent.getScaleXGlobal(),
            //        transformComponent.getScaleYGlobal(),
            //        transformComponent.getScaleZGlobal()
            //    });

            //    motion = Ref<btDefaultMotionState>::create(t);

            //    btRigidBody::btRigidBodyConstructionInfo info(rigidBodyComponent.getMass(),
            //                                                  motion.get(),
            //                                                  shape.get(),
            //                                                  inertia);

            //    body = Ref<btRigidBody>::create(info);
            //}
            //else
            {
                auto                  workingShape = Ref<btCompoundShape>::create();
                Ref<btCollisionShape> childShape;
                auto                  childIter = colliderComponent.begin();
                for (; childIter != colliderComponent.end(); ++childIter)
                {
                    switch (childIter->m_type)
                    {
                        case ColliderType::Box:
                        {
                            btTransform t;

                            t.setIdentity();

                            t.setOrigin({
                                childIter->box.getCenter().x,
                                childIter->box.getCenter().y,
                                childIter->box.getCenter().z
                            });

                            btVector3 halfExtents = {
                                childIter->box.getSize().x / 2.0f,
                                childIter->box.getSize().y / 2.0f,
                                childIter->box.getSize().z / 2.0f
                            };

                            childShape = Ref<btBoxShape>::create(halfExtents);
                            workingShape->addChildShape(t, childShape.get());
                            break;
                        }
                        case ColliderType::Sphere:
                        {
                            btTransform t;

                            t.setIdentity();

                            t.setOrigin({
                                childIter->sphere.getCenter().x,
                                childIter->sphere.getCenter().y,
                                childIter->sphere.getCenter().z
                            });

                            childShape = Ref<btSphereShape>::create(childIter->sphere.getRadius());
                            workingShape->addChildShape(t, childShape.get());
                            break;
                        }
                        case ColliderType::Capsule:
                        {
                            btTransform t;

                            t.setIdentity();

                            t.setOrigin({
                                childIter->capsule.getCenter().x,
                                childIter->capsule.getCenter().y,
                                childIter->capsule.getCenter().z
                            });

                            switch (childIter->capsule.getDirection())
                            {
                                case Direction::X_AXIS:
                                    childShape = Ref<btCapsuleShapeX>::create(childIter->capsule.getRadius(),
                                                                              childIter->capsule.getHeight());
                                    break;
                                case Direction::Y_AXIS:
                                    childShape = Ref<btCapsuleShape>::create(childIter->capsule.getRadius(),
                                                                             childIter->capsule.getHeight());
                                    break;
                                case Direction::Z_AXIS:
                                    childShape = Ref<btCapsuleShapeZ>::create(childIter->capsule.getRadius(),
                                                                              childIter->capsule.getHeight());
                                    break;
                            }
                            workingShape->addChildShape(t, childShape.get());
                            break;
                        }
                    }

                    RigidBodyInfo::ChildShapeInfo info;

                    info.btShape = childShape;

                    info.shapeInfo = childIter->m_selfRef;

                    m_rigidBodies[entity]->children.emplace_back(info);
                }

                shape = workingShape;

                btTransform t;

                //t.setFromOpenGLMatrix(value_ptr(transformComponent.getMatrixGlobal()));

                t.setOrigin(btVector3(transformComponent.getPositionXGlobal(),
                                      transformComponent.getPositionYGlobal(),
                                      transformComponent.getPositionZGlobal()));

                glm::quat q = transformComponent.getRotationGlobal();

                btQuaternion btq = btQuaternion(q.x, q.y, q.z, q.w);
                t.setRotation(btq);

                btVector3 inertia = { 0, 0, 0 };
                if (rigidBodyComponent.getMass() != 0.0f)
                    shape->calculateLocalInertia(rigidBodyComponent.getMass(), inertia);

                shape->setLocalScaling({
                    transformComponent.getScaleXGlobal(),
                    transformComponent.getScaleYGlobal(),
                    transformComponent.getScaleZGlobal()
                });

                motion = Ref<btDefaultMotionState>::create(t);

                btRigidBody::btRigidBodyConstructionInfo info(rigidBodyComponent.getMass(),
                                                              motion.get(),
                                                              shape.get(),
                                                              inertia);

                body = Ref<btRigidBody>::create(info);

                //body->setRestitution(1.0f);
            }

            body->setCollisionFlags(body->getCollisionFlags() |
                                    btCollisionObject::CF_CUSTOM_MATERIAL_CALLBACK);

            m_btWorld->addRigidBody(body.get(), rigidBodyComponent.m_collisionGroup, rigidBodyComponent.m_collisionMask);

            m_rigidBodies[entity]->entity = entity;
            m_rigidBodies[entity]->body   = body;
            m_rigidBodies[entity]->shape  = shape;
            m_rigidBodies[entity]->motion = motion;

            body->setUserPointer((void*) entity);
        }
        else
        {
            // RigidBody is already in world, check if dirty
        }
    }

    Ref<ClosestRaycastResult> PhysicsSystem::raytestClosest(glm::vec3 position, glm::vec3 direction, f32 distance, u16 mask)
    {
        glm::vec3 endPos = position + direction * distance;
        btVector3 from   = { position.x, position.y, position.z };
        btVector3 to     = { endPos.x, endPos.y, endPos.z };

        btCollisionWorld::ClosestRayResultCallback rayCallback(from, to);
        rayCallback.m_collisionFilterGroup = -1u;
        rayCallback.m_collisionFilterMask = mask;
        g_currentPhysicsSystem->m_btWorld->rayTest(from, to, rayCallback);

        RaycastResult::HitObject obj = {};

        bool hasHit = rayCallback.hasHit();
        if (hasHit)
        {
            obj.hitPosition = {
                rayCallback.m_hitPointWorld.x(),
                rayCallback.m_hitPointWorld.y(),
                rayCallback.m_hitPointWorld.z()
            };

            obj.hitNormal = {
                rayCallback.m_hitNormalWorld.x(),
                rayCallback.m_hitNormalWorld.y(),
                rayCallback.m_hitNormalWorld.z()
            };

            obj.hitFraction = rayCallback.m_closestHitFraction;

            obj.distanceTo = glm::length(direction * rayCallback.m_closestHitFraction);

            for (auto& kvp : g_currentPhysicsSystem->m_rigidBodies)
            {
                if (kvp.second->body.get() == rayCallback.m_collisionObject)
                {
                    obj.entity = kvp.first;
                    break;
                }
            }

            OYL_ASSERT(obj.entity != entt::null);
        }

        Ref<ClosestRaycastResult> result =
            Ref<ClosestRaycastResult>::create(position, endPos, hasHit, std::move(obj));

        return result;
    }
}
