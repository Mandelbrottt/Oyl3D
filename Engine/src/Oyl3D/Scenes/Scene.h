#pragma once

#include "Oyl3D/ECS/Registry.h"

#include "Oyl3D/Events/Event.h"
#include "Oyl3D/Events/EventListener.h"

#include "Oyl3D/Scenes/LayerStack.h"

namespace oyl
{
    class Scene : public EventListener
    {
        friend class Application;
    public:
        explicit Scene(const std::string& debugName = "Scene");
        virtual ~Scene();

        virtual void onEnter() = 0;
        virtual void onExit() = 0;

        void pushLayer(Ref<Layer> layer);
        void pushOverlay(Ref<Layer> overlay);
        void popLayer(Ref<Layer> layer);
        void popOverlay(Ref<Layer> overlay);

        static Ref<Scene> current() { return s_current.lock(); }

        const Ref<ECS::Registry>& getRegistry() { return m_registry; }

        virtual void setPostEventCallback(PostEventFn callback) override final;
        virtual void setRegisterCallback(RegisterFn callback) override final;
        virtual void setUnregisterCallback(UnregisterFn callback) override final;
        
        virtual bool onEvent(Ref<Event> event) override;
    private:
        void onUpdate(Timestep dt);
        void onGuiRender();

        static WeakRef<Scene> s_current;
        
    private:
        LayerStack m_layerStack;

        Ref<ECS::Registry> m_registry;
        
    protected:
        std::string m_debugName;
    };
}
