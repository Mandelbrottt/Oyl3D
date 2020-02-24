#include "oylpch.h"
#include "EditorSystems.h"

#include "App/Window.h"

#include "Components/Camera.h"
#include "Components/Lights.h"
#include "Components/Renderable.h"
#include "Components/Transform.h"

#include "Events/Event.h"
#include "Events/EventListener.h"

#include "Graphics/Buffer.h"
#include "Graphics/EditorCamera.h"
#include "Graphics/Material.h"
#include "Graphics/Model.h"
#include "Graphics/Shader.h"
#include "Graphics/Texture.h"

#include "Input/Input.h"

#include "Rendering/Renderer.h"

namespace oyl::internal
{
    void EditorCameraSystem::onEnter()
    {
        //listenForEventType(EventType::KeyPressed);
        //listenForEventType(EventType::KeyReleased);
        //listenForEventType(EventType::MouseMoved);
        //listenForEventType(EventType::MousePressed);
        //listenForEventType(EventType::MouseReleased);
        listenForEventType(EventType::EditorViewportResized);
        listenForEventType(EventType::EditorCameraMoveRequest);

        m_camera = Ref<EditorCamera>::create();
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
            m_cameraMove   = glm::vec3(0.0f);
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

        ImGui::SliderFloat("Move Speed", &m_cameraMoveSpeed, 5.0f, 100.f);
        ImGui::SliderFloat("Turn Speed", &m_cameraRotateSpeed, 0.1f, 50.0f);

        ImGui::End();
    }

