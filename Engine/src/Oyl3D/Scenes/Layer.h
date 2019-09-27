#pragma once

#include "Oyl3D/Events/Event.h"
#include "Oyl3D/Events/EventListener.h"
#include "Oyl3D/ECS/System.h"

namespace oyl
{    
    class Layer : public EventListener
    {
    public:
        explicit Layer(const std::string& debugName = "Layer");
        virtual ~Layer();

        virtual void onAttach();
        virtual void onDetach();

        virtual void onUpdate(Timestep dt);
        virtual void onGuiRender();

        virtual bool onEvent(Ref<Event> event) override;

        // TEMPORARY:
        void onUpdateSystems(Timestep dt);
        void onGuiRenderSystems();
        bool onEventSystems(Ref<Event> event);

    protected:
        template<class T>
        void scheduleSystemUpdate(Priority priority = 0);

        std::vector<Ref<ECS::System>> m_systems;
        
#if defined(OYL_LOG_CONSOLE)
    protected:
        const std::string m_debugName;
#endif
    };

    template<class T>
    void Layer::scheduleSystemUpdate(Priority priority)
    {
        static bool isInitialized = false;
        OYL_ASSERT(!isInitialized, "Systems should only be initialized once!");

        Ref<ECS::System> newSystem = Ref<T>::create();

        newSystem->setPostEventCallback(m_postEventCallback);
        m_registerCallback(newSystem, priority);
        
        m_systems.emplace_back(std::move(newSystem));

        isInitialized = true;
    }
}
