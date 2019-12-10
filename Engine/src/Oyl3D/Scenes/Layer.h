#pragma once

#include "Oyl3D/ECS/System.h"

#include "Oyl3D/Events/EventDispatcher.h"
#include "Oyl3D/Events/EventListener.h"

#include "Oyl3D/Scenes/Node.h"

namespace oyl
{
    struct Event;
    class EventDispatcher;
    class System;

    class Layer : public virtual EventListener, public virtual Node
    {
    protected:
        explicit Layer() = default;
        
    public:
        virtual  ~Layer();

        virtual void onEnter() override;
        virtual void onExit() override;

        virtual void onUpdate() override;
        virtual void onGuiRender() override;

        virtual bool onEvent(Ref<Event> event) override;

        void onUpdateSystems();
        void onGuiRenderSystems();

        const Ref<entt::registry>& getRegistry() const { return registry; }

        void setRegistry(const Ref<entt::registry>& reg);

    protected:
        // Break naming convention for sake of client usability
        Ref<entt::registry> registry;

        template<class T>
        void scheduleSystemUpdate(Priority priority = 0);

        std::vector<Ref<System>> m_systems;
    };

    template<class SYSTEM>
    void Layer::scheduleSystemUpdate(Priority priority)
    {
        //static bool isInitialized = false;
        //OYL_ASSERT(!isInitialized, "Systems should only be initialized once!");

        Ref<System> newSystem = SYSTEM::create();
        
        newSystem->setRegistry(registry);

        OYL_ASSERT(m_dispatcher, "Dispatcher should be initialized!");
        newSystem->setDispatcher(m_dispatcher);
        m_dispatcher->registerListener(newSystem, priority);

        newSystem->onEnter();

        m_systems.emplace_back(std::move(newSystem));

        //isInitialized = true;
    }
}
