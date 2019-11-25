#include <Oyl3D.h>

#include "SandboxLayer.h"

using namespace oyl;

class MainLayer : public Layer
{
public:
    OYL_CTOR(MainLayer, Layer)

    void onEnter() override
    {
        listenForEventType(TypeKeyReleased);
        
        auto lightShader = Shader::get(LIGHTING_SHADER_ALIAS);

        Material::cache(lightShader, "monkeyMat");

        auto textureShader = Shader::get(TEXTURE_SHADER_ALIAS);
        
        auto& cubeMat = Material::cache(textureShader, "cubeMat");
        cubeMat->albedoMap = Texture2D::get(UV_TEXTURE_ALIAS);
        
        // auto animShader = Shader::get("animation");
        // Material::cache(animShader, uv, "animationMat");

        {
            auto e = registry->create();
            registry->assign<component::Transform>(e);
            auto& camera = registry->assign<component::PlayerCamera>(e);
            camera.player = 0;
            camera.projection = glm::perspective(glm::radians(60.0f), 16.0f / 9.0f, 0.01f, 1000.0f);
            auto& so = registry->assign<component::SceneObject>(e);
            so.name = "Player Camera";
        }

        // TEMPORARY:
        // {
        //     auto e = registry->create();
            
        //     auto& t = registry->assign<component::Transform>(e);
        //     t.setPosition(glm::vec3(3.0f, 3.0f, 3.0f));

        //     auto& so = registry->assign<component::SceneObject>(e);
        //     so.name = "Animation Object";

        //     auto& mr = registry->assign<component::Renderable>(e);
        //     mr.mesh = Mesh::cache("res/assets/models/agony/agony1_000001.obj");
        //     mr.material = Material::get("animationMat");

        //     char filename[512];

        //     std::string s;
        //     s.reserve(512);
            
        //     auto& an = registry->assign<component::Animation>(e);
        //     for (int i = 0; i < 110; i++)
        //     {
        //         component::Animation::KeyPose kp;
        //         kp.duration = 1.0f / 30.0f;
        //         //kp.duration = 1.0f;

        //         sprintf(filename, "res/assets/models/agony/agony1_%06d.obj", i + 1);
        //         s.assign(filename);
                
        //         kp.mesh = Mesh::cache(s);
        //         an.poses.push_back(kp);
        //     }
        // }
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

    virtual void onExit() { }
};

oyl::Application* oyl::createApplication()
{
    return new Game();
}
