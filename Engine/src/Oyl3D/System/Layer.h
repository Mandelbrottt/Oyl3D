#pragma once

#include "Oyl3D/Events/Event.h"
#include "Oyl3D/System/Timestep.h"

namespace oyl {
class Layer {
public:
	Layer(const std::string& debugName = "Layer");
	virtual ~Layer();

	virtual void onAttach() = 0;
	virtual void onDetach() = 0;
	virtual void onUpdate(Timestep dt) {}
	virtual void onImGuiRender() {}
	virtual void onEvent(Event& e) {}

	inline const std::string& getName() const { return m_debugName; }
protected:
	const std::string m_debugName;
};
}