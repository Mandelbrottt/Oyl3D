#include "oylpch.h"
#include "Scene.h"

namespace oyl {

Scene::Scene(const std::string& debugName)
	: m_debugName(debugName) {

}

Scene::~Scene() {

}

void Scene::onUpdate(Timestep dt) {
	for (Layer* layer : m_layerStack)
		layer->onUpdate(dt);
}

void Scene::onEvent(Event& e) {
	for (Layer* layer : m_layerStack) {
		layer->onEvent(e);
		if (e.handled) break;
	}
}

void Scene::onImGuiRender() {
	for (Layer* layer : m_layerStack)
		layer->onImGuiRender();
}

void Scene::pushLayer(Layer* layer) {
	m_layerStack.pushLayer(layer);
}

void Scene::pushOverlay(Layer* overlay) {
	m_layerStack.pushOverlay(overlay);
}

void Scene::popLayer(Layer* layer) {
	m_layerStack.popLayer(layer);
}

void Scene::popOverlay(Layer* overlay) {
	m_layerStack.popOverlay(overlay);
}

}