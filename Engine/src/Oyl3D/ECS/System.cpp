#include "oylpch.h"

#include "App/Window.h"

#include "Components/Animatable.h"
#include "Components/Camera.h"
#include "Components/Lights.h"
#include "Components/Misc.h"
#include "Components/Renderable.h"
#include "Components/Transform.h"

#include "ECS/System.h"
#include "ECS/SystemImpl.h"

#include "Events/Event.h"
#include "Events/EventListener.h"
#include "Events/EventDispatcher.h"

#include "Graphics/Camera.h"
#include "Graphics/Material.h"
#include "Graphics/Shader.h"
#include "Graphics/Texture.h"

#include "Input/Input.h"

#include "Physics/Raycast.h"

#include "Rendering/Renderer.h"

#include <btBulletDynamicsCommon.h>
#include <btBulletCollisionCommon.h>

namespace oyl
{
    // vvv Generic System vvv //

    void System::onEnter() { }

    void System::onExit() { }

    void System::onUpdate() { }

    void System::onGuiRender() { }

    // ^^^ Generic System ^^^ //

    namespace internal
    {
        // vvv Render System vvv //

        void RenderSystem::onEnter()
        {
            listenForEventType(EventType::WindowResized);
        }

        void RenderSystem::onExit() { }

        void RenderSystem::onUpdate()
        {
            using component::Transform;
            using component::Renderable;
            using component::PlayerCamera;
            using component::PointLight;

            const auto& skybox = TextureCubeMap::get(DEFAULT_SKYBOX_ALIAS);
            const auto& shader = Shader::get(SKYBOX_SHADER_ALIAS);
            const auto& mesh = Mesh::get(CUBE_MESH_ALIAS);

            auto isRenderableValid = [](const Renderable& r)
            {
                return r.enabled && r.mesh && r.material && r.material->shader && r.material->albedoMap;
            };

            // We sort our mesh renderers based on material properties
            // This will group all of our meshes based on shader first, then material second
            registry->sort<Renderable>(
                [&isRenderableValid](const Renderable& lhs, const Renderable& rhs)
                {
                    if (!isRenderableValid(lhs))
                        return false;
                    if (!isRenderableValid(rhs))
                        return true;
                    if (lhs.material->shader != rhs.material->shader)
                        return lhs.material->shader < rhs.material->shader;
                    if (lhs.material->albedoMap != rhs.material->albedoMap)
                        return lhs.material->albedoMap < rhs.material->albedoMap;
                    return lhs.material < rhs.material;
                });

            Ref<Material> boundMaterial;

            auto camView = registry->view<Transform, PlayerCamera>();
            
            int x = m_windowSize.x / 2;
            int y = camView.size() > 2 ? m_windowSize.y / 2 : 0;

            int width = m_windowSize.x;
            if (camView.size() > 1) width /= 2;

            int height = m_windowSize.y;
            if (camView.size() > 2) height /= 2;
            
            for (auto camera : camView)
            {
                PlayerCamera& pc = camView.get<PlayerCamera>(camera);

                u32 playerNum = static_cast<u32>(pc.player);
                RenderCommand::setDrawRect(!!(playerNum & 1) * x, !(playerNum & 2) * y, width, height);
                
                pc.aspect((float) width / (float) height);

                glm::mat4 viewProj = pc.projectionMatrix();
                viewProj *= glm::mat4(glm::mat3(pc.viewMatrix()));
                
                shader->bind();
                shader->setUniformMat4("u_viewProjection", viewProj);

                RenderCommand::setDepthDraw(false);
                RenderCommand::setBackfaceCulling(false);
                Renderer::submit(mesh, shader, skybox);
                RenderCommand::setBackfaceCulling(true);
                RenderCommand::setDepthDraw(true);

                bool doCulling = true;

                auto view = registry->view<Renderable, Transform>();
                for (auto entity : view)
                {
                    Renderable& mr = view.get<Renderable>(entity);
                    
                    if (!isRenderableValid(mr))
                        break;

                    if (!(mr.cullingMask & pc.cullingMask))
                        continue;
                    
                    if (mr.material != boundMaterial)
                    {
                        boundMaterial = mr.material;
                        
                        boundMaterial->setUniformMat4("u_view", pc.viewMatrix());
                        boundMaterial->setUniformMat4("u_viewProjection", pc.viewProjectionMatrix());
                        glm::mat4 viewNormal = inverse(transpose(pc.viewMatrix()));
                        boundMaterial->setUniformMat3("u_viewNormal", glm::mat3(viewNormal));

                        auto lightView = registry->view<PointLight>();
                        int count = 0;
                        for (auto light : lightView)
                        {
                            auto lightProps =     lightView.get(light);
                            auto lightTransform = registry->get<Transform>(light);
                            
                            boundMaterial->setUniform3f("u_pointLight[" + std::to_string(count) + "].position",
                                                        pc.viewMatrix() * glm::vec4(lightTransform.getPositionGlobal(), 1.0f));
                            boundMaterial->setUniform3f("u_pointLight[" + std::to_string(count) + "].ambient",
                                                        lightProps.ambient);
                            boundMaterial->setUniform3f("u_pointLight[" + std::to_string(count) + "].diffuse",
                                                        lightProps.diffuse);
                            boundMaterial->setUniform3f("u_pointLight[" + std::to_string(count) + "].specular",
                                                        lightProps.specular);
                            count++;
                        }

                        // TEMPORARY:
                        boundMaterial->bind();
                        boundMaterial->applyUniforms();
                    }

                    auto& transformComponent = view.get<Transform>(entity);
                    glm::mat4 transform = transformComponent.getMatrixGlobal();

                    glm::bvec3 mirror = transformComponent.getMirrorGlobal();
                    if (!(mirror.x ^ mirror.y ^ mirror.z) != doCulling)
                    {
                        doCulling ^= 1;
                        RenderCommand::setBackfaceCulling(doCulling);
                    }

                    if (registry->has<component::Animatable>(entity))
                    {
                        auto& anim = registry->get<component::Animatable>(entity);
                        if (anim.getVertexArray())
                        {
                            anim.m_vao->bind();
                            
                            boundMaterial->shader->bind();
                            boundMaterial->shader->setUniform1f("lerpT_curr", glm::mod(anim.m_currentElapsed, 1.0f));
                            boundMaterial->shader->setUniform1f("lerpT_trans", glm::mod(anim.m_transitionElapsed, 1.0f));

                            Renderer::submit(boundMaterial, anim.m_vao, mr.mesh->getNumVertices(), transform);
                        }
                    }
                    else
                    {
                        Renderer::submit(mr.mesh, boundMaterial, transform);
                    }
                }
            }
        }

