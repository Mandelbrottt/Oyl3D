#pragma once

#include "Oyl3D/App/Window.h"

//#include "Oyl3D/Debug/GuiLayer.h"

#include "Oyl3D/Graphics/Buffer.h"
#include "Oyl3D/Scenes/Scene.h"

namespace oyl
{
    class EventListener;
    class Scene;
    
    namespace internal
    {   
        class RenderSystem;  
        class GuiRenderSystem;  
        class ApplicationListener;
        class GamepadListener;
        class SystemsLayer;
        class GuiLayer;
    }

    class Application
    {
    public:
        explicit Application();
        virtual  ~Application();

        void run();

        bool onEvent(const Event& event);

        void pushScene(Ref<Scene> scene);

        // TODO: Make Refs
        inline Window&      getWindow() { return *m_window; }
        inline FrameBuffer& getMainFrameBuffer() { return *m_mainBuffer; }

        inline static Application& get() { return *s_instance; }

    private:
        void initEventListeners();

    private:
        Ref<Window>      m_window;
        Ref<Scene>       m_currentScene;
        Ref<FrameBuffer> m_mainBuffer;
        
    #if !defined(OYL_DISTRIBUTION)
        Ref<internal::GuiLayer> m_guiLayer;
    #endif

        Ref<internal::SystemsLayer> m_systemsLayer;
        // TODO: Put in systems layer
        Ref<internal::RenderSystem> m_renderSystem;
        Ref<internal::GuiRenderSystem> m_guiRenderSystem;

        float m_lastFrameTime = 0;

        bool m_running  = true;
        bool m_doUpdate = true;

    private:
        static Application* s_instance;

        Ref<EventDispatcher> m_dispatcher;

        Ref<internal::ApplicationListener> m_appListener;
        Ref<internal::GamepadListener>     m_vibrationListener;
    };

    Application* createApplication();
}
