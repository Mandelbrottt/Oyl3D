#include <Oyl3D.h>

using namespace oyl;

class MainLayer : public Layer
{
public:
    OYL_CTOR(MainLayer, Layer)

    void onEnter() override { }

    void onUpdate() override { }

    bool onEvent(const Event& event) override
    {
        return false;
    }
};

class MainScene : public Scene
{
public:
    OYL_CTOR(MainScene, Scene)

    virtual void onEnter() override
    {
        pushLayer(MainLayer::create());
    }
};

class Game : public oyl::Application
{
public:
    Game()
    {
        pushScene(MainScene::create());
    }

    virtual void onExit() { }
};

oyl::Application* oyl::createApplication()
{
    return new Game();
}
