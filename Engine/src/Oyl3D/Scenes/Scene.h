#pragma once

//#include "Oyl3D/Events/Event.h"
//#include "Oyl3D/Events/EventListener.h"

#include "Oyl3D/Scenes/LayerStack.h"
#include "Oyl3D/Scenes/Node.h"

namespace oyl
{
    class Scene;
    
    namespace internal
    {
        class GuiLayer;
        class SceneToFile;

        void saveSceneToFile(const Scene& scene);
        void loadSceneFromFile(Scene& scene);
        void saveSceneBackupToFile(const Scene& scene);
    }

    class Scene : public EventListener, public virtual Node
    {
    protected:
        explicit Scene();
    public:
        virtual ~Scene();

        virtual void onEnter() override;
        virtual void onExit() override;

        virtual void onUpdate(Timestep dt) override;
        virtual void onGuiRender(Timestep dt) override;
        
        void pushLayer(Ref<Layer> layer);
        void pushOverlay(Ref<Layer> overlay);
        void popLayer(const Ref<Layer>& layer);
        void popOverlay(const Ref<Layer>& overlay);

        static Ref<Scene> current() { return s_current.lock(); }

        const Ref<entt::registry>& getRegistry() { return m_registry; }

        virtual bool onEvent(Ref<Event> event) override;

    private:
        static WeakRef<Scene> s_current;

        // TODO: Put in internal namespace
        LayerStack m_layerStack;

        Ref<entt::registry> m_registry;

        friend class Application;
        friend internal::GuiLayer;

        friend void internal::saveSceneToFile(const Scene& scene);
        friend void internal::loadSceneFromFile(Scene& scene);
        friend void internal::saveSceneBackupToFile(const Scene& scene);
    };
}
