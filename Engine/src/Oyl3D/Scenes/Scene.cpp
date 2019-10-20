#include "oylpch.h"
#include "Scene.h"

#include "ECS/System.h"
#include "ECS/Registry.h"

#include "Events/EventDispatcher.h"


namespace oyl
{
    WeakRef<Scene> Scene::s_current{};

    Scene::Scene(const std::string& debugName)
        : m_registry(Ref<ECS::Registry>::create()),
          m_renderSystem(Ref<ECS::RenderSystem>::create()),
          m_debugName(debugName)
    {
    }

    Scene::~Scene()
    {
        // Reset the registry then reset the actual Ref
        m_registry->reset();
        m_registry.reset();
    }

    void Scene::onUpdate(Timestep dt)
    {
        for (const Ref<Layer>& layer : m_layerStack)
        {
            layer->onUpdateSystems(dt);
            layer->onUpdate(dt);
        }

        m_renderSystem->onUpdate(dt);
    }

    bool Scene::onEvent(Ref<Event> event)
    {
        return false;
    }
    
    void Scene::onGuiRender()
    {
        for (const Ref<Layer>& layer : m_layerStack)
        {
            layer->onGuiRenderSystems();
            layer->onGuiRender();
        }

        m_renderSystem->onGuiRender();
    }

    void Scene::initDefaultSystems()
    {
        m_renderSystem->setRegistry(this->m_registry);
        m_renderSystem->setDispatcher(m_dispatcher);
        m_dispatcher->registerListener(m_renderSystem);
    }

    const Ref<ECS::Registry>& Scene::getRegistry()
    {
        return m_registry;
    }

    void Scene::pushLayer(Ref<Layer> layer)
    {
        layer->setRegistry(m_registry);
        layer->setDispatcher(m_dispatcher);
        m_dispatcher->registerListener(layer);
        
        m_layerStack.pushLayer(std::move(layer));
    }

    void Scene::pushOverlay(Ref<Layer> overlay)
    {
        overlay->setRegistry(m_registry);
        overlay->setDispatcher(m_dispatcher);
        m_dispatcher->registerListener(overlay);
        
        m_layerStack.pushOverlay(std::move(overlay));
    }

    void Scene::popLayer(const Ref<Layer>& layer)
    {
        layer->setRegistry(nullptr);
        layer->setDispatcher(nullptr);
        m_dispatcher->unregisterListener(layer);
        
        m_layerStack.popLayer(layer);
    }

    void Scene::popOverlay(const Ref<Layer>& overlay)
    {
        overlay->setRegistry(nullptr);
        overlay->setDispatcher(nullptr);
        m_dispatcher->unregisterListener(overlay);
        
        m_layerStack.popOverlay(overlay);
    }
}
