#include "oylpch.h"

#include "Scenes/System.h"
#include "RenderSystems.h"

#include "Components/Animatable.h"
#include "Components/Camera.h"
#include "Components/Lights.h"
#include "Components/Renderable.h"
#include "Components/Transform.h"

#include "Events/Event.h"

#include "Graphics/Buffer.h"
#include "Graphics/Material.h"
#include "Graphics/Model.h"
#include "Graphics/Shader.h"
#include "Graphics/Texture.h"

#include "Rendering/Renderer.h"

namespace oyl::internal
{
    static bool isRenderableValid(const component::Renderable& r)
    {
        return r.enabled && r.model && r.material && r.material->shader && r.material->albedoMap;
    }

    // vvv Pre Render System vvv //

    void PreRenderSystem::onUpdate()
    {
        using component::Renderable;

        // We sort our mesh renderers based on material properties
        // This will group all of our meshes based on shader first, then material second
        registry->sort<Renderable>(
            [](const Renderable& lhs, const Renderable& rhs)
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
    }

    // ^^^ Pre Render System ^^^ //

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
        using component::Camera;
        using component::PointLight;
        using component::DirectionalLight;
        using component::SpotLight;

        static const auto& skybox = TextureCubeMap::get(DEFAULT_SKYBOX_ALIAS);
        static const auto& shader = Shader::get(SKYBOX_SHADER_ALIAS);
        static const auto& mesh   = Model::get(CUBE_MODEL_ALIAS)->getMeshes()[0];

        Ref<Material> boundMaterial;

        static size_t lastNumCameras = 0;

        auto camView = registry->view<Transform, Camera>();

        int x = m_windowSize.x / 2;
        int y = camView.size() > 2 ? m_windowSize.y / 2 : 0;

        int width = m_windowSize.x;
        if (camView.size() > 1) width /= 2;

        int height = m_windowSize.y;
        if (camView.size() > 2) height /= 2;

        for (auto camera : camView)
        {
            Camera& pc = camView.get<Camera>(camera);

            if (!pc.m_forwardFrameBuffer)
            {
                pc.m_forwardFrameBuffer = FrameBuffer::create(1);
                pc.m_forwardFrameBuffer->initDepthTexture(1, 1);
                pc.m_forwardFrameBuffer->initColorTexture(0, 1, 1,
                                                          TextureFormat::RGBA8,
                                                          TextureFilter::Nearest,
                                                          TextureWrap::ClampToEdge);
            }

            if (m_camerasNeedUpdate || lastNumCameras != camView.size())
            {
                pc.m_forwardFrameBuffer->updateViewport(width, height);

                pc.aspect((float) width / (float) height);
            }

            pc.m_forwardFrameBuffer->clear();
            pc.m_forwardFrameBuffer->bind();

            RenderCommand::setDrawRect(0, 0, width, height);

            glm::mat4 viewProj = pc.projectionMatrix();
            viewProj *= glm::mat4(glm::mat3(pc.viewMatrix()));

            shader->bind();
            shader->setUniformMat4("u_viewProjection", viewProj);

            //pc.skybox->bind(0);
            skybox->bind(0);
            shader->setUniform1i("u_skybox", 0);

            RenderCommand::setDepthDraw(false);
            RenderCommand::setBackfaceCulling(false);
            RenderCommand::drawArrays(mesh.getVertexArray(), mesh.getNumVertices());
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
                    glm::mat3 viewNormal = glm::mat3(inverse(transpose(pc.viewMatrix())));
                    boundMaterial->setUniformMat3("u_viewNormal", viewNormal);

                    int shadowIndex = 0;

                    auto pointLightView = registry->view<PointLight>();
                    int  count     = 0;
                    for (auto light : pointLightView)
                    {
                        PointLight& pointLightProps     = pointLightView.get(light);
                        auto lightTransform = registry->get<Transform>(light);

                        std::string pointLightName = "u_pointLight[" + std::to_string(count) + "]";

                        boundMaterial->setUniform3f(pointLightName + ".position",
                                                    pc.viewMatrix() * glm::vec4(lightTransform.getPositionGlobal(), 1.0f));
                        boundMaterial->setUniform3f(pointLightName + ".ambient",
                                                    pointLightProps.ambient);
                        boundMaterial->setUniform3f(pointLightName + ".diffuse",
                                                    pointLightProps.diffuse);
                        boundMaterial->setUniform3f(pointLightName + ".specular",
                                                    pointLightProps.specular);
                        boundMaterial->setUniform1f(pointLightName + ".range",
                                                    pointLightProps.range);

                        count++;
                        if (count >= 8)
                            break;
                    }

                    for (; count < 8; count++)
                    {
                        std::string pointLightName = "u_pointLight[" + std::to_string(count) + "]";

                        boundMaterial->setUniform3f(pointLightName + ".position", {});
                        boundMaterial->setUniform3f(pointLightName + ".ambient",  {});
                        boundMaterial->setUniform3f(pointLightName + ".diffuse",  {});
                        boundMaterial->setUniform3f(pointLightName + ".specular", {});
                    }

                    auto dirLightView = registry->view<DirectionalLight>();
                    count = 0;
                    for (auto light : dirLightView)
                    {
                        auto& dirLightProps = dirLightView.get(light);
                        auto& lightTransform = registry->get<Transform>(light);

                        std::string dirLightName = "u_dirLight[" + std::to_string(count) + "]";

                        boundMaterial->setUniform3f(dirLightName + ".direction",
                                                    viewNormal * lightTransform.getForwardGlobal());
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
                    
                    for (; count < 8; count++)
                    {
                        std::string dirLightName = "u_dirLight[" + std::to_string(count) + "]";

                        boundMaterial->setUniform3f(dirLightName + ".direction", {});
                        boundMaterial->setUniform3f(dirLightName + ".ambient",   {});
                        boundMaterial->setUniform3f(dirLightName + ".diffuse",   {});
                        boundMaterial->setUniform3f(dirLightName + ".specular",  {});
                    }
                    
                    // TEMPORARY:
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

                if (registry->has<component::Animatable>(entity))
                {
                    auto& anim = registry->get<component::Animatable>(entity);
                    if (anim.getVertexArray())
                    {
                        anim.m_vao->bind();

                        boundMaterial->shader->bind();
                        boundMaterial->shader->setUniform1f("lerpT_curr", glm::mod(anim.m_currentElapsed, 1.0f));
                        boundMaterial->shader->setUniform1f("lerpT_trans", glm::mod(anim.m_transitionElapsed, 1.0f));

                        //Renderer::submit(boundMaterial, anim.m_vao, mr.mesh->getNumVertices(), transform);
                    }
                }
                else
                {
                    Renderer::submit(mr.model, boundMaterial, transform);
                }
            }
        }

        lastNumCameras      = camView.size();
        m_camerasNeedUpdate = false;
    }

