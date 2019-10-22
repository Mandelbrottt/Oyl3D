#include "oylpch.h"

#include "System.h"

#include "ECS/Component.h"
#include "ECS/Registry.h"

#include "Events/Event.h"
#include "Events/EventListener.h"

#include "Graphics/Camera.h"

#include "Rendering/Renderer.h"

#include "Scenes/Scene.h"

namespace oyl::ECS
{
    // vvv Generic System vvv //

    System::System(std::string name)
        : Node(std::move(name))
    {
    }

    void System::onEnter()
    {
    }

    void System::onExit()
    {
    }

    void System::onUpdate(Timestep dt)
    {
    }

    void System::onGuiRender()
    {
    }

    void System::setRegistry(Ref<Registry> reg)
    {
        registry = std::move(reg);
    }

    // ^^^ Generic System ^^^ //

    // vvv Render System vvv //

    Ref<RenderSystem> RenderSystem::create()
    {
        return Ref<RenderSystem>::create(_RenderSystem_{});
    }

    void RenderSystem::onEnter()
    {
    }

    void RenderSystem::onExit()
    {
    }

    void RenderSystem::onUpdate(Timestep dt)
    {
        using Component::Transform;
        using Component::MeshRenderer;

        Ref<Registry> reg = Scene::current()->getRegistry();
        
        // We sort our mesh renderers based on material properties
        // This will group all of our meshes based on shader first, then material second
        reg->sort<MeshRenderer>([](const MeshRenderer& lhs, const MeshRenderer& rhs)
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

        Ref<Shader>   boundShader;
        Ref<Material> boundMaterial;

        // TEMPORARY:
        auto        camView       = reg->view<Component::PlayerCamera>();
        Ref<Camera> currentCamera = reg->get<Component::PlayerCamera>(*camView.begin()).camera;

        auto view = reg->view<MeshRenderer>();
        for (const auto& entity : view)
        {
            MeshRenderer& mr = reg->get<MeshRenderer>(entity);

            if (mr.mesh == nullptr || mr.material == nullptr)
                continue;

            if (mr.material->getShader() != boundShader)
            {
                boundShader = mr.material->getShader();
                boundShader->bind();
            }

            if (mr.material != boundMaterial)
            {
                boundMaterial = mr.material;
                mr.material->setUniformMat4("u_view", currentCamera->getViewMatrix());
                mr.material->setUniformMat4("u_projection", currentCamera->getProjectionMatrix());
                boundMaterial->applyUniforms();
            }

            const glm::mat4& transform = reg->get_or_assign<Transform>(entity).getMatrix();

            Renderer::submit(mr.mesh, mr.material, transform);
        }
    }

    void RenderSystem::onGuiRender()
    {
    }

    bool RenderSystem::onEvent(Ref<Event> event)
    {
        return false;
    }

    // ^^^ Render System ^^^ //

    // vvv Oracle Camera System vvv //

    Ref<OracleCameraSystem> OracleCameraSystem::create()
    {
        return Ref<OracleCameraSystem>::create(_OracleCameraSystem_{});
    }

    void OracleCameraSystem::onEnter()
    {
        addToEventMask(TypeKeyPressed);
        addToEventMask(TypeKeyReleased);
        addToEventMask(TypeMouseMoved);
        addToEventMask(TypeViewportResized);

        Component::PlayerCamera cam;
        cam.player = 0;
        cam.camera = Ref<Camera>::create();
        cam.camera->setProjection(glm::perspective(glm::radians(60.0f), 16.0f / 9.0f, 0.01f, 1000.0f));
        cam.camera->setPosition(glm::vec3(0.0f));
        cam.camera->lookAt(glm::vec3(0.0f, 0.0f, -1.0f));

        auto e = registry->create();
        registry->assign<Component::PlayerCamera>(e, cam);
    }

    void OracleCameraSystem::onExit()
    {
    }

    void OracleCameraSystem::onUpdate(Timestep dt)
    {
        auto view = registry->view<Component::PlayerCamera>();
        for (auto& entity : view)
        {
            auto cam = registry->get<Component::PlayerCamera>(entity).camera;
            processCameraUpdate(dt, cam);
        }
    }

    void OracleCameraSystem::onGuiRender()
    {
        ImGui::Begin("Camera");

        ImGui::SliderFloat("Move Speed", &m_cameraMoveSpeed, 0.1f, 10.f);
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
                if (e.keycode == Key_LeftAlt && !e.repeatCount)
                {
                    m_doMoveCamera ^= 1;

                    CursorStateRequestEvent cursorRequest;

                    cursorRequest.state = m_doMoveCamera ? Cursor_Disabled : Cursor_Enabled;

                    postEvent(Event::create(cursorRequest));
                }
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
                auto e           = (MouseMovedEvent) *event;
                m_cameraRotate.y = e.dx;
                m_cameraRotate.x = e.dy;
                break;
            }
            case TypeViewportResized:
            {
                auto e    = (ViewportResizedEvent) *event;
                auto view = registry->view<Component::PlayerCamera>();
                for (auto entity : view)
                {
                    auto pc = registry->get<Component::PlayerCamera>(entity);
                    if (pc.player == e.id)
                    {
                        glm::mat4 proj = glm::perspective(glm::radians(60.0f), e.width / e.height, 0.01f, 1000.0f);
                        pc.camera->setProjection(proj);
                    }
                }
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

        //Entity      camEntity = *registry->view<Component::PlayerCamera>().begin();
        //Ref<Camera> cam       = registry->get<Component::PlayerCamera>(camEntity).camera;

        camera->move(move * m_cameraMoveSpeed * dt.getSeconds());

        static glm::vec3 realRotation = glm::vec3(0.0f);

        realRotation += m_cameraRotate * m_cameraRotateSpeed * dt.getSeconds();
        if (realRotation.x > 89.0f) realRotation.x = 89.0f;
        if (realRotation.x < -89.0f) realRotation.x = -89.0f;

        camera->setRotation(realRotation);

        m_cameraRotate = glm::vec3(0.0f);
    }

    // ^^^ Oracle Camera System vvv //
}
