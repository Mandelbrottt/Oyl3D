#include <Oyl3D.h>

#include "SandboxLayer.h"

using namespace oyl;

class MainLayer : public Layer
{
public:
    OYL_CTOR(MainLayer, Layer)

    void onEnter() override
    {
        listenForEventType(EventType::KeyReleased);
        listenForEventType(EventType::GamepadConnected);
        listenForEventType(EventType::GamepadDisconnected);

        listenForEventType(EventType::PhysicsTriggerEnter);
        listenForEventType(EventType::PhysicsTriggerStay);
        listenForEventType(EventType::PhysicsTriggerExit);

        listenForEventType(EventType::PhysicsCollisionEnter);
        listenForEventType(EventType::PhysicsCollisionStay);
        listenForEventType(EventType::PhysicsCollisionExit);
        
        auto lightShader = Shader::get(TEXTURE_SHADER_ALIAS);

        auto& lightMat = Material::cache(lightShader, "monkeyMat");
        lightMat->albedoMap = Texture2D::get(WHITE_TEXTURE_ALIAS);
        
        auto& cubeMat = Material::cache(lightShader, "cubeMat");
        cubeMat->albedoMap = Texture2D::get(UV_TEXTURE_ALIAS);
        
        auto animShader = Shader::get("animation");
        auto& animMat = Material::cache(animShader, "animationMat");
        animMat->albedoMap = Texture2D::cache("res/assets/textures/archer.png");
        
        {
            auto e = registry->create();
            registry->assign<component::Transform>(e);

            auto& camera = registry->assign<component::PlayerCamera>(e);
            camera.player = 0;
            camera.skybox = TextureCubeMap::get(DEFAULT_SKYBOX_ALIAS);
            
            auto& so = registry->assign<component::EntityInfo>(e);
            so.name = "Player Camera";
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
            case EventType::PhysicsTriggerEnter:
            {
                auto ev = event_cast<PhysicsCollisionStayEvent>(event);

                component::EntityInfo& info1 = registry->get<component::EntityInfo>(ev.entity1);
                component::EntityInfo& info2 = registry->get<component::EntityInfo>(ev.entity2);

                OYL_LOG_INFO("Entity \"{0}\" Started Colliding with Entity \"{1}\"", info1.name, info2.name);

                break;
            }
            case EventType::PhysicsTriggerExit:
            {
                auto ev = event_cast<PhysicsCollisionStayEvent>(event);

                component::EntityInfo& info1 = registry->get<component::EntityInfo>(ev.entity1);
                component::EntityInfo& info2 = registry->get<component::EntityInfo>(ev.entity2);

                OYL_LOG_INFO("Entity \"{0}\" Stopped Colliding with Entity \"{1}\"", info1.name, info2.name);

                break;
            }
            case EventType::PhysicsTriggerStay:
            {
                auto ev = event_cast<PhysicsCollisionStayEvent>(event);
                
                component::EntityInfo& info1 = registry->get<component::EntityInfo>(ev.entity1);
                component::EntityInfo& info2 = registry->get<component::EntityInfo>(ev.entity2);

                OYL_LOG_INFO("Entity \"{0}\" Stayed Colliding with Entity \"{1}\"", info1.name, info2.name);

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
        pushLayer(SandboxLayer::create());
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
