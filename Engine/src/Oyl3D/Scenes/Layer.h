#pragma once

#include "Oyl3D/Events/EventListener.h"
#include "Oyl3D/Scenes/Node.h"
//#include "Oyl3D/Events/EventDispatcher.h"

//#include "ECS/Registry.h"
//#include "ECS/System.h"

namespace oyl
{
    struct Event;
    class EventDispatcher;
    
    namespace ECS
    {
        class Registry;
        class System;
    }

    class Layer : public virtual EventListener, public Node
    {
    protected:
        explicit Layer(std::string name);
        
    public:
        virtual  ~Layer();

        virtual void onAttach();
        virtual void onDetach();

        virtual void onUpdate(Timestep dt);
        virtual void onGuiRender();

        virtual bool onEvent(Ref<Event> event) override;

        void onUpdateSystems(Timestep dt);
        void onGuiRenderSystems();

        const Ref<ECS::Registry>& getRegistry();

        void setRegistry(Ref<ECS::Registry> reg);

    protected:
        // Break naming convention for sake of client usability
        Ref<ECS::Registry> registry;

        template<class T>
        void scheduleSystemUpdate(Priority priority = 0);

        std::vector<Ref<ECS::System>> m_systems;
    };

    template<class SYSTEM>
    void Layer::scheduleSystemUpdate(Priority priority)
    {
        static bool isInitialized = false;
        OYL_ASSERT(!isInitialized, "Systems should only be initialized once!");

        Ref<ECS::System> newSystem = Ref<SYSTEM>::create();

        newSystem->setRegistry(registry);

        newSystem->setDispatcher(m_dispatcher);
        m_dispatcher->registerListener(newSystem, priority);

        newSystem->onEnter();

        m_systems.emplace_back(std::move(newSystem));

        isInitialized = true;
    }
}
