#include "oylpch.h"

#include "Layer.h"

#include "ECS/System.h"

namespace oyl
{
    Layer::~Layer()
    {
        for (auto& system : m_systems)
        {
            system->onExit();
        }
    }

    void Layer::onEnter() { }

    void Layer::onExit() { }

    void Layer::onUpdate(Timestep dt) { }

    void Layer::onGuiRender(Timestep dt) { }

    bool Layer::onEvent(Ref<Event> event)
    {
        return false;
    }

    void Layer::onUpdateSystems(Timestep dt)
    {
        for (auto& system : m_systems)
        {
            system->onUpdate(dt);
        }
    }

    void Layer::onGuiRenderSystems(Timestep dt)
    {
        for (auto& system : m_systems)
        {
            system->onGuiRender(dt);
        }
    }

    void Layer::setRegistry(const Ref<entt::registry>& reg)
    {
        registry = reg;
    }
}
