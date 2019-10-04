#include "oylpch.h"
#include "Scene.h"

namespace oyl
{
    Scene::Scene(const std::string& debugName)
    #if defined(OYL_LOG_CONSOLE)
        : m_debugName(debugName)
    #endif
    {
    }

    Scene::~Scene()
    {
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
        m_layerStack.pushLayer(std::move(layer));
    }

    void Scene::pushOverlay(Ref<Layer> overlay)
    {
        m_layerStack.pushOverlay(std::move(overlay));
    }

    void Scene::popLayer(Ref<Layer> layer)
    {
        m_layerStack.popLayer(std::move(layer));
    }

    void Scene::popOverlay(Ref<Layer> overlay)
    {
        m_layerStack.popOverlay(std::move(overlay));
    }
}
