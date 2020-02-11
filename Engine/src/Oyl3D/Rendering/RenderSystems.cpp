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
#include "Graphics/Mesh.h"
#include "Graphics/Shader.h"
#include "Graphics/Texture.h"

#include "Rendering/Renderer.h"

namespace oyl::internal
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
        using component::Camera;
        using component::PointLight;

        const auto& skybox = TextureCubeMap::get(DEFAULT_SKYBOX_ALIAS);
        const auto& shader = Shader::get(SKYBOX_SHADER_ALIAS);
        const auto& mesh   = Mesh::get(CUBE_MESH_ALIAS);

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

            //if (m_intermediateFrameBuffer->getWidth() != width || 
            //    m_intermediateFrameBuffer->getHeight() != height)
            //    m_intermediateFrameBuffer->updateViewport(width, height);

            pc.m_forwardFrameBuffer->clear();
            pc.m_forwardFrameBuffer->bind();

            RenderCommand::setDrawRect(0, 0, width, height);

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
                    int  count     = 0;
                    for (auto light : lightView)
                    {
                        auto lightProps     = lightView.get(light);
                        auto lightTransform = registry->get<Transform>(light);

                        boundMaterial->setUniform3f("u_pointLight[" + std::to_string(count) + "].position",
                                                    pc.viewMatrix() * glm::vec4(lightTransform.getPositionGlobal(), 1.0f));
                        boundMaterial->setUniform3f("u_pointLight[" + std::to_string(count) + "].ambient",
                                                    lightProps.ambient);
                        boundMaterial->setUniform3f("u_pointLight[" + std::to_string(count) + "].diffuse",
                                                    lightProps.diffuse);
                        boundMaterial->setUniform3f("u_pointLight[" + std::to_string(count) + "].specular",
                                                    lightProps.specular);
                        boundMaterial->setUniform1f("u_pointLight[" + std::to_string(count) + "].attenK",
                                                    lightProps.attenuation.x);
                        boundMaterial->setUniform1f("u_pointLight[" + std::to_string(count) + "].attenL",
                                                    lightProps.attenuation.y);
                        boundMaterial->setUniform1f("u_pointLight[" + std::to_string(count) + "].attenQ",
                                                    lightProps.attenuation.z);
                        count++;
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

                        Renderer::submit(boundMaterial, anim.m_vao, mr.mesh->getNumVertices(), transform);
                    }
                }
                else
                {
                    Renderer::submit(mr.mesh, boundMaterial, transform);
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

    void PostRenderSystem::onEnter()
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

    void PostRenderSystem::onExit() {}

    void PostRenderSystem::onUpdate()
    {
        using component::Camera;

        m_forwardFrameBuffer->clear();

        registry->view<Camera>().each([&](Camera& pc)
        {
            uint width = pc.m_forwardFrameBuffer->getWidth(), height = pc.m_forwardFrameBuffer->getHeight();
            if (m_intermediateFrameBuffer->getWidth() != width ||
                m_intermediateFrameBuffer->getHeight() != height)
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

    void PostRenderSystem::onGuiRender() {}

    bool PostRenderSystem::onEvent(const Event& event)
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
