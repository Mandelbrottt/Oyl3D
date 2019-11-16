#pragma once

//#include "Oyl3D/Events/Event.h"

#include "Oyl3D/Events/EventListener.h"

#include "Oyl3D/Scenes/Node.h"

namespace oyl
{
    class Camera;

    class ISystem : public virtual Node
    {
    public:
        explicit ISystem()  = default;
        virtual  ~ISystem() = default;

        virtual void onEnter() override = 0;
        virtual void onExit() override = 0;

        virtual void onUpdate(Timestep dt) override = 0;
        virtual void onGuiRender(Timestep dt) override = 0;
    };

    class System : public ISystem, public EventListener
    {
    protected:
        explicit System() = default;

    public:
        virtual ~System() = default;

        virtual void onEnter() override;
        virtual void onExit() override;

        virtual void onUpdate(Timestep dt) override;
        virtual void onGuiRender(Timestep dt) override;

        void setRegistry(Ref<entt::registry> reg) { registry = std::move(reg); }

    protected:
        Ref<entt::registry> registry;
    };
}
