#pragma once

//#include "Oyl3D/Events/Event.h"

#include "Oyl3D/Events/EventListener.h"

#include "Oyl3D/Scenes/Node.h"

namespace oyl
{
    class ISystem : public virtual Node
    {
    public:
        explicit ISystem()  = default;
        virtual  ~ISystem() = default;

        virtual void onEnter() override = 0;
        virtual void onExit() override = 0;

        virtual void onUpdate() override = 0;
        virtual void onGuiRender() override = 0;
    };

    class System : public ISystem, public EventListener
    {
    protected:
        explicit System() = default;

    public:
        virtual ~System() = default;

        // TODO: Differentiate with onCreate, onDestroy, etc.
        virtual void onEnter() override {}
        virtual void onExit() override {}

        virtual void onUpdate() override {}
        virtual void onGuiRender() override {}

        void setRegistry(const Ref<entt::registry>& reg) { registry = reg; }

    protected:
        Ref<entt::registry> registry;
    };
}
