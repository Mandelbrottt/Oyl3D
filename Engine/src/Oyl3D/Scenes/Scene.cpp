#include "oylpch.h"
#include "Scene.h"

namespace oyl
{
    WeakRef<Scene> Scene::s_current{};
    
    Scene::Scene(const std::string& debugName)
        : m_registry(Ref<ECS::Registry>::create()),
          m_debugName(debugName)
    {
    }

    Scene::~Scene()
    {
        m_registry->reset();
        m_registry = nullptr;
    }

    void Scene::onUpdate(Timestep dt)
    {
        for (const Ref<Layer>& layer : m_layerStack)
        {
            layer->onUpdateSystems(dt);
            layer->onUpdate(dt);
        }
    }

    void Scene::onGuiRender()
    {
        for (const Ref<Layer>& layer : m_layerStack)
        {
            layer->onGuiRenderSystems();
            layer->onGuiRender();
        }
    }

    bool Scene::onEvent(Ref<Event> event)
    {
        //bool handled = false;
        //for (const Ref<Layer>& layer : m_layerStack)
        //{
        //    handled = layer->onEvent(event);
        //    
        //    if (handled) break;
        //}
        //return handled;
        return false;
    }

    void Scene::setPostEventCallback(PostEventFn callback)
    {
        m_layerStack.setPostEventCallback(callback);
    }

    void Scene::setRegisterCallback(RegisterFn callback)
    {
        m_layerStack.setRegisterCallback(callback);
    }

    void Scene::setUnregisterCallback(UnregisterFn callback)
    {
        m_layerStack.setUnregisterCallback(callback);
    }

    void Scene::pushLayer(Ref<Layer> layer)
    {
        layer->setRegistry(m_registry);
        m_layerStack.pushLayer(std::move(layer));
    }

    void Scene::pushOverlay(Ref<Layer> overlay)
    {
        overlay->setRegistry(m_registry);
        m_layerStack.pushOverlay(std::move(overlay));
    }

    void Scene::popLayer(Ref<Layer> layer)
    {
        layer->setRegistry(nullptr);
        m_layerStack.popLayer(std::move(layer));
    }

    void Scene::popOverlay(Ref<Layer> overlay)
    {
        overlay->setRegistry(nullptr);
        m_layerStack.popOverlay(std::move(overlay));
    }
}
