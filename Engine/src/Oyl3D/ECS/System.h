#pragma once

#include "Oyl3D/Events/Event.h"
#include "Oyl3D/Events/EventListener.h"

namespace oyl::ECS
{
    class ISystem
    {
    public:
        explicit ISystem() = default;
        virtual ~ISystem() = default;
        
        virtual void onEnter() = 0;
        virtual void onExit() = 0;

        virtual void onUpdate(Timestep dt) = 0;
        virtual void onGuiRender() = 0;
    };

    class System : public ISystem, public EventListener
    {
    public:
        explicit System() = default;
        virtual ~System() = default;

        virtual void onEnter() override;
        virtual void onExit() override;
        
        virtual void onUpdate(Timestep dt) override;
        virtual void onGuiRender() override;
    };
}