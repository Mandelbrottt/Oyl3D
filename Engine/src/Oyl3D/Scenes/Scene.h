#pragma once

#include "Oyl3D/Scenes/LayerStack.h"
#include "Oyl3D/Scenes/Node.h"

namespace oyl
{
    namespace ECS
    {
        class Registry;
        class RenderSystem;
        class PhysicsSystem;
    }

    class Scene : public EventListener, public virtual Node
    {
        friend class Application;
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

        void loadSceneFromFile();
        void saveSceneToFile();

        static Ref<Scene> current() { return s_current.lock(); }

        const Ref<ECS::Registry>& getRegistry() { return m_registry; }

        virtual bool onEvent(Ref<Event> event) override;

    private:
        void initDefaultSystems();

        void saveSceneBackupToFile();

        static WeakRef<Scene> s_current;

    private:
        // TODO: Put in internal namespace
        LayerStack m_layerStack;

        Ref<ECS::Registry> m_registry;

        Ref<ECS::RenderSystem> m_renderSystem;
        Ref<ECS::PhysicsSystem> m_physicsSystem;
    };
}
