#include <Oyl3D.h>

#include "SandboxLayer.h"
#include "Cannon.h"

using namespace oyl;

class MainLayer : public Layer 
{
public:
    OYL_CTOR(MainLayer, Layer)

    void onEnter() override
    {
		this->addToCategoryMask(CategoryKeyboard);
        
        auto lightShader = Shader::get(LIGHTING_SHADER_ALIAS);

        Material::cache(lightShader, nullptr, "monkeyMat");

        auto textureShader = Shader::get(TEXTURE_SHADER_ALIAS);
        auto uv = Texture2D::get(UV_TEXTURE_ALIAS);
        
        Material::cache(textureShader, uv, "cubeMat");
    }

    bool onEvent(Ref<Event> event) override
    {
		auto view = registry->view<Cannon, component::Transform, component::SceneObject>();
        for (Entity e : view)
        {
			switch (event->type)
			{
			    case TypeKeyReleased:
			    {
				    Window& window = oyl::Application::get().getWindow();

				    auto evt = (oyl::KeyReleasedEvent)* event;
				    if (evt.keycode == oyl::Key_F11)
				    {
					    if (window.getFullscreenType() == oyl::Windowed)
						    window.setFullscreenType(oyl::Fullscreen);
					    else
						    window.setFullscreenType(oyl::Windowed);
				    }
				    else if (evt.keycode == oyl::Key_F7)
				    {
					    window.setVsync(!window.isVsync());
				    }
			    }
			    case TypeKeyPressed:
			    {
				    Window& window = oyl::Application::get().getWindow();
			        
				    auto evt = (oyl::KeyPressedEvent)* event;
				    if (evt.keycode == oyl::Key_A)
				    {
                        if (view.get<component::SceneObject>(e).name == "BlueCannon" && view.get<Cannon>(e).state == CannonStates::doingNothing)
                        {
							view.get<component::Transform>(e).position += glm::vec3(-1.0f, 0.0f, 0.0f);
                        }
				    }
					else if (evt.keycode == oyl::Key_D)
					{
						if (view.get<component::SceneObject>(e).name == "BlueCannon" && view.get<Cannon>(e).state == CannonStates::doingNothing)
						{
							view.get<component::Transform>(e).position += glm::vec3(1.0f, 0.0f, 0.0f);
						}
					}
					else if (evt.keycode == oyl::Key_E)
					{
						if (view.get<component::SceneObject>(e).name == "BlueCannon" && view.get<Cannon>(e).state == CannonStates::doingNothing)
						{
							CannonInteractEvent cannonEvent;
							cannonEvent.cannon = e;
							postEvent(Event::create(cannonEvent));
						}
					}
			    }
			    //TODO: for cannon interaction
			    //raycast hit event
			        //make sure player is on correct team
			            //make sure player and cannon arent already occupied
			                //check rotation so we know what side is left and right (if (rotation == 180))
			                    //check which side of the cannon the player is on by comparing positions AND make sure the cannon isnt already at the end of the track
			                        //show UI for player being able to push the cannon
			                        //check for correct keypress
			                            //set the player and cannon states to pushing (for final implementation, theyll go into adjusting position and waiting first)
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
