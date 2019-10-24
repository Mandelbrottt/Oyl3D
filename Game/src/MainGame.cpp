#include <Oyl3D.h>

#include "SandboxLayer.h"

using namespace oyl;

class MainLayer : public Layer
{
public:
    OYL_CTOR(MainLayer, Layer)

    void onEnter() override
    {
        auto light = Shader::cache(
            {
                { VertexShader, "../Engine/res/staticLighting.vert" },
                { FragmentShader, "../Engine/res/staticLighting.frag" },
            }, "lighting");

        auto tex = Shader::cache(
            {
                { VertexShader, "../Engine/res/meshShader.vert" },
                { FragmentShader, "../Engine/res/meshShader.frag" },
            }, "texture");

        auto mesh = Mesh::cache("res/monkey.obj");

        auto mat = Material::cache(light, nullptr, "monkeyMat");

        auto cube = Mesh::cache("res/cube.obj");
        auto cap = Texture2D::cache("res/capsule0.jpg");
        Material::cache(tex, cap, "cubeMat");
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

    virtual ~MainScene() = default;

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
