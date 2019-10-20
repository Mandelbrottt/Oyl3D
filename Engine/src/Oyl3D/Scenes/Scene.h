#pragma once

//#include "Oyl3D/ECS/Registry.h"

//#include "Oyl3D/Events/Event.h"
//#include "Oyl3D/Events/EventListener.h"

#include "Oyl3D/Scenes/LayerStack.h"

namespace oyl
{
    namespace ECS
    {
        class Registry;
        class RenderSystem;
    }

    class Scene : public EventListener
    {
        friend class Application;
    public:
        explicit Scene(const std::string& debugName = "Scene");
        virtual  ~Scene();

        virtual void onEnter() = 0;
        virtual void onExit() = 0;

        void pushLayer(Ref<Layer> layer);
        void pushOverlay(Ref<Layer> overlay);
        void popLayer(const Ref<Layer>& layer);
        void popOverlay(const Ref<Layer>& overlay);

        static Ref<Scene> current() { return s_current.lock(); }

        const Ref<ECS::Registry>& getRegistry();

        virtual bool onEvent(Ref<Event> event) override;
    private:
        void onUpdate(Timestep dt);
        void onGuiRender();

        void initDefaultSystems();

        static WeakRef<Scene> s_current;

    private:
        // TODO: Put in internal namespace
        LayerStack m_layerStack;

        Ref<ECS::Registry> m_registry;

        Ref<ECS::RenderSystem> m_renderSystem;

    protected:
        std::string m_debugName;
    };
}
