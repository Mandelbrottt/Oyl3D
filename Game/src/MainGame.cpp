#include <Oyl3D.h>

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
            test.x   = rand() % 9 + 1;
            test.y   = rand() % 9 + 1;
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
        addToCategoryMask(oyl::CategoryKeyboard);
        addToCategoryMask(oyl::CategoryMouse);
        addToCategoryMask(oyl::CategoryGamepad);
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
            case oyl::TypeGamepadConnected:
            case oyl::TypeGamepadDisconnected:
            {
                auto e = (oyl::GamepadConnectedEvent) *event;
                OYL_LOG("Gamepad Connection Event: {0}", e.gid);
                break;
            }
            case oyl::TypeGamepadButtonPressed:
            {
                auto e = (oyl::GamepadButtonPressedEvent) *event;
                OYL_LOG("Gamepad Button Press Event: {0} {1} {2}", e.gid, e.button, e.repeatCount);
                break;
            }
            case oyl::TypeGamepadStickMoved:
            {
                auto e = (oyl::GamepadStickMovedEvent) *event;
                OYL_LOG("Gamepad Stick Moved Event: {0} {1} {2} {3} {4} {5}", e.gid, e.stick, e.x, e.y, e.dx, e.dy);
                break;
            }
            case oyl::TypeGamepadTriggerPressed:
            {
                auto e = (oyl::GamepadTriggerPressedEvent) *event;
                OYL_LOG("Gamepad Trigger Moved Event: {0} {1} {2} {3}", e.gid, e.trigger, e.x, e.dx);
                break;
            }
            case oyl::TypeKeyPressed:
            {
                auto e = (oyl::KeyPressedEvent) *event;
                if (e.repeatCount == 0)
                {
                    oyl::GamepadVibrationEvent newEvent;
                    newEvent.gid = 0;
                    if (e.keycode == oyl::Key_Left)
                    {
                        newEvent.leftTime  = 1.0f;
                        newEvent.leftMotor = 1.0f;
                    }
                    if (e.keycode == oyl::Key_Right)
                    {
                        newEvent.rightTime  = 1.0f;
                        newEvent.rightMotor = 1.0f;
                    }
                    postEvent(oyl::Event::create(newEvent));
                }
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

        addToEventMask(oyl::TypeKeyPressed);
        addToEventMask(oyl::TypeGamepadStickMoved);

        auto shader = oyl::Shader::create({
            { oyl::VertexShader, "../Engine/res/meshShader.vert" },
            { oyl::FragmentShader, "../Engine/res/meshShader.frag" },
        });

        auto mesh = oyl::Mesh::create("res/monkey.obj");

        auto mat = oyl::Material::create(shader);
        mat->loadTexture("res/capsule0.jpg");
        mat->setUniform1i("u_texture", 0);

        for (int i = 0; i < 10; i++)
        {
            oyl::Component::MeshRenderer mr;
            mr.mesh     = mesh;
            mr.material = mat;

            auto e = registry->create();
            registry->assign<oyl::Component::MeshRenderer>(e, mr);
        }
    }

    virtual void onDetach() override
    {
    }

    virtual void onUpdate(oyl::Timestep dt) override
    {
        m_timeSince += dt;

        const auto& view = registry->view<oyl::Component::MeshRenderer, oyl::Component::Transform>();

        float i = 0;

        for (const auto& entity : view)
        {
            auto& transform    = registry->get<oyl::Component::Transform>(entity);
            transform.position = m_translate + i * glm::vec3(0.5f, 0.5f, -1.0f);
            transform.rotation = glm::vec3(glm::degrees(m_timeSince));

            i += 1.0f;
        }
    }

    virtual void onGuiRender() override
    {
    }

    virtual bool onEvent(oyl::Ref<oyl::Event> event) override
    {
        switch (event->type)
        {
            case oyl::TypeKeyReleased:
            {
                oyl::Window& window = oyl::Application::get().getWindow();

                auto e = (oyl::KeyReleasedEvent) *event;
                if (e.keycode == oyl::Key_F11)
                {
                    if (window.getFullscreenType() == oyl::Windowed)
                        window.setFullscreenType(oyl::Fullscreen);
                    else
                        window.setFullscreenType(oyl::Windowed);
                }
                else if (e.keycode == oyl::Key_F7)
                {
                    window.setVsync(!window.isVsync());
                }
            }
            case oyl::TypeGamepadStickMoved:
            {
                auto e = (oyl::GamepadStickMovedEvent) *event;
                if (e.stick == oyl::Gamepad_LeftStick)
                {
                    m_translate = glm::vec3(e.x, e.y, -5.0f);
                }
            }
        }
        return false;
    }

private:

    glm::vec3 m_translate = glm::vec3(0.0f, 0.0f, -5.0f);

    float m_timeSince = 0.0f;
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
