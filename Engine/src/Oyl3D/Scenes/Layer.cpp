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

    void Layer::onUpdate() { }

    void Layer::onGuiRender() { }

    bool Layer::onEvent(const Event& event)
    {
        return false;
    }

    void Layer::onUpdateSystems()
    {
        for (auto& system : m_systems)
        {
            system->onUpdate();
        }
    }

    void Layer::onGuiRenderSystems()
    {
        for (auto& system : m_systems)
        {
            system->onGuiRender();
        }
    }

    void Layer::setRegistry(const Ref<entt::registry>& reg)
    {
        registry = reg;
    }
}
