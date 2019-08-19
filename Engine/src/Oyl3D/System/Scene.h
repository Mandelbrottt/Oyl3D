#pragma once

#include "Oyl3D/Debug/ImGuiLayer.h"
#include "Oyl3D/Events/Event.h"
#include "Oyl3D/System/LayerStack.h"
#include "Oyl3D/System/Timestep.h"

namespace oyl {

class Scene {
	friend class Application;
public:
	Scene(const std::string& debugName);
	virtual ~Scene();

	virtual void onEnter() = 0;
	virtual void onExit() = 0;

	void pushLayer(Layer* layer);
	void pushOverlay(Layer* overlay);
	void popLayer(Layer* layer);
	void popOverlay(Layer* overlay);
private:
	void onUpdate(Timestep dt);
	void onEvent(Event& e);
	void onImGuiRender();
private:
	LayerStack m_layerStack;

	std::string m_debugName;
};

}