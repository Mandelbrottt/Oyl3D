#pragma once

#include "Oyl3D/Common.h"

#include "Oyl3D/App/Window.h"

#include "Oyl3D/Debug/ImGuiLayer.h"

#include "Oyl3D/Events/Event.h"
#include "Oyl3D/Events/EventListener.h"

#include "Oyl3D/Graphics/Buffer.h"
#include "Oyl3D/Graphics/Camera.h"

#include "Oyl3D/Scenes/LayerStack.h"
#include "Oyl3D/Scenes/Scene.h"

namespace oyl
{
    class Application
    {
    public:
        Application();
        virtual ~Application();

        void run();

        void onEvent(Event& e);

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
    };

    Application* createApplication();
}
