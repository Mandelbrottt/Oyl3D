#include "Hazel.h"

class ExampleLayer : public Hazel::Layer {
public:
	ExampleLayer()
		: Layer("Example") {}

	void onUpdate() override {
		
	}

	virtual void onImGuiRender() override {
		
	}

	void onEvent(Hazel::Event& event) override {

	}
};

class Sandbox : public Hazel::Application {
public:
	Sandbox() {
		pushLayer(new ExampleLayer());
	}
	~Sandbox() {}
};

Hazel::Application* Hazel::createApplication() {
	return new Sandbox();
}