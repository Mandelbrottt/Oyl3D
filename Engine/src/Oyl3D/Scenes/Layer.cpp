#include "oylpch.h"

#include "Layer.h"

#include "ECS/Registry.h"
#include "ECS/System.h"

namespace oyl
{
    Layer::Layer(std::string name)
        : Node(std::move(name))
    {
    }

    Layer::~Layer()
    {
        for (auto& system : m_systems)
        {
            system->onExit();
        }
    }

    void Layer::onAttach()
    {
    }

    void Layer::onDetach()
    {
    }

    void Layer::onUpdate(Timestep dt)
    {
    }

    void Layer::onGuiRender()
    {
    }

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

    void Layer::onGuiRenderSystems()
    {
        for (auto& system : m_systems)
        {
            system->onGuiRender();
        }
    }

    const Ref<ECS::Registry>& Layer::getRegistry()
    {
        return registry;
    }

    void Layer::setRegistry(Ref<ECS::Registry> reg)
    {
        registry = std::move(reg);
    }
}
