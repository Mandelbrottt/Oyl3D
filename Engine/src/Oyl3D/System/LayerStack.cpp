#include "oylpch.h"
#include "LayerStack.h"

namespace oyl {

LayerStack::LayerStack() {

}

LayerStack::~LayerStack() {
	for (Ref<Layer> layer : m_layers) {
		layer->onDetach();
		layer.reset();
	}
}

void LayerStack::pushLayer(Ref<Layer> layer) {
	m_layers.emplace(m_layers.begin() + m_layerInsertIndex, layer);
	m_layerInsertIndex++;
	layer->onAttach();
}

void LayerStack::pushOverlay(Ref<Layer> overlay) {
	m_layers.emplace_back(overlay);
	overlay->onAttach();
}

void LayerStack::popLayer(Ref<Layer> layer) {
	auto it = std::find(m_layers.begin(), m_layers.end(), layer);
	if (it != m_layers.end()) {
		m_layers.erase(it);
		m_layerInsertIndex--;
	}
	layer->onDetach();
}

void LayerStack::popOverlay(Ref<Layer> overlay) {
	auto it = std::find(m_layers.begin(), m_layers.end(), overlay);
	if (it != m_layers.end()) {
		m_layers.erase(it);
	}
	overlay->onDetach();
}

}

