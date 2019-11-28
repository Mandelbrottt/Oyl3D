#include "oylpch.h"

#include "ECS/System.h"
#include "ECS/SystemImpl.h"

#include "ECS/Component.h"

#include "Events/Event.h"
#include "Events/EventListener.h"

#include "Graphics/Camera.h"
#include "Graphics/Material.h"
#include "Graphics/Shader.h"
#include "Graphics/Texture.h"

#include "Rendering/Renderer.h"

#include "Scenes/Scene.h"

#include <btBulletDynamicsCommon.h>
#include <btBulletCollisionCommon.h>

namespace oyl
{
    // vvv Generic System vvv //

    void System::onEnter() { }

    void System::onExit() { }

    void System::onUpdate(Timestep dt) { }

    void System::onGuiRender(Timestep dt) { }

    // ^^^ Generic System ^^^ //

    namespace internal
    {
        // vvv Render System vvv //

        void RenderSystem::onEnter() { }

        void RenderSystem::onExit() { }

        void RenderSystem::onUpdate(Timestep dt)
        {
            using component::Transform;
            using component::Renderable;
            using component::PlayerCamera;
            using component::PointLight;
            using component::internal::EditorCamera;

            const auto& skybox = TextureCubeMap::get(DEFAULT_SKYBOX_ALIAS);
            const auto& shader = Shader::get(SKYBOX_SHADER_ALIAS);
            const auto& mesh = Mesh::get(CUBE_MESH_ALIAS);

            // We sort our mesh renderers based on material properties
            // This will group all of our meshes based on shader first, then material second
            registry->sort<Renderable>(
                [](const Renderable& lhs, const Renderable& rhs)
                {
                    if (rhs.material == nullptr || rhs.mesh == nullptr)
                        return false;
                    else if (lhs.material == nullptr || lhs.mesh == nullptr)
                        return true;
                    else if (lhs.material->shader != rhs.material->shader)
                        return lhs.material->shader < rhs.material->shader;
                    else
                        return lhs.material < rhs.material;
                });

            Ref<Material> boundMaterial;

            auto camView = registry->view<Transform, PlayerCamera>();
            OYL_ASSERT(camView.size() == 1, "Only one camera supported!");

            for (auto camera : camView)
            {
                glm::mat4 viewProj = camView.get<PlayerCamera>(camera).projection;
                glm::mat4 tempView = camView.get<Transform>(camera).getMatrixGlobal();
                viewProj *= glm::mat4(glm::inverse(glm::mat3(tempView)));
                
                shader->bind();
                shader->setUniformMat4("u_viewProjection", viewProj);

                RenderCommand::setDepthDraw(false);
                RenderCommand::setBackfaceCulling(false);
                Renderer::submit(mesh, shader, skybox);
                RenderCommand::setBackfaceCulling(true);
                RenderCommand::setDepthDraw(true);

                auto view = registry->view<Transform, Renderable>();
                for (const auto& entity : view)
                {
                    Renderable& mr = view.get<Renderable>(entity);

                    if (mr.mesh == nullptr || 
                        mr.material == nullptr || 
                        mr.material->shader == nullptr)
                        continue;
                    
                    if (mr.material != boundMaterial)
                    {
                        boundMaterial = mr.material;

                        glm::mat4 viewMatrix = camView.get<Transform>(camera).getMatrixGlobal();
                        viewMatrix = inverse(viewMatrix);

                        const glm::mat4& projectionMatrix = camView.get<PlayerCamera>(camera).projection;
                        
                        boundMaterial->setUniformMat4("u_view", viewMatrix);
                        boundMaterial->setUniformMat4("u_viewProjection", projectionMatrix * viewMatrix);
                        glm::mat4 viewNormal = inverse(transpose(viewMatrix));
                        boundMaterial->setUniformMat3("u_viewNormal", glm::mat3(viewNormal));

                        auto lightView =      registry->view<PointLight>();
                        auto lightProps =     lightView.get(lightView[0]);
                        auto lightTransform = registry->get<Transform>(lightView[0]);
                        
                        boundMaterial->setUniform3f("u_pointLight.position",  
                                                    viewMatrix * glm::vec4(lightTransform.getPositionGlobal(), 1.0f));
                        boundMaterial->setUniform3f("u_pointLight.ambient",   lightProps.ambient);
                        boundMaterial->setUniform3f("u_pointLight.diffuse",   lightProps.diffuse);
                        boundMaterial->setUniform3f("u_pointLight.specular",  lightProps.specular);

                        // TEMPORARY:
                        boundMaterial->bind();
                        boundMaterial->applyUniforms();
                    }
                    
                    glm::mat4 transform = view.get<Transform>(entity).getMatrixGlobal();

                    if (registry->has<component::Animator>(entity))
                    {
                        auto& anim = registry->get<component::Animator>(entity);
                        if (anim.getVertexArray())
                        {
                            anim.m_vao->bind();
                            
                            mr.material->shader->bind();
                            mr.material->shader->setUniform1f("lerpT_curr", glm::mod(anim.m_currentElapsed, 1.0f));
                            mr.material->shader->setUniform1f("lerpT_trans", glm::mod(anim.m_transitionElapsed, 1.0f));

                            Renderer::submit(mr.material, anim.m_vao, mr.mesh->getNumVertices(), transform);
                        }
                    }
                    else
                    {
                        Renderer::submit(mr.mesh, mr.material, transform);
                    }
                }
            }
        }

