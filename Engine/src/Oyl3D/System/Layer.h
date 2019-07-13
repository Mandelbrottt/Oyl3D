#pragma once

#include "Oyl3D/Events/Event.h"

namespace oyl {
class Layer {
public:
	Layer(const std::string& debugName = "Layer");;
	virtual ~Layer();

	virtual void onAttach() {}
	virtual void onDetach() {}
	virtual void onUpdate() {}
	virtual void onImGuiRender() {}
	virtual void onEvent(Event& e) {}

	inline const std::string& getName() const { return m_debugName; }
protected:
	const std::string m_debugName;
};
}