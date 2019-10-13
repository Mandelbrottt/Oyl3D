#include "oylpch.h"
#include "LayerStack.h"

namespace oyl
{
    LayerStack::LayerStack()
    {
    }

    LayerStack::~LayerStack()
    {
        for (Ref<Layer> layer : m_layers)
        {
            layer->onDetach();
            layer.reset();
        }
    }

    void LayerStack::pushLayer(Ref<Layer> layer)
    {
        m_layers.emplace(m_layers.begin() + m_layerInsertIndex, layer);
        m_layerInsertIndex++;

        m_registerCallback(layer, 0);
        
        layer->setPostEventCallback(m_postEventCallback);
        layer->setRegisterCallback(m_registerCallback);
        layer->setUnregisterCallback(m_unregisterCallback);

        layer->onAttach();
    }

    void LayerStack::pushOverlay(Ref<Layer> overlay)
    {
        m_layers.emplace_back(overlay);

        m_registerCallback(overlay, 0);

        overlay->setPostEventCallback(m_postEventCallback);
        overlay->setRegisterCallback(m_registerCallback);
        overlay->setUnregisterCallback(m_unregisterCallback);

        overlay->onAttach();
    }

    void LayerStack::popLayer(Ref<Layer> layer)
    {
        auto it = std::find(m_layers.begin(), m_layers.end(), layer);
        if (it != m_layers.end())
        {
            m_layers.erase(it);
            m_layerInsertIndex--;

            m_unregisterCallback(layer);

            layer->setPostEventCallback(nullptr);
            layer->setRegisterCallback(nullptr);
            layer->setUnregisterCallback(nullptr);
            
            layer->onDetach();
        }
    }

    void LayerStack::popOverlay(Ref<Layer> overlay)
    {
        auto it = std::find(m_layers.begin(), m_layers.end(), overlay);
        if (it != m_layers.end())
        {
            m_layers.erase(it);

            m_unregisterCallback(overlay);

            overlay->setPostEventCallback(nullptr);
            overlay->setRegisterCallback(nullptr);
            overlay->setUnregisterCallback(nullptr);
            
            overlay->onDetach();
        }
    }
}
