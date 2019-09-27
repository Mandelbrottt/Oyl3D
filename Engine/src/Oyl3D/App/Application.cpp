#include "oylpch.h"

#include "Application.h"

#include "Events/EventListener.h"

#include "Input/Input.h"

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
        : m_camera(60.0f, 16.0f / 9.0f, 0.01f, 1000.0f)
    {
        OYL_ASSERT(!s_instance, "Application already exists!");
        s_instance = this;

        Log::init();

        m_window = Window::create();
        m_window->setEventCallback(std::bind(&EventDispatcher::postEvent,
                                             &m_dispatcher,
                                             std::placeholders::_1));

        m_appListener      = Ref<_internal::ApplicationListener>::create();
        m_appListener->app = this;
        m_dispatcher.registerListener(m_appListener);

        m_imguiLayer.reset(new ImGuiLayer());
        m_imguiLayer->onAttach();

        m_dispatcher.registerListener(m_imguiLayer);

        m_mainBuffer = oyl::FrameBuffer::create(1);
        m_mainBuffer->initDepthTexture(m_window->getWidth(), m_window->getHeight());

        m_mainBuffer->initColorTexture(0, m_window->getWidth(), m_window->getHeight(),
                                       oyl::RGBA8,
                                       oyl::Nearest,
                                       oyl::Clamp);

        m_window->setVsync(false);
    }

    Application::~Application()
    {
    }

    bool Application::onEvent(Ref<Event> event)
    {
        bool handled = false;

        EventListenerDeprecated dispatcher(*event);

        handled = handled || dispatcher.dispatch<WindowCloseEvent>([&](WindowCloseEvent& e)
        {
            m_running = false;
            return false;
        });
        handled = handled || dispatcher.dispatch<WindowResizeEvent>([&](WindowResizeEvent& e)
        {
            m_window->updateViewport(e.getWidth(), e.getHeight());
            m_mainBuffer->updateViewport(e.getWidth(), e.getHeight());
            return false;
        });
        handled = handled || dispatcher.dispatch<WindowFocusEvent>([&](WindowFocusEvent& e)
        {
        #if defined(OYL_DISTRIBUTION)
            m_doUpdate = e.isFocused();
        #endif
            return !e.isFocused();
        });

        return handled;
    }

    void Application::pushScene(Ref<Scene> scene)
    {
        if (m_currentScene)
        {
            m_currentScene->onExit();
            m_currentScene.reset();
        }

        if (scene)
        {
            m_currentScene = std::move(scene);

            m_dispatcherPostCallback =
                std::bind(&EventDispatcher::postEvent,
                          &m_dispatcher,
                          std::placeholders::_1);
            m_dispatcherRegisterCallback =
                std::bind(&EventDispatcher::registerListener,
                          &m_dispatcher,
                          std::placeholders::_1,
                          std::placeholders::_2);
            m_dispatcherUnregisterCallback =
                std::bind(&EventDispatcher::unregisterListener,
                          &m_dispatcher,
                          std::placeholders::_1);

            m_dispatcher.registerListener(m_currentScene);
            m_currentScene->setPostEventCallback(m_dispatcherPostCallback);
            m_currentScene->setRegisterCallback(m_dispatcherRegisterCallback);
            m_currentScene->setUnregisterCallback(m_dispatcherUnregisterCallback);

            m_currentScene->onEnter();
        }
    }

    void Application::run()
    {
        while (m_running)
        {
            auto     time = (float) Platform::getTime();
            Timestep timestep(time - m_lastFrameTime);
            m_lastFrameTime = time;

            if (m_doUpdate)
            {
                RenderCommand::setClearColor(0.1f, 0.1f, 0.1f, 1.0f);
                RenderCommand::clear();
                m_mainBuffer->clear();

                m_mainBuffer->bind();
                Renderer::beginScene(m_camera);

                m_dispatcher.dispatchEvents();

                m_currentScene->onUpdate(abs(timestep) > 1.0f / 30.0f ? 1.0f / 30.0f : timestep);

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
        }
    }
}
