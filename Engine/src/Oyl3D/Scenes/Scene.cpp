#include "oylpch.h"
#include "Scene.h"

#include "SystemsLayer.h"

#include "Events/EventDispatcher.h"

#include "Utils/SceneToFile.h"

namespace oyl
{
    WeakRef<Scene> Scene::s_current = {};
    
    Scene::Scene()
        : m_registry(Ref<entt::registry>::create()) {}

    Scene::~Scene() {}

    void Scene::onEnter() {}

    void Scene::onExit()
    {
        internal::saveSceneBackupToFile(*this);

        // Reset the registry then reset the actual Ref
        m_registry->reset();
        m_registry.reset();

    }

    void Scene::onUpdate()
    {
        for (const Ref<Layer>& layer : m_layerStack)
        {
            layer->onUpdateSystems();
            layer->onUpdate();
        }
    }

    bool Scene::onEvent(const Event& event)
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
