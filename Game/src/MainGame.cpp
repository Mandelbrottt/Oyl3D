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
        listenForEventType(EventType::PhysicsCollisionStay);
        
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
            }
            case EventType::GamepadConnected:
            {
                auto ev = event_cast<GamepadConnectedEvent>(event);
                if (ev.gid == 1)
                {
                    auto e = registry->create();
                    registry->assign<component::Transform>(e);

                    auto& camera = registry->assign<component::PlayerCamera>(e);
                    camera.player = 1;
                    camera.skybox = TextureCubeMap::get(DEFAULT_SKYBOX_ALIAS);

                    auto& so = registry->assign<component::EntityInfo>(e);
                    so.name = "Player Camera 2";
                }
                break;
            }
            case EventType::GamepadDisconnected:
            {
                auto ev = event_cast<GamepadDisconnectedEvent>(event);
                auto view = registry->view<component::PlayerCamera>();
                for (auto pc : view)
                {
                    if (ev.gid != 0 && 
                        view.get(pc).player == ev.gid)
                    {
                        registry->destroy(pc);
                        break;
                    }
                }
                break;
            }
            case EventType::PhysicsCollisionStay:
            {
                auto ev = event_cast<PhysicsCollisionStayEvent>(event);
                
                component::EntityInfo& info1 = registry->get<component::EntityInfo>(ev.entity1);
                component::EntityInfo& info2 = registry->get<component::EntityInfo>(ev.entity2);

                OYL_LOG_INFO("Entity \"{0}\" Collided with Entity \"{1}\"", info1.name, info2.name);

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
