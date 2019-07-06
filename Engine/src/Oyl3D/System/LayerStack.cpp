#include "oylpch.h"
#include "LayerStack.h"

namespace oyl {



LayerStack::LayerStack() {
	m_layerInsert = m_layers.begin();
}

LayerStack::~LayerStack() {
	for (Layer* layer : m_layers)
		delete layer;
}

void LayerStack::pushLayer(Layer* layer) {
	m_layerInsert = m_layers.emplace(m_layerInsert, layer);
	layer->onAttach();
}

void LayerStack::pushOverlay(Layer* overlay) {
	m_layers.emplace_back(overlay);
	overlay->onAttach();
}

void LayerStack::popLayer(Layer* layer) {
	auto it = std::find(m_layers.begin(), m_layers.end(), layer);
	if (it != m_layers.end()) {
		m_layers.erase(it);
		m_layerInsert--;
	}
	layer->onDetach();
}

void LayerStack::popOverlay(Layer* overlay) {
	auto it = std::find(m_layers.begin(), m_layers.end(), overlay);
	if (it != m_layers.end()) {
		m_layers.erase(it);
	}
	overlay->onDetach();
}

}

