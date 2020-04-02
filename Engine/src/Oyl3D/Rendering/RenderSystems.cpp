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

    static bool _isDeferred(const Ref<Shader>& shader)
    {
        static Ref<Shader> deferredStatic = Shader::get(DEFERRED_STATIC_PRE_SHADER_ALIAS);
        static Ref<Shader> deferredVertex = Shader::get(DEFERRED_VERTEX_PRE_SHADER_ALIAS);
        static Ref<Shader> deferredSkeletal = Shader::get(DEFERRED_SKELETAL_PRE_SHADER_ALIAS);

        return !(shader != deferredStatic && shader != deferredVertex && shader != deferredSkeletal);
    };

    // vvv Pre Render System vvv //

    void PreRenderSystem::onEnter()
    {
    #if defined OYL_DISTRIBUTION
        listenForEventType(EventType::WindowResized);
    #else
        listenForEventType(EventType::EditorGameViewportResized);
    #endif
        
        listenForEventType(EventType::SceneChanged);
    }

    void PreRenderSystem::onUpdate()
    {
        using component::Renderable;

        registry->view<Renderable>().each([&](auto entity, Renderable& renderable)
        {
            if (!renderable.material || renderable.material->overrideShader)
                return;
            
            Ref<Shader> shader = nullptr;
            if (registry->has<component::SkeletonAnimatable>(entity))
            {
                if (renderable.material->deferred)
                    shader = Shader::get(DEFERRED_SKELETAL_PRE_SHADER_ALIAS);
                else
                    shader = Shader::get(FORWARD_SKELETAL_SHADER_ALIAS);
            }
            else
            {
                if (renderable.material->deferred)
                    shader = Shader::get(DEFERRED_STATIC_PRE_SHADER_ALIAS);
                else
                    shader = Shader::get(FORWARD_STATIC_SHADER_ALIAS);
            }
            if (renderable.material->shader != shader)
                renderable.material->shader = shader;
        });
        
        // We sort our mesh renderers based on material properties
        registry->sort<Renderable>(
            [](const Renderable& lhs, const Renderable& rhs)
            {
                if (!isRenderableValid(lhs))
                    return false;
                if (!isRenderableValid(rhs))
                    return true;
                if (!_isDeferred(lhs.material->shader) && _isDeferred(rhs.material->shader))
                    return false;
                if (_isDeferred(lhs.material->shader) && !_isDeferred(rhs.material->shader))
                    return true;
                if (lhs.material->shader != rhs.material->shader)
                    return lhs.material->shader < rhs.material->shader;
                if (lhs.material->albedoMap != rhs.material->albedoMap)
                    return lhs.material->albedoMap < rhs.material->albedoMap;
                return lhs.material < rhs.material;
            });
        
        static uint lastNumCameras = 0;

        using component::Camera;
        auto camView = registry->view<Camera>();

        int width = m_windowSize.x;
        if (camView.size() > 1) width /= 2;

        int height = m_windowSize.y;
        if (camView.size() > 2) height /= 2;

        width = glm::max(width, 1);
        height = glm::max(height, 1);

        camView.each([&](Camera& camera)
        {
            if (!camera.m_mainFrameBuffer)
            {
                camera.m_mainFrameBuffer = FrameBuffer::create(1);
                camera.m_mainFrameBuffer->initDepthTexture(1, 1);
                camera.m_mainFrameBuffer->initColorTexture(0, 1, 1,
                                                           TextureFormat::RGBF16,
                                                           TextureFilter::Linear,
                                                           TextureWrap::ClampToEdge);
            }

            if (!camera.m_deferredFrameBuffer)
            {
                // Create deferred buffer with a buffer for
                // position, albedo/specular, normals, emission/glossiness, light space position
                camera.m_deferredFrameBuffer = FrameBuffer::create(5);
                camera.m_deferredFrameBuffer->initDepthTexture(1, 1);

                // Position buffer
                camera.m_deferredFrameBuffer->initColorTexture(0, 1, 1,
                                                               TextureFormat::RGBF16,
                                                               TextureFilter::Linear,
                                                               TextureWrap::ClampToEdge);
                // Albedo/Specular buffer
                camera.m_deferredFrameBuffer->initColorTexture(1, 1, 1,
                                                               TextureFormat::RGBAF16,
                                                               TextureFilter::Linear,
                                                               TextureWrap::ClampToEdge);
                // Normal buffer
                camera.m_deferredFrameBuffer->initColorTexture(2, 1, 1,
                                                               TextureFormat::RGBF16,
                                                               TextureFilter::Linear,
                                                               TextureWrap::ClampToEdge);
                // Emission/Glossiness buffer
                camera.m_deferredFrameBuffer->initColorTexture(3, 1, 1,
                                                               TextureFormat::RGBAF16,
                                                               TextureFilter::Linear,
                                                               TextureWrap::ClampToEdge);
                // Light Space Position buffer
                camera.m_deferredFrameBuffer->initColorTexture(4, 1, 1,
                                                               TextureFormat::RGBAF16,
                                                               TextureFilter::Linear,
                                                               TextureWrap::ClampToEdge);
            }
            
            if (m_camerasNeedUpdate || lastNumCameras != camView.size())
            {
                camera.m_mainFrameBuffer->updateViewport(width, height);
                camera.m_deferredFrameBuffer->updateViewport(width, height);
            
                camera.aspect((float) width / (float) height);
            }

            RenderCommand::setDepthDraw(true);
            
            camera.m_deferredFrameBuffer->clear();
            
            camera.m_mainFrameBuffer->clear();
        });

        using component::BoneTarget;
        registry->view<BoneTarget>().each([&](BoneTarget& boneTarget)
        {
            boneTarget.forceUpdateTransform();
        });
    }

    bool PreRenderSystem::onEvent(const Event& event)
    {
        switch (event.type)
        {
            case EventType::WindowResized:
            case EventType::EditorGameViewportResized:
            {
                auto e       = event_cast<WindowResizedEvent>(event);
                m_windowSize = { e.width, e.height };

                m_camerasNeedUpdate = true;
                break;
            }
            case EventType::SceneChanged:
            {
                m_camerasNeedUpdate = true;
                break;
            }
        }
        return false;
    }

    // ^^^ Pre Render System ^^^ //

    // vvv Forward Render System vvv //

    void ForwardRenderSystem::onEnter()
    {
        listenForEventType(EventType::WindowResized);
        listenForEventType(EventType::SceneChanged);
    }

    void ForwardRenderSystem::onUpdate()
    {
        using component::Transform;
        using component::Renderable;
        using component::Camera;
        using component::PointLight;
        using component::DirectionalLight;
        using component::SpotLight;
        using component::BoneTarget;

        RenderCommand::setAlphaBlend(false);
        RenderCommand::setDepthDraw(true);
        
        Ref<Material> boundMaterial;

        auto camView = registry->view<Transform, Camera>();

        for (auto camera : camView)
        {
            Camera& pc = camView.get<Camera>(camera);

            RenderCommand::setDrawRect(0, 0, pc.m_mainFrameBuffer->getColorWidth(), pc.m_mainFrameBuffer->getColorHeight());

            pc.m_mainFrameBuffer->bind();

            bool doCulling = true;

            auto view = registry->view<Renderable>();
            for (auto entity : view)
            {
                Renderable& mr = view.get(entity);

                if (!isRenderableValid(mr))
                    break;
                if (_isDeferred(mr.material->shader))
                    continue;

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
                        boundMaterial->setUniform1f(pointLightName + ".intensity",
                                                    pointLightProps.intensity);

                        count++;
                        if (count >= 16)
                            break;
                    }

                    boundMaterial->setUniform1i("u_numPointLights", count);

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
                        boundMaterial->setUniform1f(dirLightName + ".intensity",
                                                    dirLightProps.intensity);

                        if (dirLightProps.castShadows && shadowIndex < 3)
                        {
                            boundMaterial->setUniformMat4("u_lightSpaceMatrix", dirLightProps.m_lightSpaceMatrix);
                            
                            std::string shadowName = "u_shadow[" + std::to_string(shadowIndex) + "]";
                            boundMaterial->setUniform1i(shadowName + ".type", 2);
                            boundMaterial->setUniform1i(shadowName + ".map", 5 + shadowIndex);
                            glm::vec2 biasMinMax = { dirLightProps.biasMin, dirLightProps.biasMax };
                            boundMaterial->setUniform2f(shadowName + ".biasMinMax", biasMinMax);
                            dirLightProps.m_frameBuffer->bindDepthAttachment(5 + shadowIndex);

                            shadowIndex++;
                        }

                        count++;
                        if (count >= 16)
                            break;
                    }

                    boundMaterial->setUniform1i("u_numDirLights", count);

                    boundMaterial->setUniform1i("u_numShadowMaps", shadowIndex);
                    
                    // TEMPORARY:
                    boundMaterial->bind();
                    boundMaterial->applyUniforms();
                }

                auto&     transformComponent = registry->get<Transform>(entity);
                glm::mat4 transform          = transformComponent.getMatrixGlobal();

                glm::bvec3 mirror = transformComponent.getMirrorGlobal();
                if (!(mirror.x ^ mirror.y ^ mirror.z) != doCulling)
                {
                    doCulling ^= 1;
                    RenderCommand::setBackfaceCulling(doCulling);
                }

                if (registry->has<component::VertexAnimatable>(entity))
                {
                    auto& anim = registry->get<component::VertexAnimatable>(entity);
                    if (anim.getVertexArray())
                    {
                        anim.m_vao->bind();

                        boundMaterial->shader->bind();
                        boundMaterial->shader->setUniform1f("lerpT_curr", glm::mod(anim.m_currentElapsed, 1.0f));
                        boundMaterial->shader->setUniform1f("lerpT_trans", glm::mod(anim.m_transitionElapsed, 1.0f));

                        //Renderer::submit(boundMaterial, anim.m_vao, mr.mesh->getNumVertices(), transform);
                    }
                }
                else if (auto pSa = registry->try_get<component::SkeletonAnimatable>(entity); pSa) 
                {
                    skeletonAnimate(entity, mr, *pSa);
                }

                Renderer::submit(mr.model, boundMaterial, transform);
            }
        }
    }

    // ^^^ Forward Render System ^^^ //

    // vvv Deferred Render System vvv //

    void DeferredRenderSystem::onEnter()
    {
        m_deferredPostShader = Shader::create(
            {
                { Shader::Vertex, ENGINE_RES + DEFERRED_POST_SHADER_VERTEX_PATH },
                { Shader::Pixel, ENGINE_RES + DEFERRED_POST_SHADER_FRAGMENT_PATH }
            });

        float vertices[] = {
            -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
            1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
            1.0f, 1.0f, 0.0f, 1.0f, 1.0f,
            -1.0f, 1.0f, 0.0f, 0.0f, 1.0f
        };

        u32 indices[] = {
            0, 1, 2,
            2, 3, 0
        };

        m_fullscreenQuad = VertexArray::create();
        
        Ref<VertexBuffer> vbo = VertexBuffer::create(vertices, sizeof(vertices));
        vbo->setLayout({
            { DataType::Float3, "in_position" },
            { DataType::Float2, "in_texCoord" }
        });
        Ref<IndexBuffer> ebo = IndexBuffer::create(indices, 6);
        m_fullscreenQuad->addVertexBuffer(vbo);
        m_fullscreenQuad->addIndexBuffer(ebo);
    }
    
    void DeferredRenderSystem::onUpdate()
    {
        using component::Transform;
        using component::Renderable;
        using component::Camera;
        
        Ref<Material> boundMaterial;

        auto camView = registry->view<Transform, Camera>();

        for (auto camera : camView)
        {
            Camera& pc = camView.get<Camera>(camera);

            RenderCommand::setDrawRect(0, 0, pc.m_deferredFrameBuffer->getColorWidth(), pc.m_deferredFrameBuffer->getColorHeight());
            RenderCommand::setAlphaBlend(false);
            RenderCommand::setDepthDraw(true);
            
            pc.m_deferredFrameBuffer->bind();

            bool doCulling = true;

            glm::mat3 viewNormal = glm::mat3(inverse(transpose(pc.viewMatrix())));

            auto view = registry->view<Renderable>();
            for (auto entity : view)
            {
                Renderable& mr = view.get(entity);

                if (!isRenderableValid(mr) || !_isDeferred(mr.material->shader))
                    break;

                if (!(mr.cullingMask & pc.cullingMask))
                    continue;

                if (mr.material != boundMaterial)
                {
                    boundMaterial = mr.material;

                    boundMaterial->clearUniforms();

                    boundMaterial->bind();

                    boundMaterial->setUniformMat4("u_view", pc.viewMatrix());
                    boundMaterial->setUniformMat4("u_viewProjection", pc.viewProjectionMatrix());
                    boundMaterial->setUniformMat3("u_viewNormal", viewNormal);

                    auto dirLightView = registry->view<component::DirectionalLight>();
                    for (auto e : dirLightView)
                    {
                        auto& light = dirLightView.get(e);

                        if (light.castShadows)
                        {
                            boundMaterial->setUniformMat4("u_lightSpaceMatrix", light.m_lightSpaceMatrix);
                            break;
                        }
                    }

                    boundMaterial->applyUniforms();
                }

                auto&     transformComponent = registry->get<Transform>(entity);
                glm::mat4 transform          = transformComponent.getMatrixGlobal();

                glm::bvec3 mirror = transformComponent.getMirrorGlobal();
                if (!(mirror.x ^ mirror.y ^ mirror.z) != doCulling)
                {
                    doCulling ^= 1;
                    RenderCommand::setBackfaceCulling(doCulling);
                }

                //if (registry->has<component::VertexAnimatable>(entity))
                //{
                //    auto& anim = registry->get<component::VertexAnimatable>(entity);
                //    if (anim.getVertexArray())
                //    {
                //        anim.m_vao->bind();

                //        boundMaterial->shader->bind();
                //        boundMaterial->shader->setUniform1f("lerpT_curr", glm::mod(anim.m_currentElapsed, 1.0f));
                //        boundMaterial->shader->setUniform1f("lerpT_trans", glm::mod(anim.m_transitionElapsed, 1.0f));

                //        //Renderer::submit(boundMaterial, anim.m_vao, mr.mesh->getNumVertices(), transform);
                //    }
                //}
                /*else */if (auto pSa = registry->try_get<component::SkeletonAnimatable>(entity); pSa) 
                {
                    skeletonAnimate(entity, mr, *pSa);
                }

                Renderer::submit(mr.model, boundMaterial, transform);
            }

            int shadowIndex = 0;

            using component::PointLight;
            using component::DirectionalLight;

            m_deferredPostShader->bind();
            
            auto pointLightView = registry->view<PointLight>();
            int  count          = 0;
            for (auto light : pointLightView)
            {
                PointLight& pointLightProps = pointLightView.get(light);
                auto        lightTransform  = registry->get<Transform>(light);

                std::string pointLightName = "u_pointLight[" + std::to_string(count) + "]";

                m_deferredPostShader->setUniform3f(pointLightName + ".position",
                                                   pc.viewMatrix() * glm::vec4(lightTransform.getPositionGlobal(), 1.0f));
                m_deferredPostShader->setUniform3f(pointLightName + ".ambient",
                                                   pointLightProps.ambient);
                m_deferredPostShader->setUniform3f(pointLightName + ".diffuse",
                                                   pointLightProps.diffuse);
                m_deferredPostShader->setUniform3f(pointLightName + ".specular",
                                                   pointLightProps.specular);
                m_deferredPostShader->setUniform1f(pointLightName + ".range",
                                                   pointLightProps.range);
                m_deferredPostShader->setUniform1f(pointLightName + ".intensity",
                                                   pointLightProps.intensity);

                count++;
                if (count >= 16)
                    break;
            }

            m_deferredPostShader->setUniform1i("u_numPointLights", count);
            
            auto dirLightView = registry->view<DirectionalLight>();
            count             = 0;
            for (auto light : dirLightView)
            {
                auto& dirLightProps  = dirLightView.get(light);
                auto& lightTransform = registry->get<Transform>(light);

                std::string dirLightName = "u_dirLight[" + std::to_string(count) + "]";

                m_deferredPostShader->setUniform3f(dirLightName + ".direction",
                                                   viewNormal * lightTransform.getForwardGlobal());
                m_deferredPostShader->setUniform3f(dirLightName + ".ambient",
                                                   dirLightProps.ambient);
                m_deferredPostShader->setUniform3f(dirLightName + ".diffuse",
                                                   dirLightProps.diffuse);
                m_deferredPostShader->setUniform3f(dirLightName + ".specular",
                                                   dirLightProps.specular);
                m_deferredPostShader->setUniform1f(dirLightName + ".intensity",
                                                   dirLightProps.intensity);

                if (dirLightProps.castShadows && shadowIndex < 3)
                {
                    //m_deferredPostShader->setUniformMat4("u_lightSpaceMatrix", dirLightProps.m_lightSpaceMatrix);

                    std::string shadowName = "u_shadow[" + std::to_string(shadowIndex) + "]";
                    m_deferredPostShader->setUniform1i(shadowName + ".type", 2);
                    m_deferredPostShader->setUniform1i(shadowName + ".map", 5 + shadowIndex);
                    glm::vec2 biasMinMax = { dirLightProps.biasMin, dirLightProps.biasMax };
                    m_deferredPostShader->setUniform2f(shadowName + ".biasMinMax", biasMinMax);
                    dirLightProps.m_frameBuffer->bindDepthAttachment(5 + shadowIndex);

                    shadowIndex++;
                }

                count++;
                if (count >= 16)
                    break;
            }

            m_deferredPostShader->setUniform1i("u_numDirLights", count);

            m_deferredPostShader->setUniform1i("u_numShadowMaps", shadowIndex);
            
            pc.m_deferredFrameBuffer->bind(FrameBufferContext::Read);
            pc.m_mainFrameBuffer->bind(FrameBufferContext::Write);

            for (uint i = 0; i < 5; i++)
                pc.m_deferredFrameBuffer->bindColorAttachment(i, i);
            
            RenderCommand::setDepthDraw(false);
            RenderCommand::setAlphaBlend(false);
            Renderer::submit(m_deferredPostShader, m_fullscreenQuad, glm::mat4(1.0f));

            pc.m_deferredFrameBuffer->blit(pc.m_mainFrameBuffer);
        }
    }

    // ^^^ Deferred Render System ^^^ //

    // vvv Gui Render System vvv //

    void GuiRenderSystem::onEnter()
    {
        listenForEventType(EventType::WindowResized);

        m_shader = Shader::create(
            {
                { Shader::Compound, ENGINE_RES + "shaders/ScreenSpace/GUI_Quad.oylshader" },
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
        RenderCommand::setAlphaBlend(true);

        auto camView = registry->view<Camera>();

        glm::vec2 lastLowerClipping = glm::vec2(0.0f);
        glm::vec2 lastUpperClipping = glm::vec2(0.0f);

        for (auto camera : camView)
        {
            auto& pc = camView.get(camera);

			RenderCommand::setDrawRect(0, 0, pc.m_mainFrameBuffer->getColorWidth(), pc.m_mainFrameBuffer->getColorHeight());
        	
            pc.m_mainFrameBuffer->bind();

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

                Renderer::submit(m_shader, m_vao, pc.orthoMatrix() * model);
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
        m_skyboxShader = Shader::create(
            {
                { Shader::Compound, ENGINE_RES + SKYBOX_SHADER_PATH }
            }
        );

        m_hdrShader = Shader::create(
            {
                { Shader::Vertex,   ENGINE_RES + HDR_SHADER_VERTEX_PATH },
                { Shader::Fragment, ENGINE_RES + HDR_SHADER_FRAGMENT_PATH }
            }
        );

        m_intermediateFrameBuffer = FrameBuffer::create(1);
        m_intermediateFrameBuffer->initColorTexture(0, 1, 1, 
                                                    TextureFormat::RGBF16, 
                                                    TextureFilter::Nearest, 
                                                    TextureWrap::ClampToBorder);

        float vertices[] = {
            -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
            1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
            1.0f, 1.0f, 0.0f, 1.0f, 1.0f,
            -1.0f, 1.0f, 0.0f, 0.0f, 1.0f
        };

        u32 indices[] = {
            0, 1, 2,
            2, 3, 0
        };

        m_fullscreenQuad = VertexArray::create();

        Ref<VertexBuffer> vbo = VertexBuffer::create(vertices, sizeof(vertices));
        vbo->setLayout({
            { DataType::Float3, "in_position" },
            { DataType::Float2, "in_texCoord" }
        });
        Ref<IndexBuffer> ebo = IndexBuffer::create(indices, 6);
        m_fullscreenQuad->addVertexBuffer(vbo);
        m_fullscreenQuad->addIndexBuffer(ebo);
    }

    void PostRenderSystem::onUpdate()
    {
        using component::Camera;
        auto camView = registry->view<Camera>();

        camView.each([&](Camera& camera)
        {
            camera.m_mainFrameBuffer->bind();
            RenderCommand::setDrawRect(0, 0, camera.m_mainFrameBuffer->getColorWidth(), camera.m_mainFrameBuffer->getColorHeight());
            RenderCommand::setBackfaceCulling(false);
            RenderCommand::setAlphaBlend(false);

            RenderCommand::setDepthDraw(true);
            this->skyboxPass(camera);
            RenderCommand::setDepthDraw(false);

            if (camera.doHDR)
                this->hdrPass(camera);
        });
    }

    void PostRenderSystem::skyboxPass(const component::Camera& camera)
    {
        static const auto& skybox = TextureCubeMap::get(DEFAULT_SKYBOX_ALIAS);
        static const auto& mesh = Model::get(CUBE_MODEL_ALIAS)->getMeshes()[0];

        glm::mat4 viewProj = camera.projectionMatrix();
        viewProj *= glm::mat4(glm::mat3(camera.viewMatrix()));

        m_skyboxShader->bind();
        m_skyboxShader->setUniformMat4("u_viewProjection", viewProj);

        if (camera.skybox)
            camera.skybox->bind(0);
        else
            skybox->bind(0);

        camera.m_mainFrameBuffer->bind();

        RenderCommand::drawArrays(mesh.getVertexArray(), mesh.getNumVertices());
    }

    static glm::mat4 brightnessMatrix(float a_brightness)
    {
        return {
            1.0f, 0.0f, 0.0f, 0.0f,
            0.0f, 1.0f, 0.0f, 0.0f,
            0.0f, 0.0f, 1.0f, 0.0f,
            a_brightness, a_brightness, a_brightness, 1.0f
        };
    }
    
    static glm::mat4 contrastMatrix(float a_contrast)
    {
        float t = (1.0f - a_contrast) / 2.0f;

        return {
            a_contrast, 0.0f,       0.0f,       0.0f,
            0.0f,       a_contrast, 0.0f,       0.0f,
            0.0f,       0.0f,       a_contrast, 0.0f,
            t,          t,          t,          1.0f
        };
    }
    
    static glm::mat4 saturationMatrix(float a_saturation)
    {
        glm::vec3 luminance = { 0.3086f, 0.6094f, 0.0820f };

        float oneMinusSat = 1.0 - a_saturation;

        glm::vec3 red = glm::vec3(luminance.x * oneMinusSat);
        red += glm::vec3(a_saturation, 0, 0);

        glm::vec3 green = glm::vec3(luminance.y * oneMinusSat);
        green += glm::vec3(0, a_saturation, 0);

        glm::vec3 blue = glm::vec3(luminance.z * oneMinusSat);
        blue += glm::vec3(0, 0, a_saturation);

        glm::mat4 ret(1.0f);
        ret[0].rgb = red;
        ret[1].rgb = green;
        ret[2].rgb = blue;

        return ret;
    }

    void PostRenderSystem::hdrPass(const component::Camera& camera)
    {
        if (m_intermediateFrameBuffer->getColorWidth() != camera.m_mainFrameBuffer->getColorWidth() ||
            m_intermediateFrameBuffer->getColorHeight() != camera.m_mainFrameBuffer->getColorHeight())
        {
            m_intermediateFrameBuffer->updateViewport(camera.m_mainFrameBuffer->getColorWidth(), 
                                                      camera.m_mainFrameBuffer->getColorHeight());
        }

        m_hdrShader->bind();
        m_hdrShader->setUniform1f(0, camera.exposure);
        m_hdrShader->setUniformMat4(1, brightnessMatrix(camera.brightness));
        m_hdrShader->setUniformMat4(2, contrastMatrix(camera.contrast));
        m_hdrShader->setUniformMat4(3, saturationMatrix(camera.saturation));

        camera.m_mainFrameBuffer->bind(FrameBufferContext::Read);
        camera.m_mainFrameBuffer->bindColorAttachment(0);
        m_intermediateFrameBuffer->bind(FrameBufferContext::Write);

        RenderCommand::drawIndexed(m_fullscreenQuad);

        m_intermediateFrameBuffer->blit(camera.m_mainFrameBuffer);
    }

    void ShadowRenderSystem::onEnter()
    {
        m_shader = Shader::create(ENGINE_RES + "shaders/ShadowMap.oylshader");
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
                dl.m_frameBuffer->initDepthTexture(dl.resolution.x, dl.resolution.y);
            }

            if (dl.m_frameBuffer->getDepthWidth() != dl.resolution.x ||
                dl.m_frameBuffer->getDepthHeight() != dl.resolution.y)
            {
                dl.m_frameBuffer->initDepthTexture(dl.resolution.x, dl.resolution.y);
            }

            dl.m_frameBuffer->bind();
            dl.m_frameBuffer->clear();
            
            if (dl.castShadows)
            {
                if (count >= 3) break;

                RenderCommand::setDrawRect(0, 0, dl.m_frameBuffer->getDepthWidth(), dl.m_frameBuffer->getDepthHeight());

                auto& t = registry->get<Transform>(light);

                glm::mat4 lightProjection = glm::ortho(dl.lowerBounds.x, dl.upperBounds.x,
                                                       dl.lowerBounds.y, dl.upperBounds.y,
                                                       0.0f, dl.clipLength);
                
                glm::mat4 lightView = glm::lookAt(-t.getForwardGlobal() * dl.clipLength * 0.5f + t.getPositionGlobal(),
                                                  t.getPositionGlobal(),
                                                  t.getUpGlobal());

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
                                               TextureFormat::RGBF16,
                                               TextureFilter::Nearest,
                                               TextureWrap::ClampToEdge);

        m_intermediateFrameBuffer = FrameBuffer::create(1);

        m_intermediateFrameBuffer->initColorTexture(0, 1, 1,
                                                    TextureFormat::RGBF16,
                                                    TextureFilter::Nearest,
                                                    TextureWrap::ClampToEdge);

        ViewportHandleChangedEvent hcEvent;
        hcEvent.handle = m_forwardFrameBuffer->getColorHandle(0);
        postEvent(hcEvent);

        m_shader = Shader::create(
            {
                { Shader::Vertex, ENGINE_RES + "shaders/ScreenSpace/FBO_Passthrough.vert" },
                { Shader::Pixel, ENGINE_RES + "shaders/ScreenSpace/FBO_Passthrough.frag" }
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

        RenderCommand::setAlphaBlend(false);
        RenderCommand::setDepthDraw(false);

        registry->view<Camera>().each([&](Camera& pc)
        {
            uint width = pc.m_mainFrameBuffer->getColorWidth(), height = pc.m_mainFrameBuffer->getColorHeight();
            if (m_intermediateFrameBuffer->getColorWidth() != width ||
                m_intermediateFrameBuffer->getColorHeight() != height)
                m_intermediateFrameBuffer->updateViewport(width, height);

            m_intermediateFrameBuffer->clear();
            
            RenderCommand::setDrawRect(0, 0, width, height);

            m_vao->bind();

            Shader* boundShader = nullptr;

            bool needsBlit = false;

            for (auto pass : pc.postProcessingPasses)
            {
                if (!pass.shader) continue;

                if (!needsBlit)
                {
                    m_intermediateFrameBuffer->bind(FrameBufferContext::Write);
                    pc.m_mainFrameBuffer->bind(FrameBufferContext::Read);
                    pc.m_mainFrameBuffer->bindColorAttachment(0, 0);
                    for (int i = 0; i < 5; i++)
                        pc.m_deferredFrameBuffer->bindColorAttachment(i, i + 1);
                    pc.m_mainFrameBuffer->bindDepthAttachment(6);
                }
                else
                {
                    pc.m_mainFrameBuffer->bind(FrameBufferContext::Write);
                    m_intermediateFrameBuffer->bind(FrameBufferContext::Read);
                    m_intermediateFrameBuffer->bindColorAttachment(0);
                    for (int i = 0; i < 5; i++)
                        pc.m_deferredFrameBuffer->bindColorAttachment(i, i + 1);
                    pc.m_mainFrameBuffer->bindDepthAttachment(6);
                }

                needsBlit ^= 1;

                if (boundShader != pass.shader.get())
                {
                    boundShader = pass.shader.get();
                    boundShader->bind();
                    boundShader->setUniform2f(1, 1.0f / glm::vec2(m_windowSize));
                }
                pass.applyUniforms();

                RenderCommand::drawIndexed(m_vao);
            }

            if (needsBlit)
                m_intermediateFrameBuffer->blit(pc.m_mainFrameBuffer);

            RenderCommand::setDrawRect(0, 0, m_windowSize.x, m_windowSize.y);

            m_shader->bind();

            //auto& pc = view.get<Camera>(camera);

            pc.m_mainFrameBuffer->bind(FrameBufferContext::Read);
            pc.m_mainFrameBuffer->bindColorAttachment(0);
            m_forwardFrameBuffer->bind(FrameBufferContext::Write);

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

    #if defined(OYL_DISTRIBUTION)
        m_forwardFrameBuffer->blit();
    #endif
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

    void skeletonAnimate(entt::entity entity, component::Renderable& renderable, component::SkeletonAnimatable& sa)
    {
        static auto fSkeletal = Shader::get("Oyl Forward Skeletal");
        static auto dSkeletal = Shader::get("Oyl Deferred Skeletal");
        bool valid = renderable.model && renderable.material;
        valid &= renderable.material->shader == fSkeletal || renderable.material->shader == dSkeletal;
        if (valid)
        {
            if (auto it = renderable.model->getAnimations().find(sa.animation);
                it != renderable.model->getAnimations().end())
            {
                std::vector<glm::mat4> boneTransforms;
                renderable.model->getBoneTransforms(sa.animation, sa.time, boneTransforms);

                renderable.material->shader->bind();
                for (uint i = 0; i < 64; i++)
                {
                    glm::mat4 uniform = glm::mat4(1.0f);
                    if (i < boneTransforms.size())
                        uniform = boneTransforms[i];

                    renderable.material->shader->setUniformMat4("u_boneTransforms[" + std::to_string(i) + "]", uniform);
                }
            } else
            {
                renderable.material->shader->bind();
                for (uint i = 0; i < 64; i++)
                    renderable.material->shader->setUniformMat4("u_boneTransforms[" + std::to_string(i) + "]", glm::mat4(1.0f));
            }
        }
    }
}
