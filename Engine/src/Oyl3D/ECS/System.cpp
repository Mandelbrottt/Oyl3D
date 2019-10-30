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

namespace oyl::ECS
{
    // vvv Generic System vvv //

    void System::onEnter()
    {
    }

    void System::onExit()
    {
    }

    void System::onUpdate(Timestep dt)
    {
    }

    void System::onGuiRender(Timestep dt)
    {
    }

    // ^^^ Generic System ^^^ //

    // vvv Render System vvv //

    void RenderSystem::onEnter()
    {
    }

    void RenderSystem::onExit()
    {
    }

    void RenderSystem::onUpdate(Timestep dt)
    {
        using component::Transform;
        using component::Renderable;
        using component::PlayerCamera;
        using component::PointLight;
        using component::internal::EditorCamera;

        Ref<Registry> reg = Scene::current()->getRegistry();
        
        // We sort our mesh renderers based on material properties
        // This will group all of our meshes based on shader first, then material second
        reg->sort<Renderable>([](const Renderable& lhs, const Renderable& rhs)
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
        auto        camView       = reg->view<EditorCamera>();
        Ref<Camera> currentCamera = camView.get(*camView.begin()).camera;

        auto view = reg->view<Renderable>();
        for (const auto& entity : view)
        {
            Renderable& mr = reg->get<Renderable>(entity);

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

                auto  lightView =       registry->view<PointLight>();
                auto  lightProps =      lightView.get(lightView[0]);
                auto  lightTransform =  registry->get<Transform>(lightView[0]);
                
                mr.material->setUniform3f("u_pointLight.position",  
                                          viewNormal * glm::vec4(lightTransform.position, 1.0f));
                mr.material->setUniform3f("u_pointLight.ambient",   lightProps.ambient);
                mr.material->setUniform3f("u_pointLight.diffuse",   lightProps.diffuse);
                mr.material->setUniform3f("u_pointLight.specular",  lightProps.specular);

                // TEMPORARY:
                boundMaterial->bind();
                boundMaterial->applyUniforms();
            }
            
            const glm::mat4& transform = reg->get_or_assign<Transform>(entity).getMatrix();

            Renderer::submit(mr.mesh, mr.material, transform);
        }
    }

    void RenderSystem::onGuiRender(Timestep dt)
    {
    }

    bool RenderSystem::onEvent(Ref<Event> event)
    {
        return false;
    }
    
    // ^^^ Render System //
    
    // vvv Physics System vvv //
    
    void PhysicsSystem::onEnter()
    {
        m_fixedTimeStep = 1.0f / 60.0f;

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

            btTransform newT = {};
            m_rigidBodies[entity]->motion->getWorldTransform(newT);

            btVector3 _pos = newT.getOrigin();
            transform.position = { _pos.x(), _pos.y(), _pos.z() };
        }
        
        //if (registry->view<component::RigidBody>().size() != m_rigidBodies.size())
        //{
        //    
        //}

        //btRigidBody::getCollisionShape()->getShapeType();
        
        m_world->stepSimulation(dt.getSeconds(), 1, m_fixedTimeStep);
    }

    void PhysicsSystem::onGuiRender(Timestep dt)
    {
    }

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
                t.setIdentity();
                glm::vec3 origin = transformComponent.position;
                t.setOrigin(btVector3(origin.x, origin.y, origin.z));

                glm::vec3 up = glm::vec3(transformComponent.getMatrix()[1]);
                
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
                t.setIdentity();
                glm::vec3 origin = transformComponent.position;
                t.setOrigin(btVector3(origin.x, origin.y, origin.z));
                shape = Ref<btSphereShape>::create(bodyComponent.radius);

                btVector3 inertia = { 0, 0, 0 };
                if (bodyComponent.mass != 0.0f)
                    shape->calculateLocalInertia(bodyComponent.mass, inertia);

                motion = Ref<btDefaultMotionState>::create(t);

                btRigidBody::btRigidBodyConstructionInfo info(bodyComponent.mass, motion.get(), shape.get());

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

    void RenderSystem::init()
    {
    }

    void RenderSystem::shutdown()
    {
    }

    // ^^^ Render System ^^^ //

    // ^^^ Physics System ^^^ //
    
    // vvv Oracle Camera System vvv //

    namespace internal
    {
        void OracleCameraSystem::onEnter()
        {
            using component::internal::EditorCamera;
            using component::internal::ExcludeFromHierarchy;

            addToEventMask(TypeKeyPressed);
            addToEventMask(TypeKeyReleased);
            addToEventMask(TypeMouseMoved);
            addToEventMask(TypeViewportResized);

            EditorCamera cam;
            cam.camera = Ref<Camera>::create();
            cam.camera->setProjection(glm::perspective(glm::radians(60.0f), 16.0f / 9.0f, 0.01f, 1000.0f));
            cam.camera->setPosition(glm::vec3(10.0f, 5.0f, 10.0f));
            cam.camera->lookAt(glm::vec3(0.0f, 0.0f, 0.0f));

            auto e = registry->create();
            registry->assign<EditorCamera>(e, cam);
            registry->assign<ExcludeFromHierarchy>(e);
        }

        void OracleCameraSystem::onExit()
        {
        }

        void OracleCameraSystem::onUpdate(Timestep dt)
        {
            using component::internal::EditorCamera;

            auto view = registry->view<EditorCamera>();
            for (auto& entity : view)
            {
                auto cam = registry->get<EditorCamera>(entity).camera;
                processCameraUpdate(dt, cam);
            }
        }

        void OracleCameraSystem::onGuiRender(Timestep dt)
        {
            ImGui::Begin("Camera");

            ImGui::SliderFloat("Move Speed", &m_cameraMoveSpeed, 5.0f, 30.f);
            ImGui::SliderFloat("Turn Speed", &m_cameraRotateSpeed, 0.1f, 50.0f);

            ImGui::End();
        }

        bool OracleCameraSystem::onEvent(Ref<Event> event)
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
                case TypeViewportResized:
                {
                    using component::internal::EditorCamera;
                    
                    auto e    = (ViewportResizedEvent) *event;
                    auto view = registry->view<EditorCamera>();

                    auto pc = view.get(*view.begin());

                    glm::mat4 proj = glm::perspective(glm::radians(60.0f), e.width / e.height, 0.01f, 1000.0f);
                    pc.camera->setProjection(proj);
                }
            }
            return false;
        }

        void OracleCameraSystem::processCameraUpdate(Timestep dt, const Ref<Camera>& camera)
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
    }

    // ^^^ Oracle Camera System vvv //
}
