#include "oylpch.h"

#include "Application.h"

#include "App/Window.h"

#include "Debug/GuiLayer.h"

#include "ECS/SystemImpl.h"

#include "Events/EventDispatcher.h"
#include "Events/EventListener.h"

#include "Graphics/Material.h"
#include "Graphics/Shader.h"
#include "Graphics/Texture.h"

#include "Input/GamepadListener.h"

#include "Scenes/Scene.h"
#include "Scenes/SystemsLayer.h"

#include "Rendering/Renderer.h"

#include "Utils/SceneToFile.h"

#include <imgui.h>

namespace oyl
{
    namespace internal
    {
        class ApplicationListener : public EventListener
        {
            friend Application;
        public:
            ApplicationListener()
            {
                listenForEventCategory(EventCategory::Window);
                listenForEventCategory(EventCategory::Keyboard);
                listenForEventCategory(EventCategory::Mouse);
                listenForEventCategory(EventCategory::Cursor);
            }

        private:
            virtual bool onEvent(const Event& event) override
            {
                return app->onEvent(event);
            }

            Application* app = nullptr;
        };
    }

    Application* Application::s_instance = nullptr;

    Application::Application()
    {
        OYL_ASSERT(!s_instance, "Application already exists!");
        s_instance = this;

        Time::init();
        
        Log::init();

        m_window = Window::create();

        Mesh::init();
        Texture::init();

        Shader::cache(
            {
                { Shader::Vertex, ENGINE_RES + LIGHTING_SHADER_VERTEX_PATH },
                { Shader::Pixel, ENGINE_RES + LIGHTING_SHADER_FRAGMENT_PATH },
            }, LIGHTING_SHADER_ALIAS);

        Shader::cache(
            {
                { Shader::Vertex, ENGINE_RES + "shaders/morphTargetLighting.vert" },
                { Shader::Pixel, ENGINE_RES + LIGHTING_SHADER_FRAGMENT_PATH },
            }, "animation");

        Shader::cache(
            {
                { Shader::Vertex, ENGINE_RES + "shaders/texturedQuad.vert" },
                { Shader::Pixel, ENGINE_RES + "shaders/texturedQuad.frag" }
            }, "texturedQuad");

        Shader::cache(
            {
                { Shader::Vertex, ENGINE_RES + SKYBOX_SHADER_VERTEX_PATH },
                { Shader::Pixel, ENGINE_RES + SKYBOX_SHADER_FRAGMENT_PATH }
            }, SKYBOX_SHADER_ALIAS);

        Mesh::cache(ENGINE_RES + CUBE_MESH_PATH, CUBE_MESH_ALIAS);
        Mesh::cache(ENGINE_RES + MONKEY_MESH_PATH, MONKEY_MESH_ALIAS);

        Texture2D::cache(ENGINE_RES + WHITE_TEXTURE_PATH, WHITE_TEXTURE_ALIAS);
        Texture2D::cache(ENGINE_RES + BLACK_TEXTURE_PATH, BLACK_TEXTURE_ALIAS);
        Texture2D::cache(ENGINE_RES + UV_TEXTURE_PATH, UV_TEXTURE_ALIAS);
        Texture2D::cache(ENGINE_RES + DEFAULT_NORMAL_TEXTURE_PATH, DEFAULT_NORMAL_TEXTURE_ALIAS);

        TextureCubeMap::cache(ENGINE_RES + DEFAULT_SKYBOX_PATH, DEFAULT_SKYBOX_ALIAS);

		Material::cache(Material::create(), INVALID_ALIAS);

        m_renderSystem    = internal::RenderSystem::create();
        m_guiRenderSystem = internal::GuiRenderSystem::create();

        initEventListeners();

        m_mainBuffer = FrameBuffer::create(1);
        m_mainBuffer->initDepthTexture(m_window->getWidth(), m_window->getHeight());

        m_mainBuffer->initColorTexture(0, m_window->getWidth(), m_window->getHeight(),
                                       TextureFormat::RGBA8,
                                       TextureFilter::Nearest,
                                       TextureWrap::ClampToEdge);

        ViewportHandleChangedEvent hcEvent;
        hcEvent.handle = m_mainBuffer->getColorHandle(0);

        m_dispatcher->postEvent(hcEvent);

        WindowResizedEvent wrEvent;
        wrEvent.width = 1280;
        wrEvent.height = 720;
        m_dispatcher->postEvent(wrEvent);

        m_window->setVsync(false);
    }

    Application::~Application()
    {
        m_currentScene->onExit();

    #if !defined(OYL_DISTRIBUTION)
        m_guiLayer->onExit();
        m_guiLayer->shutdown();
    #endif

        m_systemsLayer->onExit();
        
        m_renderSystem->onExit();
    }

    bool Application::onEvent(const Event& event)
    {
        bool handled = false;

        switch (event.type)
        {
            case EventType::WindowClosed:
            {
                m_running = false;
                handled   = true;
                break;
            }
            case EventType::WindowResized:
            {
                auto e = event_cast<WindowResizedEvent>(event);
                m_window->updateViewport(e.width, e.height);
                m_mainBuffer->updateViewport(e.width, e.height);
                break;
            }
            case EventType::WindowFocused:
            {
                auto e = event_cast<WindowFocusedEvent>(event);
            #if defined(OYL_DISTRIBUTION)
                m_doUpdate = e.focused;
            #endif
                break;
            }
            case EventType::CursorStateRequest:
            {
                auto e = event_cast<CursorStateRequestEvent>(event);
                if (m_window->getCursorState() != e.state)
                    m_window->setCursorState(e.state);
                break;
            }
        }

        handled |= !m_doUpdate;

        return handled;
    }