        void RenderSystem::onGuiRender() { }

        bool RenderSystem::onEvent(const Event& event)
        {
            switch (event.type)
            {
                case EventType::WindowResized:
                    auto e = event_cast<WindowResizedEvent>(event);
                    m_windowSize = { e.width, e.height };
                    break;
            }
            return false;
        }

        // ^^^ Render System ^^^ //

        // vvv Gui Render System vvv //

        void GuiRenderSystem::onEnter()
        {
            listenForEventType(EventType::WindowResized);
            m_shader = Shader::get("texturedQuad");

            float vertices[] = {
                -0.5f, -0.5f, 0.0f, 0.0f, 1.0f,
                 0.5f, -0.5f, 0.0f, 1.0f, 1.0f,
                 0.5f,  0.5f, 0.0f, 1.0f, 0.0f,
                -0.5f,  0.5f, 0.0f, 0.0f, 0.0f
            };

            u32 indices[] = {
                0, 2, 1,
                0, 3, 2
            };

            m_vao = VertexArray::create();
            Ref<VertexBuffer> vbo = VertexBuffer::create(vertices, sizeof(vertices));
            vbo->setLayout({
                { DataType::Float3, "in_position" },
                { DataType::Float2, "in_texCoord" }
            });
            Ref<IndexBuffer> ebo = IndexBuffer::create(indices, 6);
            m_vao->addVertexBuffer(vbo);
            m_vao->addIndexBuffer(ebo);
        }

        void GuiRenderSystem::onExit() {}

