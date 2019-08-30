#include <Oyl3D.h>

class MainLayer : public oyl::Layer {
public:
	MainLayer() : Layer("MainLayer") {}

	virtual void onAttach() override {}

	virtual void onDetach() override {}

	virtual void onUpdate(oyl::Timestep dt) override {}

	virtual void onImGuiRender() override {}

	virtual void onEvent(oyl::Event& event) override {}
public:
	OYL_CREATE_FUNC(MainLayer)
};

class MainScene : public oyl::Scene {
public:
	MainScene() : oyl::Scene("MainScene") {}
	~MainScene() {}

	virtual void onEnter() {
		pushLayer(MainLayer::create());
	}

	virtual void onExit() {}
public:
	OYL_CREATE_FUNC(MainScene)
};

class Game : public oyl::Application {
public:
	Game() {
		pushScene(MainScene::create());
	}

	~Game() {}
};

oyl::Application* oyl::createApplication() {
	return new Game();
}