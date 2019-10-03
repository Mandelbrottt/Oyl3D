#include <Oyl3D.h>

#include <Oyl3D/ECS/System.h>

enum CustomEvents
{
    TypeTestEvent = oyl::EventTypeCustomStart
};

OYL_EVENT_STRUCT(TestEvent, TypeTestEvent, TypeTestEvent,
                 {
                 int x;
                 int y;
                 float xyz;
                 });

class OtherSystem : public oyl::ECS::System
{
    virtual void onUpdate(oyl::Timestep dt) override
    {
        static int counter = 0;
        if (counter++ >= 60)
        {
            TestEvent test;
            test.x   = rand() % 10;
            test.y   = rand() % 10;
            test.xyz = (float) test.x / (float) test.y;

            postEvent(oyl::Event::create(test));
            counter = 0;
        }
    }
};

class PhysicsSystem : public oyl::ECS::System
{
    virtual void onEnter() override
    {
        addToEventMask(TypeTestEvent);
        addToEventMask(oyl::TypeMousePressed);
    }

    virtual bool onEvent(oyl::Ref<oyl::Event> event) override
    {
        switch (event->type)
        {
            case TypeTestEvent:
            {
                auto e = (TestEvent) *event;
                OYL_LOG("Test Event: {0} {1} {2}", e.x, e.y, e.xyz);
                break;
            }
            case oyl::TypeMousePressed:
            {
                auto e = (oyl::MousePressedEvent) *event;
                OYL_LOG("Mouse Pressed Event: {0} {1}", e.button, e.mods);
                break;
            }
        }

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