    void RenderSystem::onGuiRender() { }

    bool RenderSystem::onEvent(const Event& event)
    {
        switch (event.type)
        {
            case EventType::WindowResized:
                auto e = event_cast<WindowResizedEvent>(event);
                m_windowSize = { e.width, e.height };

                //m_forwardFrameBuffer->updateViewport(e.width, e.height);
                ////m_intermediateFrameBuffer->updateViewport(e.width, e.height);

                //ViewportHandleChangedEvent hcEvent;
                //hcEvent.handle = m_forwardFrameBuffer->getColorHandle(0);
                //m_dispatcher->postEvent(hcEvent);

                m_camerasNeedUpdate = true;

                break;
        }
        return false;
    }

    // ^^^ Render System ^^^ //

    // vvv Gui Render System vvv //

    void GuiRenderSystem::onEnter()
    {
        listenForEventType(EventType::WindowResized);

        m_shader = Shader::create(
            {
                { Shader::Vertex, ENGINE_RES + "shaders/gui.vert" },
                { Shader::Pixel, ENGINE_RES + "shaders/gui.frag" }
            });

        float vertices[] = {
            -0.5f, -0.5f, 0.0f, 0.0f, 1.0f,
            0.5f, -0.5f, 0.0f, 1.0f, 1.0f,
            0.5f, 0.5f, 0.0f, 1.0f, 0.0f,
            -0.5f, 0.5f, 0.0f, 0.0f, 0.0f
        };

        u32 indices[] = {
            0, 2, 1,
            0, 3, 2
        };

        m_vao                 = VertexArray::create();
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
        using component::Camera;

        registry->sort<GuiRenderable>(
            [this](const entt::entity lhs, const entt::entity rhs)
            {
                auto& lguir = registry->get<GuiRenderable>(lhs);
                auto& rguir = registry->get<GuiRenderable>(rhs);

                if (!lguir.enabled || lguir.texture == nullptr)
                    return false;
                if (!rguir.enabled || rguir.texture == nullptr)
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

        auto camView = registry->view<Camera>();

        int x = m_windowSize.x / 2;
        int y = camView.size() > 2 ? m_windowSize.y / 2 : 0;

        int width = m_windowSize.x;
        if (camView.size() > 1) width /= 2;

        int height = m_windowSize.y;
        if (camView.size() > 2) height /= 2;

        glm::vec2 lastLowerClipping = glm::vec2(0.0f);
        glm::vec2 lastUpperClipping = glm::vec2(0.0f);

        RenderCommand::setDrawRect(0, 0, width, height);

        for (auto camera : camView)
        {
            auto& pc = camView.get(camera);

            pc.m_forwardFrameBuffer->bind();

            //u32 playerNum = static_cast<u32>(pc.player);
            //RenderCommand::setDrawRect(!!(playerNum & 1) * x, !(playerNum & 2) * y, width, height);

            m_shader->setUniformMat4("u_projection", pc.orthoMatrix());

            auto view = registry->view<Transform, GuiRenderable>();
            for (auto entity : view)
            {
                auto& transform = view.get<Transform>(entity);
                auto& gui       = view.get<GuiRenderable>(entity);

                if (!gui.enabled || !gui.texture)
                    break;

                if (!(gui.cullingMask & pc.cullingMask))
                    continue;

                if ((!boundTexture || boundTexture != gui.texture) && gui.texture->isLoaded())
                {
                    boundTexture = gui.texture;
                    boundTexture->bind(0);
                }

                if (lastLowerClipping != gui.lowerClipping || lastUpperClipping != gui.upperClipping)
                {
                    lastLowerClipping      = gui.lowerClipping;
                    lastUpperClipping      = gui.upperClipping;
                    glm::vec4 clippingRect = { lastLowerClipping, lastUpperClipping };
                    m_shader->setUniform4f("u_clippingCoords", clippingRect);
                }

                glm::vec3 texSize = glm::vec3(0.0f);
                texSize.x         = (float) gui.texture->getWidth() / (float) gui.texture->getHeight();
                texSize.y         = 1.0f;

                glm::vec3 pos = transform.getPosition();
                pos.y         = -pos.y;
                pos.z         = 0.99f;

                glm::mat4 model = glm::mat4(1.0f);
                model           = glm::translate(model, pos);
                model           = glm::rotate(model, glm::radians(transform.getRotationEuler().z), glm::vec3(0.0f, 0.0f, -1.0f));
                model           = glm::scale(model, transform.getScale());
                model           = glm::scale(model, texSize);

                Renderer::submit(m_shader, m_vao, model);
            }
        }

        RenderCommand::setDepthDraw(true);
    }

    void GuiRenderSystem::onGuiRender() { }

    bool GuiRenderSystem::onEvent(const Event& event)
    {
        switch (event.type)
        {
            case EventType::WindowResized:
                auto e = event_cast<WindowResizedEvent>(event);
                m_windowSize = { e.width, e.height };
                m_shader->bind();
                break;
        }
        return false;
    }

    void ShadowRenderSystem::onEnter()
    {
        m_shader = Shader::create(ENGINE_RES + "shaders/shadowMapping.oylshader");
    }

    void ShadowRenderSystem::onExit() {}
    
    void ShadowRenderSystem::onUpdate()
    {
        using component::Transform;
        using component::Renderable;
        using component::PointLight;
        using component::DirectionalLight;
        using component::SpotLight;

        m_shader->bind();

        RenderCommand::setDepthDraw(true);

        auto view = registry->view<DirectionalLight>();
        int count = 0;
        for (auto light : view)
        {
            auto& dl = view.get(light);

            if (!dl.m_frameBuffer)
            {
                dl.m_frameBuffer = FrameBuffer::create(0);
                dl.m_frameBuffer->initDepthTexture(1024, 1024);
            }

            dl.m_frameBuffer->bind();
            dl.m_frameBuffer->clear();
            
            if (dl.castShadows)
            {
                if (count >= 3) break;

                RenderCommand::setDrawRect(0, 0, dl.m_frameBuffer->getDepthWidth(), dl.m_frameBuffer->getDepthHeight());

                auto& t = registry->get<Transform>(light);

                float projDist = 100.0f;
                glm::mat4 lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, 0.0f, projDist);
                glm::mat4 lightView = glm::lookAt(-t.getForwardGlobal() * projDist * 0.5f,
                                                  glm::vec3(0.0f),
                                                  glm::vec3(0.0f, 1.0f, 0.0f));

                dl.m_lightSpaceMatrix = lightProjection * lightView;

                m_shader->setUniformMat4(0, dl.m_lightSpaceMatrix);

                registry->view<Transform, Renderable>().each([&](Transform& transform, Renderable& renderable)
                {
                    if (!renderable.castShadows || !isRenderableValid(renderable)) return;

                    m_shader->setUniformMat4(4, transform.getMatrixGlobal());
                    for (const auto& mesh : renderable.model->getMeshes())
                        RenderCommand::drawArrays(mesh.getVertexArray(), mesh.getNumVertices());
                });

                count++;
            }
        }
    }

    void ShadowRenderSystem::onGuiRender() {}

    bool ShadowRenderSystem::onEvent(const Event& event)
    {
        return false;
    }

    void UserPostRenderSystem::onEnter()
    {
        listenForEventType(EventType::WindowResized);

        m_forwardFrameBuffer = FrameBuffer::create(1);

        m_forwardFrameBuffer->initColorTexture(0, 1, 1,
                                               TextureFormat::RGBA8,
                                               TextureFilter::Nearest,
                                               TextureWrap::ClampToEdge);

        m_intermediateFrameBuffer = FrameBuffer::create(1);

        m_intermediateFrameBuffer->initColorTexture(0, 1, 1,
                                                    TextureFormat::RGBA8,
                                                    TextureFilter::Nearest,
                                                    TextureWrap::ClampToEdge);

        ViewportHandleChangedEvent hcEvent;
        hcEvent.handle = m_forwardFrameBuffer->getColorHandle(0);
        postEvent(hcEvent);

        m_shader = Shader::create(
            {
                { Shader::Vertex, ENGINE_RES + "shaders/fbopassthrough.vert" },
                { Shader::Pixel, ENGINE_RES + "shaders/fbopassthrough.frag" }
            });

        float vertices[] = {
            -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
            1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
            1.0f, 1.0f, 0.0f, 1.0f, 1.0f,
            -1.0f, 1.0f, 0.0f, 0.0f, 1.0f
        };

        u32 indices[] = {
            0, 1, 2,
            0, 2, 3
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

    void UserPostRenderSystem::onExit() {}

    void UserPostRenderSystem::onUpdate()
    {
        using component::Camera;

        m_forwardFrameBuffer->clear();

        registry->view<Camera>().each([&](Camera& pc)
        {
            uint width = pc.m_forwardFrameBuffer->getColorWidth(), height = pc.m_forwardFrameBuffer->getColorHeight();
            if (m_intermediateFrameBuffer->getColorWidth() != width ||
                m_intermediateFrameBuffer->getColorHeight() != height)
                m_intermediateFrameBuffer->updateViewport(width, height);

            m_intermediateFrameBuffer->clear();
            
            RenderCommand::setDrawRect(0, 0, width, height);

            m_vao->bind();

            RenderCommand::setDepthDraw(false);
            Shader* boundShader = nullptr;

            bool needsBlit = false;

            for (auto pass : pc.postProcessingPasses)
            {
                if (!pass.shader) continue;

                if (!needsBlit)
                {
                    m_intermediateFrameBuffer->bind(FrameBufferContext::Write);
                    pc.m_forwardFrameBuffer->bind(FrameBufferContext::Read);
                    pc.m_forwardFrameBuffer->bindColorAttachment(0);
                }
                else
                {
                    pc.m_forwardFrameBuffer->bind(FrameBufferContext::Write);
                    m_intermediateFrameBuffer->bind(FrameBufferContext::Read);
                    m_intermediateFrameBuffer->bindColorAttachment(0);
                }

                needsBlit ^= 1;

                if (boundShader != pass.shader.get())
                {
                    boundShader = pass.shader.get();
                    boundShader->bind();
                    boundShader->setUniform1i(0, 0);
                }
                pass.applyUniforms();

                RenderCommand::drawIndexed(m_vao);
            }
            RenderCommand::setDepthDraw(true);

            if (needsBlit)
                m_intermediateFrameBuffer->blit(pc.m_forwardFrameBuffer);

            RenderCommand::setDepthDraw(false);
            RenderCommand::setDrawRect(0, 0, m_windowSize.x, m_windowSize.y);

            m_forwardFrameBuffer->bind(FrameBufferContext::Write);
            m_shader->bind();

            //auto& pc = view.get<Camera>(camera);

            pc.m_forwardFrameBuffer->bindColorAttachment(0);

            m_shader->setUniform1i("u_texture", 0);

            glm::vec3 translation(0.0f);
            glm::vec3 scale(1.0f);

            if (registry->size<Camera>() > 1)
                translation.x = 0.5f, scale.x = 0.5f;
            if (registry->size<Camera>() > 2)
                translation.y = 0.5f, scale.y = 0.5f;

            uint playerNum = static_cast<uint>(pc.player);

            if (!(playerNum & 0x01))
                translation.x *= -1;
            if (playerNum & 0x02)
                translation.y *= -1;

            glm::mat4 model = glm::translate(glm::mat4(1.0f), translation);
            model           = glm::scale(model, scale);

            Renderer::submit(m_shader, m_vao, model);
        });

        m_forwardFrameBuffer->unbind(); 

    #if defined(OYL_DISTRIBUTION)
        m_forwardFrameBuffer->blit();
    #endif

        RenderCommand::setDepthDraw(true);
    }

    void UserPostRenderSystem::onGuiRender() {}

    bool UserPostRenderSystem::onEvent(const Event& event)
    {
        switch (event.type)
        {
            case EventType::WindowResized:
                auto e = event_cast<WindowResizedEvent>(event);
                m_windowSize = { e.width, e.height };

                m_forwardFrameBuffer->updateViewport(e.width, e.height);
                //m_intermediateFrameBuffer->updateViewport(e.width, e.height);

                ViewportHandleChangedEvent hcEvent;
                hcEvent.handle = m_forwardFrameBuffer->getColorHandle(0);
                postEvent(hcEvent);

                break;
        }
        return false;
    }

    // ^^^ Gui Render System ^^^ //
}
