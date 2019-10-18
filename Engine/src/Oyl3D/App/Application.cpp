#include "oylpch.h"

#include "Application.h"

#include "Events/EventListener.h"

#include "System/Platform.h"

#include "Rendering/Renderer.h"

#include <imgui.h>

namespace oyl
{
    namespace _internal
    {
        class ApplicationListener : public EventListener
        {
            friend class oyl::Application;
        private:
            virtual bool onEvent(Ref<Event> event) override
            {
                return app->onEvent(event);
            }

            oyl::Application* app = nullptr;
        };
    }

    Application* Application::s_instance = nullptr;

    Application::Application()
    {
        OYL_ASSERT(!s_instance, "Application already exists!");
        s_instance = this;

        m_camera.setProjection(glm::perspective(glm::radians(60.0f), 16.0f / 9.0f, 0.01f, 1000.0f));
        m_camera.setPosition(glm::vec3(0.0f));
        m_camera.lookAt(glm::vec3(0.0f, 0.0f, -1.0f));
        
        Log::init();

        m_window = Window::create();
        
        m_imguiLayer = Ref<ImGuiLayer>::create();
        m_imguiLayer->onAttach();

        initEventListeners();
        
        m_mainBuffer = oyl::FrameBuffer::create(1);
        m_mainBuffer->initDepthTexture(m_window->getWidth(), m_window->getHeight());

        m_mainBuffer->initColorTexture(0, m_window->getWidth(), m_window->getHeight(),
                                       oyl::RGBA8,
                                       oyl::Nearest,
                                       oyl::Clamp);

        //m_window->setVsync(false);
    }

    Application::~Application()
    {
    }

    bool Application::onEvent(const Ref<Event>& event)
    {
        bool handled = false;

        switch (event->type)
        {
            case TypeWindowClosed:
            {
                m_running = false;
                handled   = true;
                break;
            }
            case TypeWindowResized:
            {
                auto e = (WindowResizedEvent) *event;
                m_window->updateViewport(e.width, e.height);
                m_mainBuffer->updateViewport(e.width, e.height);
                break;
            }
            case TypeWindowFocused:
            {
                auto e = (WindowFocusedEvent) *event;
            #if defined(OYL_DISTRIBUTION)
                m_doUpdate = e.focused;
            #endif
                break;
            }
        }

        handled |= m_doUpdate;

        return handled;
    }

    void Application::pushScene(Ref<Scene> scene)
    {
        if (m_currentScene)
        {
            m_currentScene->onExit();
            m_currentScene = nullptr;
        }

        if (scene)
        {
            m_currentScene = std::move(scene);

            Scene::s_current = m_currentScene;
            
            m_dispatcher.registerListener(m_currentScene);
            m_currentScene->setPostEventCallback(m_dispatcherPostCallback);
            m_currentScene->setRegisterCallback(m_dispatcherRegisterCallback);
            m_currentScene->setUnregisterCallback(m_dispatcherUnregisterCallback);

            // TEMPORARY: Make single function that registers all scene systems
            m_dispatcher.registerListener(m_currentScene->m_renderSystem);
            m_currentScene->m_renderSystem->setPostEventCallback(m_dispatcherPostCallback);
            m_currentScene->m_renderSystem->setRegisterCallback(m_dispatcherRegisterCallback);
            m_currentScene->m_renderSystem->setUnregisterCallback(m_dispatcherUnregisterCallback);

            m_currentScene->onEnter();
        }
    }

    void Application::run()
    {
        while (m_running)
        {
            auto     time = (float) Platform::getTime();
            Timestep realTimestep(time - m_lastFrameTime);
            Timestep timestep(abs(realTimestep) > 1.0f / 30.0f ? 1.0f / 30.0f : realTimestep);
            m_lastFrameTime = time;

            if (m_doUpdate)
            {
                RenderCommand::setClearColor(0.1f, 0.1f, 0.1f, 1.0f);
                RenderCommand::clear();
                m_mainBuffer->clear();

                m_mainBuffer->bind();
                Renderer::beginScene(m_camera);

                m_dispatcher.dispatchEvents();

                m_currentScene->onUpdate(timestep);

                m_currentScene->m_renderSystem->onUpdate(timestep);

                Renderer::endScene();
                m_mainBuffer->unbind();
            }

#if !defined(OYL_DISTRIBUTION)
            m_imguiLayer->begin();

            m_imguiLayer->onGuiRender();
            m_currentScene->onGuiRender();

            m_imguiLayer->end();

#else
            m_mainBuffer->moveToBackBuffer(m_window->getWidth(), m_window->getHeight());
#endif

            m_window->onUpdate(m_doUpdate);

            m_vibrationListener->onUpdate(timestep);
        }
    }
    
    void Application::initEventListeners()
    {
        m_dispatcherPostCallback =
            OYL_CALLBACK_1(EventDispatcher::postEvent, &m_dispatcher);
        m_dispatcherRegisterCallback =
            OYL_CALLBACK_2(EventDispatcher::registerListener, &m_dispatcher);
        m_dispatcherUnregisterCallback =
            OYL_CALLBACK_1(EventDispatcher::unregisterListener, &m_dispatcher);

        m_window->setEventCallback(m_dispatcherPostCallback);

        m_appListener = Ref<_internal::ApplicationListener>::create();

        m_appListener->app = this;
        m_appListener->addToEventMask(TypeWindowClosed);
        m_appListener->addToEventMask(TypeWindowResized);
        m_appListener->addToEventMask(TypeWindowFocused);
        m_appListener->addToCategoryMask(CategoryWindow);

        m_dispatcher.registerListener(m_appListener);

        m_vibrationListener = _internal::GamepadListener::create();
        m_vibrationListener->addToEventMask(TypeGamepadVibration);
        m_vibrationListener->addToCategoryMask(CategoryGamepadVibration);

        m_dispatcher.registerListener(m_vibrationListener);

        m_dispatcher.registerListener(m_imguiLayer);
    }
}
