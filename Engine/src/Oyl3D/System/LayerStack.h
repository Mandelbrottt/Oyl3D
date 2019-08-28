#pragma once

#include "Oyl3D/System/Layer.h"

namespace oyl {

class LayerStack {
public:
	LayerStack();
	~LayerStack();

	void pushLayer(Ref<Layer> layer);
	void pushOverlay(Ref<Layer> overlay);
	void popLayer(Ref<Layer> layer);
	void popOverlay(Ref<Layer> overlay);

	std::vector<Ref<Layer>>::iterator begin() { return m_layers.begin(); }
	std::vector<Ref<Layer>>::iterator end() { return m_layers.end(); }
	std::vector<Ref<Layer>>::const_iterator begin() const { return m_layers.begin(); }
	std::vector<Ref<Layer>>::const_iterator end() const { return m_layers.end(); }
private:
	std::vector<Ref<Layer>> m_layers;
	uint m_layerInsertIndex = 0;
};

}