    bool EditorCameraSystem::onEvent(const Event& event)
    {
        switch (event.type)
        {
            case EventType::KeyPressed:
            {
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
                auto e           = event_cast<MouseMovedEvent>(event);
                m_cameraRotate.y = e.dx;
                m_cameraRotate.x = e.dy;

                break;
            }
            case EventType::EditorCameraMoveRequest:
            {
                auto                    e = event_cast<EditorCameraMoveRequestEvent>(event);
                CursorStateRequestEvent cursorRequest;
                if (e.doMove)
                {
                    listenForEventType(EventType::MouseMoved);
                    listenForEventType(EventType::KeyPressed);
                    listenForEventType(EventType::KeyReleased);
                    cursorRequest.state = CursorState::Disabled;
                }
                else
                {
                    ignoreEventType(EventType::MouseMoved);
                    ignoreEventType(EventType::KeyPressed);
                    ignoreEventType(EventType::KeyReleased);
                    cursorRequest.state = CursorState::Normal;
                }

                if (m_doMoveCamera != e.doMove)
                    postEvent(cursorRequest);
                m_doMoveCamera = e.doMove;

                break;
            }
            case EventType::EditorViewportResized:
            {
                auto      e    = event_cast<EditorViewportResizedEvent>(event);
                glm::mat4 proj = glm::perspective(glm::radians(60.0f), e.width / e.height, 0.1f, 1000.0f);
                m_camera->setProjection(proj);

                break;
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
        m_editorViewportBuffer->initColorTexture(0, 1, 1,
                                                 TextureFormat::RGBA8,
                                                 TextureFilter::Nearest,
                                                 TextureWrap::ClampToEdge);

        EditorViewportHandleChangedEvent handleChanged;
        handleChanged.handle = m_editorViewportBuffer->getColorHandle(0);
        postEvent(handleChanged);
    }

    void EditorRenderSystem::onExit() { }

    void EditorRenderSystem::onUpdate()
    {
        using component::Transform;
        using component::Renderable;
        using component::Camera;
        using component::PointLight;
        using component::DirectionalLight;

        m_editorViewportBuffer->bind();
        m_editorViewportBuffer->clear();

        RenderCommand::setDrawRect(0, 0, m_windowSize.x, m_windowSize.y);

        static const auto& skybox = TextureCubeMap::get(DEFAULT_SKYBOX_ALIAS);
        static const auto& shader = Shader::get(SKYBOX_SHADER_ALIAS);
        static const auto& mesh   = Model::get(CUBE_MODEL_ALIAS)->getMeshes()[0];

        glm::mat4 viewProj = m_targetCamera->getProjectionMatrix();
        viewProj *= glm::mat4(glm::mat3(m_targetCamera->getViewMatrix()));
        shader->bind();
        shader->setUniformMat4("u_viewProjection", viewProj);

        skybox->bind(0);
        shader->setUniform1i("u_skybox", 0);
        
        RenderCommand::setDepthDraw(false);
        RenderCommand::setBackfaceCulling(false);
        RenderCommand::drawArrays(mesh.getVertexArray(), mesh.getNumVertices());
        RenderCommand::setBackfaceCulling(true);
        RenderCommand::setDepthDraw(true);

        // We sort our mesh renderers based on material properties
        // This will group all of our meshes based on shader first, then material second
        registry->sort<Renderable>(
            [](const Renderable& lhs, const Renderable& rhs)
            {
                if (lhs.material == nullptr || lhs.model == nullptr)
                    return false;
                if (rhs.material == nullptr || rhs.model == nullptr)
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

            if (mr.model == nullptr || mr.material == nullptr)
                break;

            if (mr.material != boundMaterial)
            {
                *boundMaterial        = *mr.material;
                boundMaterial->shader = tempShader;

                boundMaterial->setUniformMat4("u_view", m_targetCamera->getViewMatrix());
                boundMaterial->setUniformMat4("u_viewProjection", m_targetCamera->getViewProjectionMatrix());
                glm::mat3 viewNormal = glm::mat3(m_targetCamera->getViewMatrix());
                viewNormal           = inverse(transpose(viewNormal));
                boundMaterial->setUniformMat3("u_viewNormal", viewNormal);

                int shadowIndex = 0;

                auto lightView = registry->view<PointLight>();
                int  count     = 0;
                for (auto light : lightView)
                {
                    auto lightProps     = lightView.get(light);
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

                auto dirLightView = registry->view<DirectionalLight>();
                count = 0;
                for (auto light : dirLightView)
                {
                    auto& dirLightProps = dirLightView.get(light);

                    std::string dirLightName = "u_dirLight[" + std::to_string(count) + "]";

                    boundMaterial->setUniform3f(dirLightName + ".direction",
                                                viewNormal * registry->get<Transform>(light).getForwardGlobal());
                    boundMaterial->setUniform3f(dirLightName + ".ambient",
                                                dirLightProps.ambient);
                    boundMaterial->setUniform3f(dirLightName + ".diffuse",
                                                dirLightProps.diffuse);
                    boundMaterial->setUniform3f(dirLightName + ".specular",
                                                dirLightProps.specular);

                    if (dirLightProps.castShadows && shadowIndex < 3)
                    {
                        boundMaterial->setUniformMat4("u_lightSpaceMatrix", dirLightProps.m_lightSpaceMatrix);

                        std::string shadowName = "u_shadow[" + std::to_string(shadowIndex) + "]";
                        boundMaterial->setUniform1i(shadowName + ".type", 2);
                        boundMaterial->setUniform1i(shadowName + ".map", 5 + shadowIndex);
                        dirLightProps.m_frameBuffer->bindDepthAttachment(5 + shadowIndex);

                        shadowIndex++;
                    }

                    count++;
                    if (count >= 8)
                        break;
                }

                boundMaterial->bind();
                boundMaterial->applyUniforms();
            }

            auto&     transformComponent = view.get<Transform>(entity);
            glm::mat4 transform          = transformComponent.getMatrixGlobal();

            glm::bvec3 mirror = transformComponent.getMirrorGlobal();
            if (!(mirror.x ^ mirror.y ^ mirror.z) != doCulling)
            {
                doCulling ^= 1;
                RenderCommand::setBackfaceCulling(doCulling);
            }

            Renderer::submit(mr.model, boundMaterial, transform);
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
                return false;
            }
            case EventType::EditorCameraChanged:
            {
                auto e         = event_cast<EditorCameraChangedEvent>(event);
                m_targetCamera = *e.camera;
            }
        }
        return false;
    }

    void EditorRenderSystem::init() { }

    void EditorRenderSystem::shutdown() { }
}
