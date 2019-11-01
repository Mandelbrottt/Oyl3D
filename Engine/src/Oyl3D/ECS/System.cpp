#include "oylpch.h"

#include "ECS/System.h"
#include "ECS/SystemImpl.h"

#include "ECS/Component.h"
#include "ECS/Registry.h"

#include "Events/Event.h"
#include "Events/EventListener.h"

#include "Graphics/Camera.h"
#include "Graphics/Material.h"
#include "Graphics/Shader.h"

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
            
            // We sort our mesh renderers based on material properties
            // This will group all of our meshes based on shader first, then material second
            registry->sort<Renderable>(
                [](const Renderable& lhs, const Renderable& rhs)
                {
                    if (rhs.material == nullptr || rhs.mesh == nullptr)
                        return false;
                    else if (lhs.material == nullptr || lhs.mesh == nullptr)
                        return true;
                    else if (lhs.material->getShader() != rhs.material->getShader())
                        return lhs.material->getShader() < rhs.material->getShader();
                    else
                        return lhs.material < rhs.material;
                });

            Ref<Material> boundMaterial;

            auto camView = registry->view<Transform, PlayerCamera>();
            OYL_ASSERT(camView.size() == 1, "Only one camera supported!");

            for (auto camera : camView)
            {
                //Ref<Camera> currentCamera = camView.get(camera).camera;

                auto view = registry->view<Transform, Renderable>();
                for (const auto& entity : view)
                {
                    Renderable& mr = view.get<Renderable>(entity);

                    if (mr.mesh == nullptr || 
                        mr.material == nullptr || 
                        mr.material->getShader() == nullptr)
                        continue;
                    
                    if (mr.material != boundMaterial)
                    {
                        boundMaterial = mr.material;

                        glm::mat4 viewMatrix = camView.get<Transform>(camera).getMatrixGlobal();
                        viewMatrix = inverse(viewMatrix);

                        const glm::mat4& projectionMatrix = camView.get<PlayerCamera>(camera).projection;
                        
                        mr.material->setUniformMat4("u_view", viewMatrix);
                        mr.material->setUniformMat4("u_viewProjection", projectionMatrix * viewMatrix);
                        glm::mat4 viewNormal = inverse(transpose(viewMatrix));
                        mr.material->setUniformMat3("u_viewNormal", glm::mat3(viewNormal));

                        auto lightView =      registry->view<PointLight>();
                        auto lightProps =     lightView.get(lightView[0]);
                        auto lightTransform = registry->get<Transform>(lightView[0]);
                        
                        mr.material->setUniform3f("u_pointLight.position",  
                                                  viewNormal * glm::vec4(lightTransform.getPosition(), 1.0f));
                        mr.material->setUniform3f("u_pointLight.ambient",   lightProps.ambient);
                        mr.material->setUniform3f("u_pointLight.diffuse",   lightProps.diffuse);
                        mr.material->setUniform3f("u_pointLight.specular",  lightProps.specular);

                        // TEMPORARY:
                        boundMaterial->bind();
                        boundMaterial->applyUniforms();
                    }
                    
                    glm::mat4 transform = view.get<Transform>(entity).getMatrixGlobal();

                    Renderer::submit(mr.mesh, mr.material, transform);
                }
            }
        }

        void RenderSystem::onGuiRender(Timestep dt) { }

        bool RenderSystem::onEvent(Ref<Event> event)
        {
            return false;
        }

        void RenderSystem::init() { }

        void RenderSystem::shutdown() { }
        
        // ^^^ Render System //
        
        // vvv Physics System vvv //
        
        void PhysicsSystem::onEnter()
        {
            m_fixedTimeStep = 1.0f / 60.0f;

            m_rigidBodies.clear();

            m_collisionConfig = UniqueRef<btDefaultCollisionConfiguration>::create();
            m_dispatcher = UniqueRef<btCollisionDispatcher>::create(m_collisionConfig.get());
            m_broadphase = UniqueRef<btDbvtBroadphase>::create();
            m_solver = UniqueRef<btSequentialImpulseConstraintSolver>::create();
            m_world = UniqueRef<btDiscreteDynamicsWorld>::create(m_dispatcher.get(), 
                                                                 m_broadphase.get(),
                                                                 m_solver.get(),
                                                                 m_collisionConfig.get());
            
            m_world->setGravity(btVector3(0.0f, -10.0f, 0.0f));
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
            
            auto view = registry->view<Transform, RigidBody>();
            for (auto entity : view)
            {
                auto& transform = view.get<Transform>(entity);
                auto& rigidBody = view.get<RigidBody>(entity);

                rigidBody.id = entity;
                if (m_rigidBodies.find(entity) == m_rigidBodies.end())
                {
                    addRigidBody(entity, transform, rigidBody);
                }

                btVector3 _pos = {};
                btQuaternion _rot = {};
                if (transform.m_isPositionOverridden || transform.m_isRotationOverridden)
                {
                    m_world->removeRigidBody(m_rigidBodies[entity]->body.get());
                    addRigidBody(entity, transform, rigidBody);

                    transform.m_isPositionOverridden = false;
                    transform.m_isRotationOverridden = false;
                }
                else
                {
                    btTransform t = m_rigidBodies[entity]->body->getWorldTransform();

                    _pos = t.getOrigin();
                    _rot = t.getRotation();
                    
                    transform.m_localPosition = { _pos.x(), _pos.y(), _pos.z() };

                    glm::vec3 newRot = {};
                    _rot.getEulerZYX(newRot.z, newRot.y, newRot.x);
                    newRot = glm::degrees(newRot);
                    
                    transform.m_localEulerRotation = newRot;
                    
                    transform.m_isLocalDirty = true;
                }
            }
                        
            m_world->stepSimulation(dt.getSeconds(), 1, m_fixedTimeStep);
        }

        void PhysicsSystem::onGuiRender(Timestep dt) { }

        bool PhysicsSystem::onEvent(Ref<Event> event)
        {
            return false;
        }

        void PhysicsSystem::addRigidBody(Entity entity, 
                                         const component::Transform& transformComponent, 
                                         const component::RigidBody& bodyComponent)
        {
            Ref<btCollisionShape> shape  = nullptr;
            Ref<btMotionState>  motion = nullptr;
            Ref<btRigidBody>    body   = nullptr;
            
            switch (bodyComponent.type)
            {
                case RigidBody_StaticPlane:
                {
                    btTransform t;
                    //t.setIdentity();
                    //glm::vec3 origin = transformComponent.getPosition();
                    //t.setOrigin(btVector3(origin.x, origin.y, origin.z));
                    
                    glm::mat4 newTransform = transformComponent.getMatrixGlobal();
                    t.setFromOpenGLMatrix(value_ptr(newTransform));
                    
                    glm::vec3 up = transformComponent.getUpGlobal();
                    
                    shape  = Ref<btStaticPlaneShape>::create(btVector3(up.x, up.y, up.z), 0);
                    motion = Ref<btDefaultMotionState>::create(t);

                    btRigidBody::btRigidBodyConstructionInfo info(0.0f, motion.get(), shape.get());

                    body = Ref<btRigidBody>::create(info);
                    
                    break;
                }
                case RigidBody_Box:
                {
                    OYL_ASSERT(false, "Not Yet Implemented!");
                    break;
                }
                case RigidBody_Sphere:
                {
                    btTransform t;
                    //t.setIdentity();
                    //glm::vec3 origin = transformComponent.getPosition();
                    //t.setOrigin(btVector3(origin.x, origin.y, origin.z));
                    
                    glm::mat4 newTransform = transformComponent.getMatrixGlobal();
                    t.setFromOpenGLMatrix(value_ptr(newTransform));
                    
                    shape = Ref<btSphereShape>::create(bodyComponent.radius);

                    btVector3 inertia = { 0, 0, 0 };
                    if (bodyComponent.mass != 0.0f)
                        shape->calculateLocalInertia(bodyComponent.mass, inertia);

                    motion = Ref<btDefaultMotionState>::create(t);

                    btRigidBody::btRigidBodyConstructionInfo info(bodyComponent.mass, motion.get(), shape.get(), inertia);

                    body = Ref<btRigidBody>::create(info);

                    break;
                }
            }

            m_world->addRigidBody(body.get());
            
            m_rigidBodies[entity] = Ref<RigidBodyInfo>::create();
            m_rigidBodies[entity]->body = body;
            m_rigidBodies[entity]->shape = shape;
            m_rigidBodies[entity]->motion = motion;
        }

        // ^^^ Physics System ^^^ //
        
        // vvv Editor Camera System vvv //

        void EditorCameraSystem::onEnter()
        {
            using component::internal::EditorCamera;
            using component::internal::ExcludeFromHierarchy;

            addToEventMask(TypeKeyPressed);
            addToEventMask(TypeKeyReleased);
            addToEventMask(TypeMouseMoved);
            addToEventMask(TypeEditorViewportResized);

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

    void EditorRenderSystem::onEnter() { }

    void EditorRenderSystem::onExit() { }

    void EditorRenderSystem::onUpdate(Timestep dt)
    {
        using component::Transform;
        using component::Renderable;
        using component::PlayerCamera;
        using component::PointLight;
        using component::internal::EditorCamera;

        // We sort our mesh renderers based on material properties
        // This will group all of our meshes based on shader first, then material second
        registry->sort<Renderable>(
            [](const Renderable& lhs, const Renderable& rhs)
            {
                if (rhs.material == nullptr || rhs.mesh == nullptr)
                    return false;
                else if (lhs.material == nullptr || lhs.mesh == nullptr)
                    return true;
                else if (lhs.material->getShader() != rhs.material->getShader())
                    return lhs.material->getShader() < rhs.material->getShader();
                else
                    return lhs.material < rhs.material;
            });

        Ref<Material> boundMaterial;

        // TEMPORARY
        auto        camView = registry->view<EditorCamera>();
        Ref<Camera> currentCamera = camView.get(*camView.begin()).camera;

        auto view = registry->view<Renderable>();
        for (const auto& entity : view)
        {
            Renderable& mr = registry->get<Renderable>(entity);

            if (mr.mesh == nullptr ||
                mr.material == nullptr ||
                mr.material->getShader() == nullptr)
                continue;

            if (mr.material != boundMaterial)
            {
                boundMaterial = mr.material;

                mr.material->setUniformMat4("u_view", currentCamera->getViewMatrix());
                mr.material->setUniformMat4("u_viewProjection", currentCamera->getViewProjectionMatrix());
                glm::mat4 viewNormal = glm::mat4(currentCamera->getViewMatrix());
                viewNormal = glm::inverse(glm::transpose(viewNormal));
                mr.material->setUniformMat3("u_viewNormal", glm::mat4(viewNormal));

                auto  lightView = registry->view<PointLight>();
                auto  lightProps = lightView.get(lightView[0]);
                auto  lightTransform = registry->get<Transform>(lightView[0]);

                mr.material->setUniform3f("u_pointLight.position",
                                          viewNormal * glm::vec4(lightTransform.getPosition(), 1.0f));
                mr.material->setUniform3f("u_pointLight.ambient", lightProps.ambient);
                mr.material->setUniform3f("u_pointLight.diffuse", lightProps.diffuse);
                mr.material->setUniform3f("u_pointLight.specular", lightProps.specular);

                // TEMPORARY:
                boundMaterial->bind();
                boundMaterial->applyUniforms();
            }

            const glm::mat4& transform = registry->get_or_assign<Transform>(entity).getMatrixGlobal();

            Renderer::submit(mr.mesh, mr.material, transform);
        }
    }

    void EditorRenderSystem::onGuiRender(Timestep dt) { }

    bool EditorRenderSystem::onEvent(Ref<Event> event)
    {
        return false;
    }

    void EditorRenderSystem::init() { }

    void EditorRenderSystem::shutdown() { }

    // ^^^ Editor Render System ^^ //

    }
}
