#include "oylpch.h"

#include "Application.h"

#include "App/Window.h"

#include "Debug/GuiLayer.h"

#include "Animation/AnimationSystems.h"
#include "Rendering/RenderSystems.h"
#include "Debug/EditorSystems.h"

#include "Events/EventDispatcher.h"
#include "Events/EventListener.h"

#include "Graphics/Shader.h"
#include "Graphics/Texture.h"
#include "Graphics/Model.h"

#include "Input/GamepadListener.h"

#include "Scenes/Scene.h"
#include "Scenes/SystemsLayer.h"

#include "Rendering/Renderer.h"

#include "Utils/SceneToFile.h"

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

            #if !defined OYL_DISTRIBUTION
                listenForEventType(EventType::EditorBackButton);
            #endif
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

        Model::init();
        Texture::init();

        //Shader::cache(
        //    {
        //        { Shader::Compound, ENGINE_RES + "shaders/skeletal.oylshader" },
        //    }, "Oyl Skeletal");

        //Shader::cache(
        //    {
        //        { Shader::Vertex, ENGINE_RES + LIGHTING_SHADER_VERTEX_PATH },
        //        { Shader::Pixel, ENGINE_RES + LIGHTING_SHADER_FRAGMENT_PATH },
        //    }, LIGHTING_SHADER_ALIAS);

        //Shader::cache(
        //    {
        //        { Shader::Vertex, ENGINE_RES + "shaders/morphTargetLighting.vert" },
        //        { Shader::Pixel, ENGINE_RES + LIGHTING_SHADER_FRAGMENT_PATH },
        //    }, "animation");
            
        Shader::cache(
            {
                { Shader::Vertex, ENGINE_RES + FORWARD_STATIC_SHADER_VERTEX_PATH },
                { Shader::Pixel, ENGINE_RES + FORWARD_STATIC_SHADER_FRAGMENT_PATH },
            }, FORWARD_STATIC_SHADER_ALIAS);

        Shader::cache(
            {
                { Shader::Vertex, ENGINE_RES + FORWARD_VERTEX_SHADER_VERTEX_PATH },
                { Shader::Pixel, ENGINE_RES +  FORWARD_VERTEX_SHADER_FRAGMENT_PATH },
            }, FORWARD_VERTEX_SHADER_ALIAS);
        
        Shader::cache(
            {
                { Shader::Vertex, ENGINE_RES + FORWARD_SKELETAL_SHADER_VERTEX_PATH },
                { Shader::Pixel, ENGINE_RES + FORWARD_SKELETAL_SHADER_FRAGMENT_PATH },
            }, FORWARD_SKELETAL_SHADER_ALIAS);

        Shader::cache(
            {
                { Shader::Vertex, ENGINE_RES + DEFERRED_STATIC_PRE_SHADER_VERTEX_PATH },
                { Shader::Pixel, ENGINE_RES + DEFERRED_STATIC_PRE_SHADER_FRAGMENT_PATH },
            }, DEFERRED_STATIC_PRE_SHADER_ALIAS);

        Shader::cache(
            {
                { Shader::Vertex, ENGINE_RES + DEFERRED_VERTEX_PRE_SHADER_VERTEX_PATH },
                { Shader::Pixel, ENGINE_RES + DEFERRED_VERTEX_PRE_SHADER_FRAGMENT_PATH },
            }, DEFERRED_VERTEX_PRE_SHADER_ALIAS);

        Shader::cache(
            {
                { Shader::Vertex, ENGINE_RES + DEFERRED_SKELETAL_PRE_SHADER_VERTEX_PATH },
                { Shader::Pixel, ENGINE_RES + DEFERRED_SKELETAL_PRE_SHADER_FRAGMENT_PATH },
            }, DEFERRED_SKELETAL_PRE_SHADER_ALIAS);

        Model::cache(ENGINE_RES + CUBE_MODEL_PATH, CUBE_MODEL_ALIAS);
        Model::cache(ENGINE_RES + MONKEY_MODEL_PATH, MONKEY_MODEL_ALIAS);

        Texture2D::cache(ENGINE_RES + WHITE_TEXTURE_PATH, WHITE_TEXTURE_ALIAS);
        Texture2D::cache(ENGINE_RES + BLACK_TEXTURE_PATH, BLACK_TEXTURE_ALIAS);
        Texture2D::cache(ENGINE_RES + UV_TEXTURE_PATH, UV_TEXTURE_ALIAS);
        Texture2D::cache(ENGINE_RES + DEFAULT_NORMAL_TEXTURE_PATH, DEFAULT_NORMAL_TEXTURE_ALIAS);

        TextureCubeMap::cache(ENGINE_RES + DEFAULT_SKYBOX_PATH, DEFAULT_SKYBOX_ALIAS);

    #if !defined OYL_DISTRIBUTION
        m_editorRenderSystem = internal::EditorRenderSystem::create();
    #endif

        initEventListeners();

        m_skeletalAnimationSystem = internal::SkeletalAnimationSystem::create();
        m_preRenderSystem         = internal::PreRenderSystem::create();
        m_shadowRenderSystem      = internal::ShadowRenderSystem::create();
        m_deferredRenderSystem    = internal::DeferredRenderSystem::create();
        m_forwardRenderSystem     = internal::ForwardRenderSystem::create();
        m_guiRenderSystem         = internal::GuiRenderSystem::create();
        m_postRenderSystem        = internal::UserPostRenderSystem::create();

    #if !defined OYL_DISTRIBUTION
        m_editorRenderSystem->setDispatcher(m_dispatcher);
        m_editorRenderSystem->onEnter();
        m_dispatcher->registerListener(m_editorRenderSystem, -1u);
    #endif

        m_skeletalAnimationSystem->setDispatcher(m_dispatcher);
        m_skeletalAnimationSystem->onEnter();
        m_dispatcher->registerListener(m_skeletalAnimationSystem, -1u);

        m_preRenderSystem->setDispatcher(m_dispatcher);
        m_preRenderSystem->onEnter();
        m_dispatcher->registerListener(m_preRenderSystem, -1u);

        m_shadowRenderSystem->setDispatcher(m_dispatcher);
        m_shadowRenderSystem->onEnter();
        m_dispatcher->registerListener(m_shadowRenderSystem, -1u);

        m_deferredRenderSystem->setDispatcher(m_dispatcher);
        m_deferredRenderSystem->onEnter();
        m_dispatcher->registerListener(m_deferredRenderSystem, -1u);

        m_forwardRenderSystem->setDispatcher(m_dispatcher);
        m_forwardRenderSystem->onEnter();
        m_dispatcher->registerListener(m_forwardRenderSystem, -1u);

        m_guiRenderSystem->setDispatcher(m_dispatcher);
        m_guiRenderSystem->onEnter();
        m_dispatcher->registerListener(m_guiRenderSystem, -1u);

        m_postRenderSystem->setDispatcher(m_dispatcher);
        m_postRenderSystem->onEnter();
        m_dispatcher->registerListener(m_postRenderSystem, -1u);

        WindowResizedEvent wrEvent;
        wrEvent.width = 1280;
        wrEvent.height = 720;
        m_dispatcher->postEvent(wrEvent);

        m_window->setVsync(false);
    }

    Application::~Application()
    {
        Scene::s_current.lock()->onExit();

    #if !defined(OYL_DISTRIBUTION)
        m_guiLayer->onExit();
        m_guiLayer->shutdown();
    #endif

        m_systemsLayer->onExit();
        
        m_forwardRenderSystem->onExit();
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
        #if !defined OYL_DISTRIBUTION
            case EventType::EditorBackButton:
            {
                pushScene(m_guiLayer->m_originalScene, false);
                
                for (const auto& [name, registry] : m_guiLayer->m_registryRestores)
                    *m_registeredScenes[name]->m_registry = registry.clone();
                break;
            }
        #endif
        }

        handled |= !m_doUpdate;

        return handled;
    }

    void Application::changeScene(const std::string& name)
    {
        if (name != m_currentScene && m_registeredScenes.find(name) == m_registeredScenes.end())
            return;

        m_nextScene = name;
    }

    void Application::pushScene(const std::string& scene, bool callOnEnter)
    {    
        OYL_ASSERT(m_registeredScenes.find(scene) != m_registeredScenes.end(), "Pushed scene must exist!");

        if (callOnEnter && !m_currentScene.empty())
        {
            m_registeredScenes[m_currentScene]->Scene::onExit();
            m_registeredScenes[m_currentScene]->onExit();
        }

        m_currentScene = scene;

        Scene::s_current = m_registeredScenes.at(scene);

        SceneChangedEvent sceneChangedEvent;
        sceneChangedEvent.name = m_currentScene.c_str();
        m_dispatcher->postEvent(sceneChangedEvent);

        auto& pScene = m_registeredScenes[m_currentScene];

    #if !defined OYL_DISTRIBUTION
        m_editorRenderSystem->setRegistry(pScene->m_registry);
    #endif
        
        m_skeletalAnimationSystem->setRegistry(pScene->m_registry);
        m_preRenderSystem->setRegistry(pScene->m_registry);
        m_shadowRenderSystem->setRegistry(pScene->m_registry);
        m_deferredRenderSystem->setRegistry(pScene->m_registry);
        m_forwardRenderSystem->setRegistry(pScene->m_registry);
        m_guiRenderSystem->setRegistry(pScene->m_registry);
        m_postRenderSystem->setRegistry(pScene->m_registry);

        if (m_systemsLayer)
            m_systemsLayer->onExit();
        
        m_systemsLayer = internal::SystemsLayer::create();
        
        m_systemsLayer->setDispatcher(m_dispatcher);
        m_systemsLayer->setRegistry(pScene->m_registry);

        m_systemsLayer->onEnter();

        m_dispatcher->registerListener(m_systemsLayer);
        
    #if !defined(OYL_DISTRIBUTION)
        if (!m_guiLayer)
        {
            m_guiLayer = internal::GuiLayer::create();
            m_guiLayer->init();

            m_guiLayer->setDispatcher(m_dispatcher);
            m_guiLayer->setRegistry(pScene->m_registry);
            m_guiLayer->onEnter();    
        }
        else
        {
            m_guiLayer->setDispatcher(m_dispatcher);
            m_guiLayer->setRegistry(pScene->m_registry);

            if (callOnEnter)
            {
                m_guiLayer->onExit();
                m_guiLayer->onEnter();
            }
        }
        m_dispatcher->registerListener(m_guiLayer, -2u);

        m_guiRenderSystem->setRegistry(pScene->m_registry);
    #endif

        m_dispatcher->registerListener(pScene);
        pScene->setDispatcher(m_dispatcher);

        if (callOnEnter)
        {
            pScene->Scene::onEnter();
            pScene->onEnter();
        
            internal::registryFromSceneFile(*pScene->m_registry, m_currentScene);
        }
    }

    void Application::run()
    {
        OYL_ASSERT(!m_nextScene.empty());

        //m_registeredScenes[m_nextScene]->m_registry->reset();
        pushScene(m_nextScene, false);
        m_nextScene.clear();
        
        while (m_running)
        {
            Time::update();
            
            m_dispatcher->dispatchEvents();

            if (m_doUpdate)
            {
                if (!m_nextScene.empty() && m_nextScene != m_currentScene)
                {
                    m_registeredScenes[m_currentScene]->m_registry->reset();
                    pushScene(m_nextScene);
                    m_nextScene.clear();
                }
                
            #if !defined(OYL_DISTRIBUTION)
                if (m_guiLayer->doGameUpdate())
                {
            #endif
                    m_systemsLayer->onUpdateSystems();
                    m_systemsLayer->onUpdate();

                    m_registeredScenes[m_currentScene]->onUpdate();

                    m_skeletalAnimationSystem->onUpdate();
            #if !defined OYL_DISTRIBUTION
                }
                m_guiLayer->onUpdateSystems();
                m_guiLayer->onUpdate();
            #endif
                
                RenderCommand::setClearColor(0.1f, 0.1f, 0.1f, 1.0f);
                RenderCommand::clear();

                Renderer::beginScene();
                
                m_preRenderSystem->onUpdate();
                m_shadowRenderSystem->onUpdate();
                m_deferredRenderSystem->onUpdate();
                m_forwardRenderSystem->onUpdate();
                m_guiRenderSystem->onUpdate();
                m_postRenderSystem->onUpdate();
            #if !defined OYL_DISTRIBUTION
                m_editorRenderSystem->onUpdate();
            #endif

                Renderer::endScene();                
            }

        #if !defined(OYL_DISTRIBUTION)
            m_guiLayer->begin();

            m_preRenderSystem->onGuiRender();
            m_shadowRenderSystem->onGuiRender();
            m_deferredRenderSystem->onGuiRender();
            m_forwardRenderSystem->onGuiRender();
            m_guiRenderSystem->onGuiRender();
            m_postRenderSystem->onGuiRender();
            
            m_skeletalAnimationSystem->onGuiRender();

            m_guiLayer->onGuiRenderSystems();
            m_editorRenderSystem->onGuiRender();
            m_guiLayer->onGuiRender();

            if (m_guiLayer->doGameUpdate())
                m_registeredScenes[m_currentScene]->onGuiRender();

            m_guiLayer->end();
        #else
            // TODO: Turn into Renderer Call
            //m_mainBuffer->moveToBackBuffer(m_window->getWidth(), m_window->getHeight());
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
        m_dispatcher->registerListener(m_appListener, -1u);
        m_appListener->setDispatcher(m_dispatcher);

        m_vibrationListener = internal::GamepadListener::create();
        m_dispatcher->registerListener(m_vibrationListener, -1u);
        m_vibrationListener->setDispatcher(m_dispatcher);
    }
}
