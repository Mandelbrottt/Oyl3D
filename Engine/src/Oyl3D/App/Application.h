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
        class SkeletalAnimationSystem;  
        class EditorRenderSystem;  
        class PreRenderSystem;  
        class RenderSystem;  
        class GuiRenderSystem;  
        class ShadowRenderSystem;  
        class UserPostRenderSystem;  
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

        void changeScene(const std::string& name);

        // TODO: Make Refs
        Window& getWindow() { return *m_window; }
        //inline FrameBuffer& getMainFrameBuffer() { return *m_mainBuffer; }

        static Application& get() { return *s_instance; }

    protected:
        template<class T, std::enable_if_t<std::is_base_of<Scene, T>::value, int> = 0>
        void registerScene();
        
    private:
        void initEventListeners();
        void pushScene(const Ref<Scene>& scene, bool callOnEnter = true);

    private:
        Ref<Window> m_window;
        
        std::string m_currentScene;
        std::string m_nextScene;

        std::unordered_map<std::string, Ref<Scene>> m_registeredScenes;
        
    #if !defined(OYL_DISTRIBUTION)
        Ref<internal::GuiLayer> m_guiLayer;
    #endif

        Ref<internal::SystemsLayer> m_systemsLayer;
        // TODO: Put in systems layer
        Ref<internal::SkeletalAnimationSystem> m_skeletalAnimationSystem;
        Ref<internal::EditorRenderSystem> m_editorRenderSystem;
        Ref<internal::PreRenderSystem> m_preRenderSystem;
        Ref<internal::ShadowRenderSystem> m_shadowRenderSystem;
        Ref<internal::RenderSystem> m_renderSystem;
        Ref<internal::GuiRenderSystem> m_guiRenderSystem;
        Ref<internal::UserPostRenderSystem> m_postRenderSystem;

        bool m_running  = true;
        bool m_doUpdate = true;

    private:
        static Application* s_instance;

        Ref<EventDispatcher> m_dispatcher;

        Ref<internal::ApplicationListener> m_appListener;
        Ref<internal::GamepadListener>     m_vibrationListener;

        friend internal::GuiLayer;
    };

    template<class S, std::enable_if_t<std::is_base_of<Scene, S>::value, int>>
    void Application::registerScene()
    {
        Ref<Scene> newScene = S::create();
        if (m_registeredScenes.find(newScene->m_name) != m_registeredScenes.end())
            return;

        if (m_registeredScenes.empty())
        {
            m_nextScene = newScene->m_name;
            pushScene(newScene);
        }

        m_registeredScenes[newScene->m_name] = std::move(newScene);
    }

    Application* createApplication();
}
