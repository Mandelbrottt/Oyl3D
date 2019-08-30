#include "oylpch.h"
#include "Scene.h"

namespace oyl {

Scene::Scene(const std::string& debugName) 
#if defined(OYL_LOG_CONSOLE)
	: m_debugName(debugName)
#endif 
{

}

Scene::~Scene() {

}

void Scene::onUpdate(Timestep dt) {
	for (Ref<Layer> layer : m_layerStack)
		layer->onUpdate(dt);
}

void Scene::onEvent(Event& e) {
	for (Ref<Layer> layer : m_layerStack) {
		layer->onEvent(e);
		if (e.handled) break;
	}
}

void Scene::onImGuiRender() {
	for (Ref<Layer> layer : m_layerStack)
		layer->onImGuiRender();
}

void Scene::pushLayer(Ref<Layer> layer) {
	m_layerStack.pushLayer(layer);
}

void Scene::pushOverlay(Ref<Layer> overlay) {
	m_layerStack.pushOverlay(overlay);
}

void Scene::popLayer(Ref<Layer> layer) {
	m_layerStack.popLayer(layer);
}

void Scene::popOverlay(Ref<Layer> overlay) {
	m_layerStack.popOverlay(overlay);
}

}