#include <Oyl3D.h>

#include "SandboxLayer.h"
#include "Cannon.h"
#include "Player.h"

using namespace oyl;

class MainLayer : public Layer 
{
public:
	OYL_CTOR(MainLayer, Layer)

	bool isCameraActive = false;

    void onEnter() override
    {
		addToCategoryMask(CategoryKeyboard);
		addToCategoryMask(CategoryMouse);

		scheduleSystemUpdate<PlayerSystem>();
		scheduleSystemUpdate<CannonSystem>();
        
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
		auto view = registry->view<Cannon, component::Transform, component::SceneObject>();
        for (entt::entity e : view)
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
				else if (e.keycode == oyl::Key_C)
				{
					isCameraActive = !isCameraActive;

					isCameraActive ? Cursor_Enabled : Cursor_Disabled;
				}
                
				break;
            }
			case TypeKeyPressed:
			{
				Window& window = oyl::Application::get().getWindow();

				auto evt = (oyl::KeyPressedEvent)* event;

				switch (evt.keycode)
				{
				    //TODO: Remove U and I keypress events, they're just for testing
				    case oyl::Key_U:
					{
						auto cannonView = registry->view<Cannon, component::Transform, component::SceneObject>();
						for (entt::entity e : cannonView)
						{
							component::Transform& cannonTransform = cannonView.get<component::Transform>(e);

							if (cannonView.get<component::SceneObject>(e).name == "BlueCannon" && cannonView.get<Cannon>(e).state == CannonStates::doingNothing)
							{
								cannonTransform.setPosition(cannonTransform.getPosition() += glm::vec3(-1.0f, 0.0f, 0.0f));
							}
						}
				        
						break;
					}
					case oyl::Key_I:
					{
						auto cannonView = registry->view<Cannon, component::Transform, component::SceneObject>();
						for (entt::entity e : cannonView)
						{
							component::Transform& cannonTransform = cannonView.get<component::Transform>(e);

							if (cannonView.get<component::SceneObject>(e).name == "BlueCannon" && cannonView.get<Cannon>(e).state == CannonStates::doingNothing)
							{
								cannonTransform.setPosition(cannonTransform.getPosition() += glm::vec3(1.0f, 0.0f, 0.0f));
							}
						}

						break;
					}
					case oyl::Key_E:
					{
						auto cannonView = registry->view<Cannon, component::Transform, component::SceneObject>();

						for (entt::entity e : cannonView)
						{
							if (cannonView.get<component::SceneObject>(e).name == "BlueCannon" && cannonView.get<Cannon>(e).state == CannonStates::doingNothing)
							{
								CannonInteractEvent cannonEvent;
								cannonEvent.cannon = e;
								postEvent(Event::create(cannonEvent));
							}
						}

						break;
					}
					case oyl::Key_W:
					{
						auto playerView = registry->view<Player, component::Transform, component::RigidBody, component::SceneObject>();

						for (entt::entity e : playerView)
						{
							PlayerMoveEvent playerMove;
							playerMove.player = e;
							playerMove.direction = playerView.get<component::Transform>(e).getForward();
							postEvent(Event::create(playerMove));
						}

						break;
					}
					case oyl::Key_A:
					{
						auto playerView = registry->view<Player, component::Transform, component::RigidBody, component::SceneObject>();

						for (entt::entity e : playerView)
						{
							PlayerMoveEvent playerMove;
							playerMove.player = e;
							playerMove.direction = -playerView.get<component::Transform>(e).getRight();
							postEvent(Event::create(playerMove));
						}

						break;
					}
					case oyl::Key_S:
					{
						auto playerView = registry->view<Player, component::Transform, component::RigidBody, component::SceneObject>();

						for (entt::entity e : playerView)
						{
							PlayerMoveEvent playerMove;
							playerMove.player = e;
							playerMove.direction = -playerView.get<component::Transform>(e).getForward();
							postEvent(Event::create(playerMove));
						}

						break;
					}
					case oyl::Key_D:
					{
						auto playerView = registry->view<Player, component::Transform, component::RigidBody, component::SceneObject>();
				        
						for (entt::entity e : playerView)
						{
							PlayerMoveEvent playerMove;
							playerMove.player = e;
							playerMove.direction = playerView.get<component::Transform>(e).getRight();
							postEvent(Event::create(playerMove));
						}

						break;
					}
				}
			}
            case TypeMouseMoved:
			{
				if (isCameraActive)
				{
					auto evt = (MouseMovedEvent)* event;
				    
					auto playerView = registry->view<Player, component::Transform>();
					for (auto& entity : playerView)
					{
						component::Transform& playerTransform = registry->get<component::Transform>(entity);

						playerTransform.setRotationEulerY(playerTransform.getRotationEulerY() - evt.dx * 0.5f);
					}
				    
					auto playerCameraView = registry->view<component::PlayerCamera, component::Transform>();
					for (auto& entity : playerCameraView)
					{
						component::Transform& cameraTransform = registry->get<component::Transform>(entity);
					    
						cameraTransform.setRotationEulerX(cameraTransform.getRotationEulerX() - evt.dy * 0.5f);
					}
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