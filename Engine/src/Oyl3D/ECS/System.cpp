#include "oylpch.h"

#include "System.h"

#include "ECS/component.h"
#include "ECS/Registry.h"

#include "Events/Event.h"
#include "Events/EventListener.h"

#include "Graphics/Camera.h"
#include "Graphics/Material.h"
#include "Graphics/Shader.h"

#include "Rendering/Renderer.h"

#include "Scenes/Scene.h"

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

    void System::setRegistry(Ref<Registry> reg)
    {
        registry = std::move(reg);
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

    // ^^^ Render System ^^^ //

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
