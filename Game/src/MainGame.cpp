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
            camera.projection = glm::perspective(glm::radians(60.0f), 16.0f / 9.0f, 0.01f, 1000.0f);
            camera.skybox = TextureCubeMap::get(DEFAULT_SKYBOX_ALIAS);
            
            auto& so = registry->assign<component::SceneObject>(e);
            so.name = "Player Camera";
        }

        // TEMPORARY:
        {
            auto e = registry->create();
         
            auto& t = registry->assign<component::Transform>(e);
            t.setPosition(glm::vec3(3.0f, 3.0f, 3.0f));
        
            auto& so = registry->assign<component::SceneObject>(e);
            so.name = "Animation Object";
        
            auto& mr = registry->assign<component::Renderable>(e);
            mr.mesh = Mesh::cache("res/assets/models/agony/agony1_000001.obj");
            mr.material = animMat;
        
            char filename[512];
        
            std::string s;
            s.reserve(512);
         
            auto anim1 = Ref<component::Animation>::create();
            auto anim2 = Ref<component::Animation>::create();
        
            int scale = 2;
            
            for (int i = 0; i < 110 / scale; i++)
            {
                component::Animation::KeyPose kp;
                kp.duration = (1.0f / 30.0f) * scale;
        
                sprintf(filename, "res/assets/models/agony/agony1_%06d.obj", i * scale + 1);
                s.assign(filename);
             
                kp.mesh = Mesh::cache(s);
                anim1->poses.push_back(kp);
            }
        
            for (int i = 0; i < 65 / scale; i++)
            {
                component::Animation::KeyPose kp;
                kp.duration = (1.0f / 30.0f) * (float) scale;
        
                sprintf(filename, "res/assets/models/boxing/boxing_%06d.obj", i * scale + 1);
                s.assign(filename);
        
                kp.mesh = Mesh::create(s);
                anim2->poses.push_back(kp);
            }
        
            auto& anim = registry->assign<component::Animator>(e);
        
            anim.pushAnimation("agony", anim1);
            anim.pushAnimation("boxing", anim2);
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
                else if (e.keycode == oyl::Key_M)
                {
                    static bool oneOrTheOther = false;
                    auto  view = registry->view<component::Animator>();
                    auto& anim = view.get(view[0]);
                    
                    if (oneOrTheOther ^= 1)
                        anim.setNextAnimation("boxing");
                    else
                        anim.setNextAnimation("agony");
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