    void Application::pushScene(Ref<Scene> scene)
    {        
        if (m_currentScene)
        {
            m_currentScene->onExit();
            m_currentScene = nullptr;

        #if !defined(OYL_DISTRIBUTION)
            m_guiLayer->onExit();
        #endif

            m_systemsLayer->onExit();
            
            m_renderSystem->onExit();
        }

        OYL_ASSERT(scene, "Pushed scene must be initialized!");

        m_currentScene = std::move(scene);

        Scene::s_current = m_currentScene;

        m_renderSystem->setDispatcher(m_dispatcher);
        m_renderSystem->setRegistry(m_currentScene->m_registry);
        m_renderSystem->onEnter();

        m_dispatcher->registerListener(m_renderSystem);

        m_guiRenderSystem->setDispatcher(m_dispatcher);
        m_guiRenderSystem->setRegistry(m_currentScene->m_registry);
        m_guiRenderSystem->onEnter();

        m_dispatcher->registerListener(m_guiRenderSystem);

        if (!m_systemsLayer)
        {
            m_systemsLayer = internal::SystemsLayer::create();
            
            m_systemsLayer->setDispatcher(m_dispatcher);
            m_systemsLayer->setRegistry(m_currentScene->m_registry);
            m_systemsLayer->onEnter();

            m_dispatcher->registerListener(m_systemsLayer);
        }
        else
        {
            m_systemsLayer->setRegistry(m_currentScene->m_registry);
            m_systemsLayer->onEnter();
        }

    #if !defined(OYL_DISTRIBUTION)
        if (!m_guiLayer)
        {
            m_guiLayer = internal::GuiLayer::create();
            m_guiLayer->init();

            m_guiLayer->setDispatcher(m_dispatcher);
            m_guiLayer->setRegistry(m_currentScene->m_registry);
            m_guiLayer->onEnter();
            
            m_dispatcher->registerListener(m_guiLayer);
        }
        else
        {
            m_guiLayer->setRegistry(m_currentScene->m_registry);
            m_guiLayer->onEnter();
        }
    #endif

        m_dispatcher->registerListener(m_currentScene);
        m_currentScene->setDispatcher(m_dispatcher);

        m_currentScene->Scene::onEnter();
        m_currentScene->onEnter();
        
        //internal::loadSceneFromFile(*m_currentScene);
        internal::registryFromSceneFile(*m_currentScene->m_registry, m_currentScene->m_name);
        
    }

    void Application::run()
    {        
        while (m_running)
        {
            Time::update();
            
            if (m_doUpdate)
            {
                m_dispatcher->dispatchEvents();

            #if !defined(OYL_DISTRIBUTION)
                if (m_guiLayer->doGameUpdate())
                {
                    m_systemsLayer->onUpdateSystems();
                    m_systemsLayer->onUpdate();

                    m_currentScene->onUpdate();
                }
                
                m_guiLayer->onUpdateSystems();
                m_guiLayer->onUpdate();
            #else
                m_systemsLayer->onUpdateSystems(timestep);
                m_systemsLayer->onUpdate(timestep);

                m_currentScene->onUpdate(timestep);
            #endif

                RenderCommand::setClearColor(0.1f, 0.1f, 0.1f, 1.0f);
                RenderCommand::clear();

                m_mainBuffer->clear();
                m_mainBuffer->bind();

                Renderer::beginScene();
                
                m_renderSystem->onUpdate();
                m_guiRenderSystem->onUpdate();

                Renderer::endScene();
                m_mainBuffer->unbind();
            }

        #if !defined(OYL_DISTRIBUTION)
            m_guiLayer->begin();

            m_renderSystem->onGuiRender();
            m_guiRenderSystem->onGuiRender();

            m_guiLayer->onGuiRenderSystems();
            m_guiLayer->onGuiRender();

            if (m_guiLayer->doGameUpdate())
                m_currentScene->onGuiRender();

            m_guiLayer->end();
        #else
            // TODO: Turn into Renderer Call
            m_mainBuffer->moveToBackBuffer(m_window->getWidth(), m_window->getHeight());
        #endif

            m_window->onUpdate(m_doUpdate);

            m_vibrationListener->onUpdate();
        }
    }

    void Application::initEventListeners()
    {
        m_dispatcher = Ref<EventDispatcher>::create();

        // TODO: Make Window an EventListener
        m_window->setEventCallback(OYL_CALLBACK_1(EventDispatcher::postEvent, m_dispatcher.get()));

        m_appListener      = Ref<internal::ApplicationListener>::create();
        m_appListener->app = this;
        m_dispatcher->registerListener(m_appListener);
        m_appListener->setDispatcher(m_dispatcher);

        m_vibrationListener = internal::GamepadListener::create();
        m_dispatcher->registerListener(m_vibrationListener);
        m_vibrationListener->setDispatcher(m_dispatcher);
    }
}
