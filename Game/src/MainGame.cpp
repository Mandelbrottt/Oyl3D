#include <Oyl3D.h>

using namespace oyl;

class MainLayer : public Layer
{
public:
    OYL_CTOR(MainLayer, Layer)

    void onEnter() override
    {
        listenForEventType(EventType::KeyReleased);
        listenForEventType(EventType::PhysicsCollisionEnter);
        listenForEventType(EventType::PhysicsCollisionExit);
        listenForEventType(EventType::PhysicsCollisionStay);

        {
            auto e = registry->create();

            auto& l = registry->assign<component::PointLight>(e);
            l.ambient = { 0.3f, 0.3f, 0.3f };
            l.diffuse = { 1.0f, 1.0f, 1.0f };
            l.specular = { 1.0f, 1.0f, 1.0f };
            
            auto& so = registry->assign<component::EntityInfo>(e);
            so.name = "Light 1";
        }
        {
            auto e = registry->create();

            auto& camera = registry->assign<component::PlayerCamera>(e);
            camera.player = PlayerNumber::One;
            camera.skybox = TextureCubeMap::get(DEFAULT_SKYBOX_ALIAS);
            
            auto& so = registry->assign<component::EntityInfo>(e);
            so.name = "Player Camera";
        }
        {
            entt::entity e = registry->create();

            auto& so = registry->assign<component::EntityInfo>(e);
            so.name = "Container";

            auto& rb = registry->assign<component::RigidBody>(e);
            rb.setMass(0.0f);
            rb.setFriction(5.0f);
            rb.setProperties(component::RigidBody::FREEZE_ROTATION_X |
                             component::RigidBody::FREEZE_ROTATION_Y |
                             component::RigidBody::FREEZE_ROTATION_Z, true);

            rb.setProperties(component::RigidBody::IS_KINEMATIC, true);

            //rb.setProperties(component::RigidBody::DETECT_COLLISIONS, false);

            auto& cl = registry->assign<component::Collidable>(e);

            auto& shi = cl.pushShape(ColliderType::Box); 
            shi.box.setSize({ 1.0f, 1.0f, 1.0f });
        }
        //{
        //    auto e = registry->create();
        //    auto& gr = registry->assign<component::GuiRenderable>(e);
        //    gr.texture = Texture2D::get("archer");
        //}
    }

    void onUpdate() override
    {
        using component::EntityInfo;
        using component::Transform;
        using component::RigidBody;
        auto view = registry->view<EntityInfo>();
        for (auto entity : view)
        {
            if (view.get(entity).name == "Capsule")
            {
                auto& transform = registry->get<Transform>(entity);
                auto& rigidbody = registry->get<RigidBody>(entity);

                if (Input::isKeyPressed(Key::W))
                    rigidbody.addImpulse(transform.getForwardGlobal());
                if (Input::isKeyPressed(Key::S))
                    rigidbody.addImpulse(-transform.getForwardGlobal());
                if (Input::isKeyPressed(Key::A))
                    rigidbody.addImpulse(-transform.getRightGlobal());
                if (Input::isKeyPressed(Key::D))
                    rigidbody.addImpulse(transform.getRightGlobal());
                if (Input::isKeyPressed(Key::Space))
                    rigidbody.addImpulse(transform.getUpGlobal());
            }
        }
    }

    bool onEvent(const Event& event) override
    {
        switch (event.type)
        {
            case EventType::KeyReleased:
            {
                Window& window = Application::get().getWindow();

                auto e = event_cast<KeyReleasedEvent>(event);
                if (e.keycode == Key::F11)
                {
                    // TODO: Make Event Request
                    if (window.getWindowState() == WindowState::Windowed)
                        window.setWindowState(WindowState::Fullscreen);
                    else
                        window.setWindowState(WindowState::Windowed);
                }
                else if (e.keycode == Key::F7)
                {
                    window.setVsync(!window.isVsync());
                }
                break;
            }
        }
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
