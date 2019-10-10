#pragma once

#include "Oyl3D/Common.h"

#include "Oyl3D/App/Window.h"

#include "Oyl3D/Debug/ImGuiLayer.h"

#include "Oyl3D/Input/GamepadVibration.h"

#include "Oyl3D/Events/Event.h"
#include "Oyl3D/Events/EventListener.h"
#include "Oyl3D/Events/EventDispatcher.h"

#include "Oyl3D/Graphics/Buffer.h"
#include "Oyl3D/Graphics/Camera.h"

#include "Oyl3D/Scenes/LayerStack.h"
#include "Oyl3D/Scenes/Scene.h"


namespace oyl
{
    namespace _internal
    {
        class ApplicationListener;
    }

    class Application
    {
    public:
        Application();
        virtual ~Application();

        void run();

        bool onEvent(Ref<Event> event);

        void pushScene(Ref<Scene> scene);

        inline Window&            getWindow() { return *m_window; }
        inline FrameBuffer&       getMainFrameBuffer() { return *m_mainBuffer; }
        inline PerspectiveCamera& getMainCamera() { return m_camera; }

        inline static Application& get() { return *s_instance; }
    private:

        Ref<Window>      m_window;
        Ref<Scene>       m_currentScene;
        Ref<ImGuiLayer>  m_imguiLayer;
        Ref<FrameBuffer> m_mainBuffer;

        PerspectiveCamera m_camera;

        float m_lastFrameTime = 0;

        bool m_running  = true;
        bool m_doUpdate = true;
    private:
        static Application* s_instance;

        EventDispatcher m_dispatcher;
        
        std::function<void(UniqueRef<Event>)> m_dispatcherPostCallback;
        
        std::function<void(Ref<IEventListener>, IEventListener::Priority)> m_dispatcherRegisterCallback;
        std::function<void(Ref<IEventListener>)> m_dispatcherUnregisterCallback;

        Ref<_internal::ApplicationListener> m_appListener;
        Ref<_internal::GamepadVibration> m_vibrationListener;
    };

    Application* createApplication();
}
