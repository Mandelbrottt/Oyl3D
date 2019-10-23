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
        public:
            ApplicationListener()
            {
                addToCategoryMask(CategoryWindow);
                //addToCategoryMask(CategoryKeyboard);
                //addToCategoryMask(CategoryMouse);
                addToCategoryMask(CategoryCursorStateRequest);
            }

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

        Log::init();

        m_window = Window::create();

        m_imguiLayer = Ref<ImGuiLayer>::create();

        initEventListeners();

        m_mainBuffer = oyl::FrameBuffer::create(1);
        m_mainBuffer->initDepthTexture(m_window->getWidth(), m_window->getHeight());

        m_mainBuffer->initColorTexture(0, m_window->getWidth(), m_window->getHeight(),
                                       oyl::RGBA8,
                                       oyl::Nearest,
                                       oyl::Clamp);

        m_window->setVsync(false);
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
            case TypeCursorStateRequest:
            {
                auto e = (CursorStateRequestEvent) *event;
                if (m_window->getCursorState() != e.state)
                    m_window->setCursorState(e.state);
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

            m_dispatcher->registerListener(m_currentScene);
            m_currentScene->setDispatcher(m_dispatcher);

            m_currentScene->initDefaultSystems();

            m_currentScene->pushOverlay(m_imguiLayer);
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

                Renderer::beginScene();

                m_dispatcher->dispatchEvents();

                m_currentScene->onUpdate(timestep);

                Renderer::endScene();
                m_mainBuffer->unbind();
            }

#if !defined(OYL_DISTRIBUTION)
            m_imguiLayer->begin();

            m_currentScene->onGuiRender(timestep);

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
        m_dispatcher = Ref<EventDispatcher>::create();

        // TODO: Make Window an EventListener
        m_window->setEventCallback(OYL_CALLBACK_1(EventDispatcher::postEvent, m_dispatcher.get()));

        m_appListener      = Ref<_internal::ApplicationListener>::create();
        m_appListener->app = this;
        m_dispatcher->registerListener(m_appListener);

        m_vibrationListener = _internal::GamepadListener::create();
        m_dispatcher->registerListener(m_vibrationListener);

        m_dispatcher->registerListener(m_imguiLayer);
    }
}
