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

        void onUpdateSystems(Timestep dt);
        void onGuiRenderSystems();

    protected:
        template<class T>
        void scheduleSystemUpdate(Priority priority = 0);

        std::vector<Ref<ECS::System>> m_systems;
        
#if defined(OYL_LOG_CONSOLE)
    protected:
        const std::string m_debugName;
#endif
    };
}

#include "Layer.inl"
