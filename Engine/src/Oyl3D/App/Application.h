#pragma once

#include "Oyl3D/App/Window.h"

//#include "Oyl3D/Debug/GuiLayer.h"

#include "Oyl3D/Graphics/Buffer.h"
#include "Oyl3D/Scenes/Scene.h"

#include "Oyl3D/Utils/SceneToFile.h"

namespace oyl
{
    class EventListener;
    class Scene;
    
    namespace internal
    {   
        class SkeletalAnimationSystem;  
        class EditorRenderSystem;  
        class PreRenderSystem;  
        class DeferredRenderSystem;  
        class ForwardRenderSystem;  
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
        void pushScene(const std::string& scene, bool callOnEnter = true);

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
        
        std::vector<Ref<System>> m_renderSystems;
        
        Ref<internal::SkeletalAnimationSystem> m_skeletalAnimationSystem;
        //Ref<internal::EditorRenderSystem> m_editorRenderSystem;
        //Ref<internal::PreRenderSystem> m_preRenderSystem;
        //Ref<internal::ShadowRenderSystem> m_shadowRenderSystem;
        //Ref<internal::ForwardRenderSystem> m_forwardRenderSystem;
        //Ref<internal::DeferredRenderSystem> m_deferredRenderSystem;
        //Ref<internal::GuiRenderSystem> m_guiRenderSystem;
        //Ref<internal::UserPostRenderSystem> m_postRenderSystem;

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

        std::string name = newScene->m_name;

        newScene->setDispatcher(m_dispatcher);
        
    #if !defined OYL_DISTRIBUTION
        newScene->Scene::onEnter();
        newScene->onEnter();

        internal::registryFromSceneFile(*newScene->m_registry, name);
    #endif

        bool swapToScene = m_registeredScenes.empty();

        m_registeredScenes[name] = std::move(newScene);

        //pushScene(name, !swapToScene);

        if (swapToScene)
        {
            m_nextScene = name;
            //pushScene(name);
        }
    }

    Application* createApplication();
}