        void RenderSystem::onGuiRender(Timestep dt) { }

        bool RenderSystem::onEvent(Ref<Event> event)
        {
            return false;
        }

        // ^^^ Render System ^^^ //

        // vvv Gui Render System vvv //

        void GuiRenderSystem::onEnter()
        {
            listenForEventType(TypeWindowResized);
            m_shader = Shader::get("texturedQuad");

            float vertices[] = {
                -0.5f, -0.5f, 0.0f, 0.0f, 1.0f,
                 0.5f, -0.5f, 0.0f, 1.0f, 1.0f,
                 0.5f,  0.5f, 0.0f, 1.0f, 0.0f,
                -0.5f,  0.5f, 0.0f, 0.0f, 0.0f
            };

            u32 indices[] = {
                0, 2, 1,
                0, 3, 2,
            };

            // TEMPORARY: move to onEnter()
            m_vao = VertexArray::create();
            Ref<VertexBuffer> vbo = VertexBuffer::create(vertices, sizeof(vertices));
            vbo->setLayout({
                { Float3, "in_position" },
                { Float2, "in_texCoord" }
            });
            Ref<IndexBuffer> ebo = IndexBuffer::create(indices, 6);
            m_vao->addVertexBuffer(vbo);
            m_vao->addIndexBuffer(ebo);
        }

        void GuiRenderSystem::onExit() {}

        void GuiRenderSystem::onUpdate(Timestep dt)
        {
            using component::GuiRenderable;
            using component::Transform;

            registry->sort<GuiRenderable>(
                [](const GuiRenderable& lhs, const GuiRenderable& rhs)
                {
                    if (rhs.texture == nullptr)
                        return false;
                    else if (lhs.texture == nullptr)
                        return true;
                    else
                        return lhs.texture < rhs.texture;
                });

            Ref<Texture2D> boundTexture;

            m_shader->bind();
            m_shader->setUniform1i("u_texture", 0);

            auto view = registry->view<Transform, GuiRenderable>();
            for (auto entity : view)
            {
                auto& transform = view.get<Transform>(entity);
                auto& gui = view.get<GuiRenderable>(entity);

                if ((!boundTexture || boundTexture != gui.texture) && gui.texture->isLoaded())
                {
                    boundTexture = gui.texture;
                    boundTexture->bind(0);
                }

                glm::vec3 texSize = glm::vec3(0.0f);
                texSize.x = (float) gui.texture->getWidth() / (float) gui.texture->getHeight();
                texSize.y = 1.0f;

                glm::vec3 pos = transform.getPosition();
                pos.z = 0.99f;
                
                glm::mat4 model = glm::mat4(1.0f);
                model = glm::translate(model, pos);
                model = glm::rotate(model, glm::radians(transform.getRotationEuler().z), glm::vec3(0.0f, 0.0f, -1.0f));
                model = glm::scale(model, transform.getScale());
                model = glm::scale(model, texSize);

                Renderer::submit(m_shader, m_vao, model);
            }
        }

