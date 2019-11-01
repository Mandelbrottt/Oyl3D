#include <Oyl3D.h>

#include "SandboxLayer.h"

using namespace oyl;

class MainLayer : public Layer
{
public:
    OYL_CTOR(MainLayer, Layer)

    void onEnter() override
    {
        auto lightShader = Shader::get(LIGHTING_SHADER_ALIAS);

        Material::cache(lightShader, nullptr, "monkeyMat");

        auto textureShader = Shader::get(TEXTURE_SHADER_ALIAS);
        auto uv = Texture2D::get(UV_TEXTURE_ALIAS);
        
        Material::cache(textureShader, uv, "cubeMat");

        {
            auto e = registry->create();
            registry->assign<component::Transform>(e);
            auto& camera = registry->assign<component::PlayerCamera>(e);
            camera.player = 0;
            camera.projection = glm::perspective(glm::radians(60.0f), 16.0f / 9.0f, 0.01f, 1000.0f);
            auto& so = registry->assign<component::SceneObject>(e);
            so.name = "Player Camera";
        }
    }

    bool onEvent(Ref<Event> event) override
    {
        switch (event->type)
        {
            case TypeKeyReleased:
            {
                Window& window = oyl::Application::get().getWindow();

                auto e = (oyl::KeyReleasedEvent) *event;
                if (e.keycode == oyl::Key_F11)
                {
                    // TODO: Make Event Request
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

    virtual void onExit()
    {
    }
};

oyl::Application* oyl::createApplication()
{
    return new Game();
}
