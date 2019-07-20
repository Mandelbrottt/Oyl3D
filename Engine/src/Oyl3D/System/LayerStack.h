#pragma once

#include "Oyl3D/System/Layer.h"

namespace oyl {

class LayerStack {
public:
	LayerStack();
	~LayerStack();

	void pushLayer(Layer* layer);
	void pushOverlay(Layer* overlay);
	void popLayer(Layer* layer);
	void popOverlay(Layer* overlay);

	std::vector<Layer*>::iterator begin() { return m_layers.begin(); }
	std::vector<Layer*>::iterator end() { return m_layers.end(); }
	std::vector<Layer*>::const_iterator begin() const { return m_layers.begin(); }
	std::vector<Layer*>::const_iterator end() const { return m_layers.end(); }
private:
	std::vector<Layer*> m_layers;
	uint m_layerInsertIndex = 0;
};

}