        void GuiRenderSystem::onGuiRender(Timestep dt) {}

        bool GuiRenderSystem::onEvent(Ref<Event> event)
        {
            switch (event->type)
            {
                case TypeWindowResized:
                    auto e = (WindowResizedEvent) *event;
                    //OYL_LOG("{0}", offsetof(WindowResizedEvent, width));
                    //OYL_LOG("{0}", offsetof(WindowResizedEvent, height));
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

        void AnimationSystem::onUpdate(Timestep dt)
        {
            auto view = registry->view<component::Animator>();
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
                    anim.m_transitionElapsed += dt.getSeconds() / anim.m_transitionDuration;
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
                anim.m_currentElapsed += dt.getSeconds() / anim.m_currentAnimation->poses[lastVal].duration;

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

        void AnimationSystem::onGuiRender(Timestep dt) {}

        bool AnimationSystem::onEvent(Ref<Event> event) { return false; }

        // ^^^ Animation System ^^^ //
        
        // vvv Physics System vvv //
        
        void PhysicsSystem::onEnter()
        {
            listenForEventType(TypePhysicsResetWorld);
            
            m_fixedTimeStep = 1.0f / 60.0f;

            m_collisionConfig = UniqueRef<btDefaultCollisionConfiguration>::create();
            m_dispatcher = UniqueRef<btCollisionDispatcher>::create(m_collisionConfig.get());
            m_broadphase = UniqueRef<btDbvtBroadphase>::create();
            m_solver = UniqueRef<btSequentialImpulseConstraintSolver>::create();
            m_world = UniqueRef<btDiscreteDynamicsWorld>::create(m_dispatcher.get(), 
                                                                 m_broadphase.get(),
                                                                 m_solver.get(),
                                                                 m_collisionConfig.get());

            m_rigidBodies.clear();
            
            m_world->setGravity(btVector3(0.0f, -9.81f, 0.0f));
        }

        void PhysicsSystem::onExit()
        {
            m_world.reset();
            m_solver.reset();
            m_broadphase.reset();
            m_dispatcher.reset();
            m_collisionConfig.reset();

            m_rigidBodies.clear();
        }

        void PhysicsSystem::onUpdate(Timestep dt)
        {
            using component::Transform;
            using component::RigidBody;
            using component::Collider;
            
            auto view = registry->view<Transform, RigidBody>();
            for (auto entity : view)
            {
                auto& transform = view.get<Transform>(entity);
                auto& rigidBody = view.get<RigidBody>(entity);
                auto& collider  = registry->get_or_assign<Collider>(entity);
                
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

					cachedBody.body->activate();

                    transform.m_isPositionOverridden = false;
                }
                
                if (transform.m_isRotationOverridden)
                {
                    btTransform t = cachedBody.body->getWorldTransform();

                    glm::quat rotation = transform.getRotation();
                    t.setRotation(btQuaternion(rotation.x, rotation.y, rotation.z, rotation.w));

                    cachedBody.body->setWorldTransform(t);

					cachedBody.body->activate();
                    
                    transform.m_isRotationOverridden = false;
                }
                if (transform.m_isScaleOverridden)
                {
                    cachedBody.shape->setLocalScaling(btVector3(transform.getScaleX(),
                                                                transform.getScaleY(),
                                                                transform.getScaleZ()));

					cachedBody.body->activate();
                    
                    transform.m_isScaleOverridden = false;
                }
                if (rigidBody.m_isDirty)
                {
                    m_world->removeRigidBody(cachedBody.body.get());
                    
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

                    flags &= ~btRigidBody::CF_NO_CONTACT_RESPONSE;
                    flags |= rigidBody.getProperty(RigidBody::DETECT_COLLISIONS)
                                 ? 0
                                 : btRigidBody::CF_NO_CONTACT_RESPONSE;

                    cachedBody.body->setCollisionFlags(flags);

                    // Rotation Locking
                    btVector3 inertiaTensor = {};

                    inertiaTensor.setX(rigidBody.getProperty(RigidBody::FREEZE_ROTATION_X) ? 0.0f : 1.0f);
                    inertiaTensor.setY(rigidBody.getProperty(RigidBody::FREEZE_ROTATION_Y) ? 0.0f : 1.0f);
                    inertiaTensor.setZ(rigidBody.getProperty(RigidBody::FREEZE_ROTATION_Z) ? 0.0f : 1.0f);

                    cachedBody.body->setInvInertiaDiagLocal(inertiaTensor);

                    cachedBody.body->updateInertiaTensor();

                    // Gravity
                    if (rigidBody.getProperty(RigidBody::USE_GRAVITY))
                    {
                        cachedBody.body->setGravity(m_world->getGravity());
                    }
                    else
                    {
                        cachedBody.body->setGravity({ 0.0f, 0.0f, 0.0f });
                    }

                    m_world->addRigidBody(cachedBody.body.get());

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
            
            m_world->stepSimulation(dt.getSeconds(), 1, m_fixedTimeStep);

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

                transform.m_localPosition = { _pos.x(), _pos.y(), _pos.z() };

                transform.m_localRotation = glm::quat(_rot.w(), _rot.x(), _rot.y(), _rot.z());

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

        void PhysicsSystem::onGuiRender(Timestep dt)
        {
            using component::Transform;
            using component::RigidBody;
            using component::SceneObject;
            auto view = registry->view<SceneObject, RigidBody>();

            ImGui::Begin("Physics");

            for (auto entity : view)
            {
                auto& rb = view.get<RigidBody>(entity);
                ImGui::Text("%s: X(%.3f) Y(%.3f) Z(%.3f)",
                            view.get<SceneObject>(entity).name.c_str(),
                            rb.getVelocity().x,
                            rb.getVelocity().y, 
                            rb.getVelocity().z);
            }

            ImGui::End();
        }

        bool PhysicsSystem::onEvent(Ref<Event> event)
        {
            switch (event->type)
            {
                case TypePhysicsResetWorld:
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
                                                     const component::Transform& transformComponent, 
                                                     const component::Collider&  colliderComponent, 
                                                     const component::RigidBody& rigidBodyComponent)
        {
            // Check if collider was emptied past frame
            if (colliderComponent.empty())
            {
                if (m_rigidBodies.find(entity) != m_rigidBodies.end()) 
                {
                    m_world->removeRigidBody(m_rigidBodies.at(entity)->body.get());
                    m_rigidBodies.erase(entity);
                }
                return;
            }
            
            // TODO: Don't get rid of the body from the world, just update it
            if (colliderComponent.isDirty() && 
                m_rigidBodies.find(entity) != m_rigidBodies.end())
            {
                m_world->removeRigidBody(m_rigidBodies.at(entity)->body.get());
                m_rigidBodies.erase(entity);

                for (auto& shape : const_cast<component::Collider&>(colliderComponent))
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
                        case Collider_Box:
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
                        case Collider_Sphere:
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
                    
                    t.setFromOpenGLMatrix(value_ptr(transformComponent.getMatrixGlobal()));

                    t.setOrigin(btVector3(transformComponent.getPositionX(),
                                          transformComponent.getPositionY(),
                                          transformComponent.getPositionZ()));

                    glm::quat q = quat_cast(transformComponent.getMatrixGlobal());

                    btQuaternion btq = btQuaternion(q.x, q.y, q.z, q.w);
                    t.setRotation(btq);
                    //
                    
                    btVector3 inertia = { 0, 0, 0 };
                    if (rigidBodyComponent.getMass() != 0.0f)
                        shape->calculateLocalInertia(rigidBodyComponent.getMass(), inertia);

                    shape->setLocalScaling({
                        transformComponent.getScaleX(),
                        transformComponent.getScaleY(),
                        transformComponent.getScaleZ()
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
                            case Collider_Box:
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
                            case Collider_Sphere:
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
                    
                    t.setFromOpenGLMatrix(value_ptr(transformComponent.getMatrixGlobal()));

                    t.setOrigin(btVector3(transformComponent.getPositionX(),
                                          transformComponent.getPositionY(),
                                          transformComponent.getPositionZ()));

                    glm::quat q = quat_cast(transformComponent.getMatrixGlobal());

                    btQuaternion btq = btQuaternion(q.x, q.y, q.z, q.w);
                    t.setRotation(btq);

                    btVector3 inertia = { 0, 0, 0 };
                    if (rigidBodyComponent.getMass() != 0.0f)
                        shape->calculateLocalInertia(rigidBodyComponent.getMass(), inertia);

                    shape->setLocalScaling({
                        transformComponent.getScaleX(),
                        transformComponent.getScaleY(),
                        transformComponent.getScaleZ()
                    });
                    
                    motion = Ref<btDefaultMotionState>::create(t);

                    btRigidBody::btRigidBodyConstructionInfo info(rigidBodyComponent.getMass(),
                                                                  motion.get(),
                                                                  shape.get(),
                                                                  inertia);

                    body = Ref<btRigidBody>::create(info);

                    body->setRestitution(1.0f);
                }
                
                m_world->addRigidBody(body.get());

                m_rigidBodies[entity]->body   = body;
                m_rigidBodies[entity]->shape  = shape;
                m_rigidBodies[entity]->motion = motion;
            }
            else
            {
                // RigidBody is already in world, check if dirty
            }
        }

        // ^^^ Physics System ^^^ //

        // vvv Transform Update System vvv //

        void TransformUpdateSystem::onUpdate(Timestep dt)
        {
            using component::Transform;
            using component::Parent;

            auto view = registry->view<Transform>();
            for (auto entity : view)
            {
                auto& ct = view.get(entity);
                if (registry->has<Parent>(entity))
                {
                    auto parent = registry->get<Parent>(entity).parent;
                    if (parent != entt::null)
                    {   
                        auto& pt = registry->get<Transform>(parent);

                        if (!pt.m_localRef) 
                            pt.m_localRef = Ref<Transform>(&pt, [](Transform*) {});

                        ct.m_parentRef = pt.m_localRef;
                    }
                    else ct.m_parentRef = {};
                }
                else ct.m_parentRef = {};
            }
        };

        // ^^^ Transform Update System ^^^ //
        
        // vvv Editor Camera System vvv //

        void EditorCameraSystem::onEnter()
        {
            using component::internal::EditorCamera;
            using component::internal::ExcludeFromHierarchy;

            listenForEventType(TypeKeyPressed);
            listenForEventType(TypeKeyReleased);
            listenForEventType(TypeMouseMoved);
            listenForEventType(TypeEditorViewportResized);

            EditorCamera cam;
            cam.camera = Ref<Camera>::create();
            cam.camera->setProjection(glm::perspective(glm::radians(60.0f), 16.0f / 9.0f, 0.01f, 1000.0f));
            cam.camera->setPosition(glm::vec3(10.0f, 5.0f, 10.0f));
            cam.camera->lookAt(glm::vec3(0.0f, 0.0f, 0.0f));

            auto e = registry->create();
            registry->assign<EditorCamera>(e, cam);
            registry->assign<ExcludeFromHierarchy>(e);
        }

        void EditorCameraSystem::onExit() { }

        void EditorCameraSystem::onUpdate(Timestep dt)
        {
            using component::internal::EditorCamera;

            auto view = registry->view<EditorCamera>();
            for (auto& entity : view)
            {
                auto cam = registry->get<EditorCamera>(entity).camera;
                processCameraUpdate(dt, cam);
            }
        }

        void EditorCameraSystem::onGuiRender(Timestep dt)
        {
            ImGui::Begin("Camera##CameraSettings");

            ImGui::SliderFloat("Move Speed", &m_cameraMoveSpeed, 5.0f, 30.f);
            ImGui::SliderFloat("Turn Speed", &m_cameraRotateSpeed, 0.1f, 50.0f);

            ImGui::End();
        }

        bool EditorCameraSystem::onEvent(Ref<Event> event)
        {
            switch (event->type)
            {
                case TypeKeyPressed:
                {
                    auto e = (KeyPressedEvent) *event;
                    if (e.keycode == Key_LeftAlt && !e.repeatCount)
                    {
                        m_doMoveCamera ^= 1;

                        CursorStateRequestEvent cursorRequest;

                        cursorRequest.state = m_doMoveCamera ? Cursor_Disabled : Cursor_Enabled;

                        postEvent(Event::create(cursorRequest));
                    }
                    if (!m_doMoveCamera) break;
                    
                    if (e.keycode == Key_W)
                        m_cameraMove.z = -1;
                    if (e.keycode == Key_S)
                        m_cameraMove.z = 1;
                    if (e.keycode == Key_D)
                        m_cameraMove.x = 1;
                    if (e.keycode == Key_A)
                        m_cameraMove.x = -1;;
                    if (e.keycode == Key_Space)
                        m_cameraMove.y = 1;
                    if (e.keycode == Key_LeftControl)
                        m_cameraMove.y = -1;
                    break;
                }
                case TypeKeyReleased:
                {
                    auto e = (KeyReleasedEvent) *event;
                    if (e.keycode == Key_W || e.keycode == Key_S)
                        m_cameraMove.z = 0;
                    if (e.keycode == Key_D || e.keycode == Key_A)
                        m_cameraMove.x = 0;
                    if (e.keycode == Key_Space || e.keycode == Key_LeftControl)
                        m_cameraMove.y = 0;
                    break;
                }
                case TypeMouseMoved:
                {
                    if (!m_doMoveCamera) break;

                    auto e           = (MouseMovedEvent) *event;
                    m_cameraRotate.y = e.dx;
                    m_cameraRotate.x = e.dy;

                    break;
                }
                case TypeEditorViewportResized:
                {
                    using component::internal::EditorCamera;
                    
                    auto e    = (EditorViewportResizedEvent) *event;
                    auto view = registry->view<EditorCamera>();

                    auto pc = view.get(*view.begin());

                    glm::mat4 proj = glm::perspective(glm::radians(60.0f), e.width / e.height, 0.01f, 1000.0f);
                    pc.camera->setProjection(proj);
                }
            }
            return false;
        }

        void EditorCameraSystem::processCameraUpdate(Timestep dt, const Ref<Camera>& camera)
        {
            if (!m_doMoveCamera) return;

            glm::vec3 move = m_cameraMove;

            if (move != glm::vec3(0.0f))
                move = glm::normalize(move);

            camera->move(move * m_cameraMoveSpeed * dt.getSeconds());

            static glm::vec3 realRotation = glm::vec3(20.0f, -45.0f, 0.0f);

            realRotation += m_cameraRotate * m_cameraRotateSpeed * dt.getSeconds();
            if (realRotation.x > 89.0f) realRotation.x = 89.0f;
            if (realRotation.x < -89.0f) realRotation.x = -89.0f;

            camera->setRotation(realRotation);

            m_cameraRotate = glm::vec3(0.0f);
        }

        // ^^^ Editor Camera System vvv //

        // vvv Editor Render System vvv //

        void EditorRenderSystem::onEnter()
        {
            listenForEventType(TypeWindowResized);
            listenForAllEvents();

            m_editorViewportBuffer = FrameBuffer::create(1);
            m_editorViewportBuffer->initDepthTexture(1, 1);
            m_editorViewportBuffer->initColorTexture(0, 1, 1, oyl::RGBA8, oyl::Nearest, oyl::Clamp);

            EditorViewportHandleChangedEvent handleChanged;
            handleChanged.handle = m_editorViewportBuffer->getColorHandle(0);
            postEvent(Event::create(handleChanged));
        }

        void EditorRenderSystem::onExit() { }

        void EditorRenderSystem::onUpdate(Timestep dt)
        {
            using component::Transform;
            using component::Renderable;
            using component::PlayerCamera;
            using component::PointLight;
            using component::internal::EditorCamera;
            
            m_editorViewportBuffer->clear();
            m_editorViewportBuffer->bind();

            // TODO: Make EditorCamera like PlayerCamers
            auto camView = registry->view<EditorCamera>();
            Ref<Camera> currentCamera = camView.get(*camView.begin()).camera;

            const auto& skybox = TextureCubeMap::get(DEFAULT_SKYBOX_ALIAS);
            const auto& shader = Shader::get(SKYBOX_SHADER_ALIAS);
            const auto& mesh   = Mesh::get(CUBE_MESH_ALIAS);

            glm::mat4 viewProj = currentCamera->getProjectionMatrix();
            viewProj *= glm::mat4(glm::mat3(currentCamera->getViewMatrix()));
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
                    if (rhs.material == nullptr || rhs.mesh == nullptr)
                        return true;
                    else if (lhs.material == nullptr || lhs.mesh == nullptr)
                        return false;
                    else if (lhs.material->shader != rhs.material->shader)
                        return lhs.material->shader < rhs.material->shader;
                    else
                        return lhs.material < rhs.material;
                });

            Ref<Material> boundMaterial = Material::create();
            Ref<Shader>   tempShader    = Shader::get(LIGHTING_SHADER_ALIAS);

            auto view = registry->view<Renderable>();
            for (auto entity : view)
            {
                Renderable& mr = view.get(entity);

                if (mr.mesh == nullptr || mr.material == nullptr)
                    break;

                if (mr.material != boundMaterial)
                {                    
                    *boundMaterial = *mr.material;
                    boundMaterial->shader = tempShader;

                    boundMaterial->setUniformMat4("u_view", currentCamera->getViewMatrix());
                    boundMaterial->setUniformMat4("u_viewProjection", currentCamera->getViewProjectionMatrix());
                    glm::mat4 viewNormal = glm::mat4(currentCamera->getViewMatrix());
                    viewNormal = glm::inverse(glm::transpose(viewNormal));
                    boundMaterial->setUniformMat3("u_viewNormal", glm::mat4(viewNormal));

                    auto lightView      = registry->view<PointLight>();
                    auto lightProps     = lightView.get(lightView[0]);
                    auto lightTransform = registry->get<Transform>(lightView[0]);

                    boundMaterial->setUniform3f("u_pointLight.position",
                                                viewNormal * glm::vec4(lightTransform.getPosition(), 1.0f));
                    boundMaterial->setUniform3f("u_pointLight.ambient", lightProps.ambient);
                    boundMaterial->setUniform3f("u_pointLight.diffuse", lightProps.diffuse);
                    boundMaterial->setUniform3f("u_pointLight.specular", lightProps.specular);

                    boundMaterial->bind();
                    boundMaterial->applyUniforms();
                }

                const glm::mat4& transform = registry->get_or_assign<Transform>(entity).getMatrixGlobal();

                Renderer::submit(mr.mesh, boundMaterial, transform);
            }

            m_editorViewportBuffer->unbind();
        }

        void EditorRenderSystem::onGuiRender(Timestep dt) { }

        bool EditorRenderSystem::onEvent(Ref<Event> event)
        {
            switch (event->type)
            {
                case TypeWindowResized:
                {
                    auto e = (WindowResizedEvent) *event;
                    m_editorViewportBuffer->updateViewport(e.width, e.height);
                    return true;
                }
            }
            return false;
        }

        void EditorRenderSystem::init() { }

        void EditorRenderSystem::shutdown() { }

        // ^^^ Editor Render System ^^^ //
    }
}
