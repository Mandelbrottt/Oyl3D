#include <Oyl3D.h>

#include <Oyl3D/ECS/System.h>

class OtherSystem : public oyl::ECS::System
{
    virtual void onUpdate(oyl::Timestep dt) override
    {
        static int counter = 0;
        if (counter++ >= 10)
        {
            postEvent(oyl::UniqueRef<oyl::WindowFocusEvent>::create(1));
            counter = 0;
        }
    }
};

class PhysicsSystem : public oyl::ECS::System //, oyl::EventListener
{
    virtual bool onEvent(oyl::Ref<oyl::Event> e) override
    {
        OYL_LOG("IT WORKS");
        return false;
    }
};

class MainLayer : public oyl::Layer
{
public:
    OYL_CREATE_FUNC(MainLayer)

    MainLayer()
        : Layer("Main")
    {
    }

    virtual void onAttach() override
    {
        scheduleSystemUpdate<PhysicsSystem>();
        scheduleSystemUpdate<OtherSystem>();
        // TODO: Internally registerForEvents(eventsBitMask);
    }

    virtual void onDetach() override
    {
    }

    virtual void onUpdate(oyl::Timestep dt) override
    {
    }

    virtual void onGuiRender() override
    {
    }

    virtual bool onEvent(oyl::Ref<oyl::Event> e) override
    {
        return false;
    }

private:

};

class MainScene : public oyl::Scene
{
public:
    OYL_CREATE_FUNC(MainScene)

    MainScene()
        : oyl::Scene("MainScene")
    {
    }

    virtual ~MainScene() = default;

    virtual void onEnter() override
    {
        m_mainLayer = MainLayer::create();
        pushLayer(m_mainLayer);
    }

    virtual void onExit() override
    {
        popLayer(m_mainLayer);
    }

private:
    oyl::Ref<MainLayer> m_mainLayer;
};

class Game : public oyl::Application
{
public:
    Game()
    {
        pushScene(MainScene::create());
    }

    virtual void onExit()
    {
    }

public:
    OYL_CREATE_FUNC(MainScene)
};

oyl::Application* oyl::createApplication()
{
    return new Game();
}
