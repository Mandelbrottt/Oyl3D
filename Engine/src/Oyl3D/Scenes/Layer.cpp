#include "oylpch.h"

#include "Layer.h"

namespace oyl
{
    Layer::Layer(const std::string& debugName)
    #if defined(OYL_LOG_CONSOLE)
        : m_debugName(debugName)
    #endif
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

    void Layer::setRegistry(Ref<ECS::Registry> reg)
    {
        registry = std::move(reg);
    }
}
