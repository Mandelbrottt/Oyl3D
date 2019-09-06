#include <Oyl3D.h>

class ClientLayer : public oyl::Layer {
public:
    OYL_CREATE_FUNC(ClientLayer)

    ClientLayer() : Layer("ClientLayer") {}

    virtual void onAttach() override {}

    virtual void onDetach() override {}

    virtual void onUpdate(oyl::Timestep dt) override {}

    virtual void onImGuiRender() override {}

    virtual void onEvent(oyl::Event& event) override {}
};

class ClientScene : public oyl::Scene {
public:
    OYL_CREATE_FUNC(ClientScene)

    ClientScene() : oyl::Scene("ClientScene") {}
    ~ClientScene() {}

    virtual void onEnter() {
        pushLayer(ClientLayer::create());
    }

    virtual void onExit() {}
};

class ClientApp : public oyl::Application {
public:
    ClientApp() {
        pushScene(ClientScene::create());
    }

    virtual void onExit() {}
};

oyl::Application* oyl::createApplication() {
    return new ClientApp();
}