        void GuiRenderSystem::onUpdate()
        {
            using component::GuiRenderable;
            using component::Transform;
            using component::PlayerCamera;

            registry->sort<GuiRenderable>(
                [this](const entt::entity lhs, const entt::entity rhs)
                {
                    auto& lguir = registry->get<GuiRenderable>(lhs);
                    auto& rguir = registry->get<GuiRenderable>(rhs);

                    if (rguir.texture == nullptr)
                        return false;
                    if (lguir.texture == nullptr)
                        return true;

                    auto& lt = registry->get<Transform>(lhs);
                    auto& rt = registry->get<Transform>(rhs);

                    if (float lz = lt.getPositionZGlobal(), rz = rt.getPositionZGlobal();
                        lz != rz)
                        return lz > rz;

                    return lguir.texture < rguir.texture;
                });

            Ref<Texture2D> boundTexture;

            m_shader->bind();
            m_shader->setUniform1i("u_texture", 0);

            RenderCommand::setDepthDraw(false);

            auto camView = registry->view<PlayerCamera>();
            
            for (auto camera : camView)
            {                
                auto& pc = camView.get(camera);

                auto view = registry->view<Transform, GuiRenderable>();
                for (auto entity : view)
                {
                    auto& transform = view.get<Transform>(entity);
                    auto& gui = view.get<GuiRenderable>(entity);

                    if (!gui.enabled || !gui.texture)
                        break;

                    if (!(gui.cullingMask & pc.cullingMask))
                        continue;

                    if ((!boundTexture || boundTexture != gui.texture) && gui.texture->isLoaded())
                    {
                        boundTexture = gui.texture;
                        boundTexture->bind(0);
                    }

                    glm::vec3 texSize = glm::vec3(0.0f);
                    texSize.x = (float) gui.texture->getWidth() / (float) gui.texture->getHeight();
                    texSize.y = 1.0f;

                    glm::vec3 pos = transform.getPosition();
                    pos.y = -pos.y;
                    pos.z = 0.99f;
                    
                    glm::mat4 model = glm::mat4(1.0f);
                    model = glm::translate(model, pos);
                    model = glm::rotate(model, glm::radians(transform.getRotationEuler().z), glm::vec3(0.0f, 0.0f, -1.0f));
                    model = glm::scale(model, transform.getScale());
                    model = glm::scale(model, texSize);

                    Renderer::submit(m_shader, m_vao, model);
                }
            }

            RenderCommand::setDepthDraw(true);
        }

        void GuiRenderSystem::onGuiRender() {}

        bool GuiRenderSystem::onEvent(const Event& event)
        {
            switch (event.type)
            {
                case EventType::WindowResized:
                    auto e = event_cast<WindowResizedEvent>(event);
                    f32 aspectRatio = (float) e.width / (float) e.height;
                    f32 size = 10.0f;
                    glm::mat4 projection = glm::ortho(-size * aspectRatio / 2.0f, 
                                                      size * aspectRatio / 2.0f, 
                                                      size / 2.0f, 
                                                      -size / 2.0f);

                    m_shader->bind();
                    m_shader->setUniformMat4("u_projection", projection);

                    break;
            }
            return false;
        }

        // ^^^ Gui Render System ^^^ //
        
        // vvv Animation System vvv //
        
        void AnimationSystem::onEnter() {}

        void AnimationSystem::onExit() {}

        void AnimationSystem::onUpdate()
        {
            auto view = registry->view<component::Animatable>();
            for (auto entity : view)
            {
                auto& anim = view.get(entity);

                if (!anim.m_currentAnimation)
                    anim.m_currentAnimation = anim.m_animations.begin()->second;

                if (!anim.m_vao)
                {
                    anim.m_vao = VertexArray::create();

                    anim.m_vao->addVertexBuffer(anim.m_currentAnimation->poses[0].mesh->m_vbo);
                    anim.m_vao->addVertexBuffer(anim.m_currentAnimation->poses[1].mesh->m_vbo);
                }

                if (anim.m_nextAnimation)
                {
                    anim.m_transitionElapsed += Time::deltaTime() / anim.m_transitionDuration;
                    if (anim.m_transitionElapsed >= 1.0f)
                    {
                        anim.m_currentAnimation = anim.m_nextAnimation;
                        anim.m_nextAnimation.reset();
                        anim.m_transitionElapsed  = 0.0f;
                        anim.m_transitionDuration = 0.0f;
                        anim.m_currentElapsed     = -0.0001f;
                    }
                }

                anim.m_currentElapsed =
                    glm::mod(anim.m_currentElapsed, (f32) anim.m_currentAnimation->poses.size());

                uint lastVal = glm::floor(anim.m_currentElapsed);
                anim.m_currentElapsed += Time::deltaTime() / anim.m_currentAnimation->poses[lastVal].duration;

                anim.m_currentElapsed =
                    glm::mod(anim.m_currentElapsed, (f32) anim.m_currentAnimation->poses.size());

                uint currVal = glm::floor(anim.m_currentElapsed);

                if (lastVal != currVal)
                {
                    ++lastVal %= anim.m_currentAnimation->poses.size();
                    ++currVal %= anim.m_currentAnimation->poses.size();
                    
                    auto lastMeshVbo = anim.m_currentAnimation->poses[lastVal].mesh->m_vbo;
                    auto currMeshVbo = anim.m_currentAnimation->poses[currVal].mesh->m_vbo;
                    
                    anim.m_vao->unload();
                    anim.m_vao->load();

                    anim.m_vao->addVertexBuffer(lastMeshVbo);
                    anim.m_vao->addVertexBuffer(currMeshVbo);
                    if (anim.m_nextAnimation)
                    {
                        auto transMeshVbo = anim.m_nextAnimation->poses[0].mesh->m_vbo;
                        anim.m_vao->addVertexBuffer(transMeshVbo);
                    }
                }
            }
        }

        void AnimationSystem::onGuiRender() {}

        bool AnimationSystem::onEvent(const Event& event) { return false; }

        // ^^^ Animation System ^^^ //
        
        // vvv Physics System vvv //

        static PhysicsSystem* g_currentPhysicsSystem = nullptr;

        static Ref<EventDispatcher> g_dispatcher;
        static Ref<entt::registry>  g_currentRegistry;

        static void* g_obj1 = 0;
        static void* g_obj2 = 0;

        static void contactStartedCallback(btPersistentManifold* const& manifold)
        {
            auto body1 = manifold->getBody0();
            auto body2 = manifold->getBody1();

            if (body1 == g_obj1 && body2 == g_obj2)
                return;
            
            g_obj1 = (void*) body1;
            g_obj2 = (void*) body2;
            
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

        static void contactEndedCallback(btPersistentManifold* const& manifold)
        {
            auto body1 = manifold->getBody0();
            auto body2 = manifold->getBody1();

            if (body1 == g_obj1 && body2 == g_obj2)
                return;

            g_obj1 = (void*) body1;
            g_obj2 = (void*) body2;
            
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

            if (body1 == g_obj1 && body2 == g_obj2)
                return false;

            g_obj1 = (void*) body1;
            g_obj2 = (void*) body2;

            auto entity1 = (entt::entity) reinterpret_cast<ENTT_ID_TYPE>(body1->getUserPointer());
            auto entity2 = (entt::entity) reinterpret_cast<ENTT_ID_TYPE>(body2->getUserPointer());

            if (!g_currentRegistry->valid(entity1) || !g_currentRegistry->valid(entity2))
                return false;

            PhysicsCollisionStayEvent event;
            event.entity1 = entity1;
            event.entity2 = entity2;

            if (body1->getCollisionFlags() & btCollisionObject::CF_NO_CONTACT_RESPONSE ||
                body2->getCollisionFlags() & btCollisionObject::CF_NO_CONTACT_RESPONSE)
            {
                event.type = EventType::PhysicsTriggerStay;
            }

            g_dispatcher->postEvent(event);
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
            m_btDispatcher = UniqueRef<btCollisionDispatcher>::create(m_btCollisionConfig.get());
            m_btBroadphase = UniqueRef<btDbvtBroadphase>::create();
            m_btSolver = UniqueRef<btSequentialImpulseConstraintSolver>::create();
            m_btWorld = UniqueRef<btDiscreteDynamicsWorld>::create(m_btDispatcher.get(), 
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
                    
                    btTransform t = cachedBody.body->getWorldTransform();

                    t.setOrigin(btVector3(transform.getPositionXGlobal(),
                                          transform.getPositionYGlobal(),
                                          transform.getPositionZGlobal()));
                    
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
                if (rigidBody.m_isDirty)
                {
                    //m_world->removeRigidBody(cachedBody.body.get());
                    
                    // Velocity
                    cachedBody.body->setLinearVelocity(btVector3(rigidBody.m_velocity.x,
                                                                 rigidBody.m_velocity.y,
                                                                 rigidBody.m_velocity.z));

                    // Forces
                    cachedBody.body->clearForces();

                    // Friction
                    cachedBody.body->setFriction(rigidBody.m_friction);
                    cachedBody.body->setSpinningFriction(rigidBody.m_friction);
                    cachedBody.body->setAnisotropicFriction(btVector3(rigidBody.m_friction, 
                                                                      rigidBody.m_friction, 
                                                                      rigidBody.m_friction), 
                                                            btCollisionObject::CF_ANISOTROPIC_FRICTION);
                    cachedBody.body->setRollingFriction(rigidBody.m_friction);

                    // Flags
                    int flags = cachedBody.body->getCollisionFlags();

                    if (rigidBody.getProperty(RigidBody::IS_KINEMATIC))
                    {
                        flags |= btRigidBody::CF_KINEMATIC_OBJECT;
                        cachedBody.body->setActivationState(DISABLE_DEACTIVATION);
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

                    //// Rotation Locking
                    //btVector3 inertiaTensor = {};

                    //inertiaTensor.setX(rigidBody.getProperty(RigidBody::FREEZE_ROTATION_X) ? 0.0f : 1.0f);
                    //inertiaTensor.setY(rigidBody.getProperty(RigidBody::FREEZE_ROTATION_Y) ? 0.0f : 1.0f);
                    //inertiaTensor.setZ(rigidBody.getProperty(RigidBody::FREEZE_ROTATION_Z) ? 0.0f : 1.0f);

                    //cachedBody.body->setInvInertiaDiagLocal(inertiaTensor);

                    //cachedBody.body->updateInertiaTensor();

                    //m_world->addRigidBody(cachedBody.body.get());

                    // Gravity
                    if (rigidBody.getProperty(RigidBody::USE_GRAVITY))
                    {
                        cachedBody.body->setGravity(m_btWorld->getGravity());
                    }
                    else
                    {
                        cachedBody.body->setGravity({ 0.0f, 0.0f, 0.0f });
                    }

                    rigidBody.m_isDirty = false;
                }

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

            g_obj1 = g_obj2 = 0;

            for (auto entity : view)
            {
                auto& transform = view.get<Transform>(entity);
                auto& rigidBody = view.get<RigidBody>(entity);

                RigidBodyInfo& cachedBody = *m_rigidBodies[entity];
                
                btVector3 _pos = {};
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
                    auto _t = p->getMatrixGlobal();
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
        void PhysicsSystem::processIncomingRigidBody(entt::entity entity, 
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
                    shape.mesh.m_isDirty     = false;
                }
            }
            
            if (m_rigidBodies.find(entity) == m_rigidBodies.end())
            {
                // Add RigidBody to World

                Ref<btCollisionShape> shape  = nullptr;
                Ref<btMotionState>    motion = nullptr;
                Ref<btRigidBody>      body   = nullptr;

                m_rigidBodies[entity] = Ref<RigidBodyInfo>::create();

                if (colliderComponent.size() == 1)
                {
                    btTransform t;

                    const auto& shapeThing = colliderComponent.getShape(0);
                    switch (shapeThing.m_type)
                    {
                        case ColliderType::Box:
                        {
                            t.setIdentity();

                            t.setOrigin({
                                shapeThing.box.getCenter().x,
                                shapeThing.box.getCenter().y,
                                shapeThing.box.getCenter().z
                            });

                            btVector3 halfExtents = {
                                shapeThing.box.getSize().x / 2.0f,
                                shapeThing.box.getSize().y / 2.0f,
                                shapeThing.box.getSize().z / 2.0f
                            };

                            shape = Ref<btBoxShape>::create(halfExtents);

                            RigidBodyInfo::ChildShapeInfo info;

                            info.btShape = shape;

                            info.shapeInfo = shapeThing.m_selfRef;

                            m_rigidBodies[entity]->children.push_back(info);

                            break;
                        }
                        case ColliderType::Sphere:
                        {
                            t.setIdentity();

                            t.setOrigin({
                                shapeThing.sphere.getCenter().x,
                                shapeThing.sphere.getCenter().y,
                                shapeThing.sphere.getCenter().z
                            });

                            shape = Ref<btSphereShape>::create(shapeThing.sphere.getRadius());

                            RigidBodyInfo::ChildShapeInfo info;

                            info.btShape = shape;

                            info.shapeInfo = shapeThing.m_selfRef;

                            m_rigidBodies[entity]->children.emplace_back(info);

                            break;
                        }
                    }
                    
                    // TEMPORARY: Make relative to collider
                    t.setIdentity();
                    
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
                }
                else
                {
                    auto workingShape = Ref<btCompoundShape>::create();
                    auto childIter = colliderComponent.begin();
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
                                
                                auto childShape = Ref<btBoxShape>::create(halfExtents);
                                workingShape->addChildShape(t, childShape.get());

                                RigidBodyInfo::ChildShapeInfo info;

                                info.btShape = childShape;

                                info.shapeInfo = childIter->m_selfRef;

                                m_rigidBodies[entity]->children.emplace_back(info);

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

                                auto childShape = Ref<btSphereShape>::create(childIter->sphere.getRadius());
                                workingShape->addChildShape(t, childShape.get());

                                RigidBodyInfo::ChildShapeInfo info;

                                info.btShape = childShape;

                                info.shapeInfo = childIter->m_selfRef;

                                m_rigidBodies[entity]->children.emplace_back(info);

                                break;
                            }
                        }
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

                    body->setRestitution(1.0f);
                }

                body->setCollisionFlags(body->getCollisionFlags() |
                                        btCollisionObject::CF_CUSTOM_MATERIAL_CALLBACK);
                
                m_btWorld->addRigidBody(body.get());

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

        Ref<ClosestRaycastResult> PhysicsSystem::raytestClosest(glm::vec3 position, glm::vec3 direction, f32 distance)
        {
            glm::vec3 endPos = position + direction * distance;
            btVector3 from = { position.x, position.y, position.z };
            btVector3 to   = { endPos.x, endPos.y, endPos.z };

            btCollisionWorld::ClosestRayResultCallback rayCallback(from, to);
            g_currentPhysicsSystem->m_btWorld->rayTest(from, to, rayCallback);

            RaycastResult::HitObject obj = {};
            
            if (rayCallback.hasHit())
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
                Ref<ClosestRaycastResult>::create(position, endPos, rayCallback.hasHit(), std::move(obj));

            return result;
        }

        // ^^^ Physics System ^^^ //

        // vvv Transform Update System vvv //

        //void TransformUpdateSystem::onUpdate()
        //{
        //    using component::Transform;
        //    using component::Parent;

        //    auto view = registry->view<Transform>();
        //    for (auto entity : view)
        //    {
        //        auto& ct = view.get(entity);
        //        if (registry->has<Parent>(entity))
        //        {
        //            auto parent = registry->get<Parent>(entity).parent;
        //            if (parent != entt::null)
        //            {   
        //                auto& pt = registry->get<Transform>(parent);

        //                if (!pt.m_localRef) 
        //                    pt.m_localRef = Ref<Transform>(&pt, [](Transform*) {});

        //                ct.m_parentRef = pt.m_localRef;
        //            }
        //            else ct.m_parentRef = {};
        //        }
        //        else ct.m_parentRef = {};
        //    }
        //};

        // ^^^ Transform Update System ^^^ //
        
        // vvv Editor Camera System vvv //

        void EditorCameraSystem::onEnter()
        {
            listenForEventType(EventType::KeyPressed);
            listenForEventType(EventType::KeyReleased);
            listenForEventType(EventType::MouseMoved);
            listenForEventType(EventType::MousePressed);
            listenForEventType(EventType::MouseReleased);
            listenForEventType(EventType::EditorViewportResized);

            m_camera = Ref<Camera>::create();
            m_camera->setProjection(glm::perspective(glm::radians(60.0f), 16.0f / 9.0f, 0.1f, 1000.0f));
            m_camera->setPosition(glm::vec3(10.0f, 5.0f, 10.0f));
            m_camera->lookAt(glm::vec3(0.0f, 0.0f, 0.0f));

            EditorCameraChangedEvent ccEvent;
            ccEvent.camera = &m_camera;

            postEvent(ccEvent);
        }

        void EditorCameraSystem::onExit() { }

        void EditorCameraSystem::onUpdate()
        {
            if (!m_doMoveCamera)
            {
                m_cameraMove = glm::vec3(0.0f);
                m_cameraRotate = glm::vec3(0.0f);
                return;
            }

            m_camera->move(m_cameraMove * Time::unscaledDeltaTime());

            static glm::vec3 realRotation = glm::vec3(20.0f, -45.0f, 0.0f);

            realRotation += m_cameraRotate * m_cameraRotateSpeed * Time::unscaledDeltaTime();
            if (realRotation.x > 89.0f) realRotation.x = 89.0f;
            if (realRotation.x < -89.0f) realRotation.x = -89.0f;

            m_camera->setRotation(realRotation);

            m_cameraRotate = glm::vec3(0.0f);
        }

        void EditorCameraSystem::onGuiRender()
        {
            ImGui::Begin("Camera##CameraSettings");

            ImGui::SliderFloat("Move Speed", &m_cameraMoveSpeed, 5.0f, 30.f);
            ImGui::SliderFloat("Turn Speed", &m_cameraRotateSpeed, 0.1f, 50.0f);

            ImGui::End();
        }

        bool EditorCameraSystem::onEvent(const Event& event)
        {
            switch (event.type)
            {
                case EventType::KeyPressed:
                {
                    if (!m_doMoveCamera) break;

                    auto e = event_cast<KeyPressedEvent>(event);
                    if (!e.repeatCount)
                    {
                        if (e.keycode == Key::W)
                            m_cameraMove.z -= m_cameraMoveSpeed;
                        if (e.keycode == Key::S)
                            m_cameraMove.z += m_cameraMoveSpeed;
                        if (e.keycode == Key::D)
                            m_cameraMove.x += m_cameraMoveSpeed;
                        if (e.keycode == Key::A)
                            m_cameraMove.x -= m_cameraMoveSpeed;
                        if (e.keycode == Key::Space)
                            m_cameraMove.y += m_cameraMoveSpeed;
                        if (e.keycode == Key::LeftControl)
                            m_cameraMove.y -= m_cameraMoveSpeed;
                    }
                    break;
                }
                case EventType::KeyReleased:
                {
                    if (!m_doMoveCamera) break;
                    
                    auto e = event_cast<KeyReleasedEvent>(event);
                    if (e.keycode == Key::W)
                        m_cameraMove.z += m_cameraMoveSpeed;
                    if (e.keycode == Key::S)
                        m_cameraMove.z -= m_cameraMoveSpeed;
                    if (e.keycode == Key::D)
                        m_cameraMove.x -= m_cameraMoveSpeed;
                    if (e.keycode == Key::A)
                        m_cameraMove.x += m_cameraMoveSpeed;
                    if (e.keycode == Key::Space)
                        m_cameraMove.y -= m_cameraMoveSpeed;
                    if (e.keycode == Key::LeftControl)
                        m_cameraMove.y += m_cameraMoveSpeed;
                    break;
                }
                case EventType::MouseMoved:
                {
                    if (!m_doMoveCamera) break;

                    auto e = event_cast<MouseMovedEvent>(event);
                    m_cameraRotate.y = e.dx;
                    m_cameraRotate.x = e.dy;

                    break;
                }
                case EventType::MousePressed:
                {
                    auto e = event_cast<MousePressedEvent>(event);
                    if (e.button == Mouse::Right)
                    {
                        m_doMoveCamera = true;

                        CursorStateRequestEvent cursorRequest;
                        cursorRequest.state = CursorState::Disabled;

                        postEvent(cursorRequest);
                    }
                    break;
                }
                case EventType::MouseReleased:
                {
                    auto e = event_cast<MouseReleasedEvent>(event);
                    if (e.button == Mouse::Right)
                    {
                        m_doMoveCamera = false;

                        CursorStateRequestEvent cursorRequest;
                        cursorRequest.state = CursorState::Normal;

                        postEvent(cursorRequest);
                    }
                    break;
                }
                case EventType::EditorViewportResized:
                {
                    auto e = event_cast<EditorViewportResizedEvent>(event);
                    glm::mat4 proj = glm::perspective(glm::radians(60.0f), e.width / e.height, 0.1f, 1000.0f);
                    m_camera->setProjection(proj);
                }
            }
            return false;
        }

        // ^^^ Editor Camera System vvv //

        // vvv Editor Render System vvv //

        void EditorRenderSystem::onEnter()
        {
            listenForEventType(EventType::WindowResized);
            listenForEventType(EventType::EditorCameraChanged);

            m_editorViewportBuffer = FrameBuffer::create(1);
            m_editorViewportBuffer->initDepthTexture(1, 1);
            m_editorViewportBuffer->initColorTexture(0, 1, 1, Texture::RGBA8, Texture::Nearest, Texture::Clamp);

            EditorViewportHandleChangedEvent handleChanged;
            handleChanged.handle = m_editorViewportBuffer->getColorHandle(0);
            postEvent(handleChanged);
        }

        void EditorRenderSystem::onExit() { }

        void EditorRenderSystem::onUpdate()
        {
            using component::Transform;
            using component::Renderable;
            using component::PlayerCamera;
            using component::PointLight;
            
            m_editorViewportBuffer->clear();
            m_editorViewportBuffer->bind();

            RenderCommand::setDrawRect(0, 0, m_windowSize.x, m_windowSize.y);

            const auto& skybox = TextureCubeMap::get(DEFAULT_SKYBOX_ALIAS);
            const auto& shader = Shader::get(SKYBOX_SHADER_ALIAS);
            const auto& mesh   = Mesh::get(CUBE_MESH_ALIAS);

            glm::mat4 viewProj = m_targetCamera->getProjectionMatrix();
            viewProj *= glm::mat4(glm::mat3(m_targetCamera->getViewMatrix()));
            shader->bind();
            shader->setUniformMat4("u_viewProjection", viewProj);

            RenderCommand::setDepthDraw(false);
            RenderCommand::setBackfaceCulling(false);
            Renderer::submit(mesh, shader, skybox);
            RenderCommand::setBackfaceCulling(true);
            RenderCommand::setDepthDraw(true);

            // We sort our mesh renderers based on material properties
            // This will group all of our meshes based on shader first, then material second
            registry->sort<Renderable>(
                [](const Renderable& lhs, const Renderable& rhs)
                {
                    if (lhs.material == nullptr || lhs.mesh == nullptr)
                        return false;
                    if (rhs.material == nullptr || rhs.mesh == nullptr)
                        return true;
                    if (lhs.material->shader != rhs.material->shader)
                        return lhs.material->shader < rhs.material->shader;
                    return lhs.material < rhs.material;
                });

            Ref<Material> boundMaterial = Material::create();
            Ref<Shader>   tempShader    = Shader::get(LIGHTING_SHADER_ALIAS);

            bool doCulling = true;

            auto view = registry->view<Renderable, Transform>();
            for (auto entity : view)
            {
                Renderable& mr = view.get<Renderable>(entity);

                if (mr.mesh == nullptr || mr.material == nullptr)
                    break;

                if (mr.material != boundMaterial)
                {                    
                    *boundMaterial = *mr.material;
                    boundMaterial->shader = tempShader;

                    boundMaterial->setUniformMat4("u_view", m_targetCamera->getViewMatrix());
                    boundMaterial->setUniformMat4("u_viewProjection", m_targetCamera->getViewProjectionMatrix());
                    glm::mat3 viewNormal = glm::mat3(m_targetCamera->getViewMatrix());
                    viewNormal = inverse(transpose(viewNormal));
                    boundMaterial->setUniformMat3("u_viewNormal", viewNormal);

                    auto lightView = registry->view<PointLight>();
                    int count = 0;
                    for (auto light : lightView)
                    {
                        auto lightProps = lightView.get(light);
                        auto lightTransform = registry->get<Transform>(light);

                        boundMaterial->setUniform3f("u_pointLight[" + std::to_string(count) + "].position",
                                                    m_targetCamera->getViewMatrix() * glm::vec4(lightTransform.getPositionGlobal(), 1.0f));
                        boundMaterial->setUniform3f("u_pointLight[" + std::to_string(count) + "].ambient",
                                                    lightProps.ambient);
                        boundMaterial->setUniform3f("u_pointLight[" + std::to_string(count) + "].diffuse",
                                                    lightProps.diffuse);
                        boundMaterial->setUniform3f("u_pointLight[" + std::to_string(count) + "].specular",
                                                    lightProps.specular);
                        count++;
                    }
                    
                    boundMaterial->bind();
                    boundMaterial->applyUniforms();
                }

                auto& transformComponent = view.get<Transform>(entity);
                glm::mat4 transform = transformComponent.getMatrixGlobal();

                glm::bvec3 mirror = transformComponent.getMirrorGlobal();
                if (!(mirror.x ^ mirror.y ^ mirror.z) != doCulling)
                {
                    doCulling ^= 1;
                    RenderCommand::setBackfaceCulling(doCulling);
                }
                
                Renderer::submit(mr.mesh, boundMaterial, transform);
            }

            m_editorViewportBuffer->unbind();
        }

        void EditorRenderSystem::onGuiRender() { }

        bool EditorRenderSystem::onEvent(const Event& event)
        {
            switch (event.type)
            {
                case EventType::WindowResized:
                {
                    auto e = event_cast<WindowResizedEvent>(event);
                    m_editorViewportBuffer->updateViewport(e.width, e.height);
                    m_windowSize = { e.width, e.height };
                    return true;
                }
                case EventType::EditorCameraChanged:
                {
                    auto e = event_cast<EditorCameraChangedEvent>(event);
                    m_targetCamera = *e.camera;
                }
            }
            return false;
        }

        void EditorRenderSystem::init() { }

        void EditorRenderSystem::shutdown() { }

        // ^^^ Editor Render System ^^^ //
    }
}
