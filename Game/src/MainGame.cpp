#include <Oyl3D.h>

#include "SandboxLayer.h"
#include "Player.h"
#include "Cannon.h"
#include "CustomComponents.h"
#include "CustomEvents.h"
#include "PlayerInteractionValidation.h"
#include "GarbagePileSystem.h"
#include "UIManager.h"

using namespace oyl;

class MainLayer : public Layer
{
public:
	OYL_CTOR(MainLayer, oyl::Layer)

	bool isCameraActive = false;

	void onEnter() override
	{
		addToCategoryMask(CategoryKeyboard);
		addToCategoryMask(CategoryMouse);
		this->listenForEventCategory((OylEnum)CategoryGarbagePile);

		scheduleSystemUpdate<PlayerSystem>();
		scheduleSystemUpdate<CannonSystem>();
		scheduleSystemUpdate<PlayerInteractionValidationSystem>();
		scheduleSystemUpdate<GarbagePileSystem>();
		scheduleSystemUpdate<UIManagerSystem>();

		{
			auto e = registry->create();

            auto& t = registry->assign<component::Transform>(e);
            t.setPosition(glm::vec3(0.0f, -2.5f, 0.0f));
            t.setScale(glm::vec3(4.0f, 4.0f, 1.0f));

			auto& uiType = registry->assign<PlayerInteractionType>(e);
			uiType.type = PlayerInteractionResult::pushCannon;

            auto& so = registry->assign<component::SceneObject>(e);
            so.name = "Push Cannon Message";
            
            auto& gui = registry->assign<component::GuiRenderable>(e);
            gui.texture = Texture2D::cache("res/assets/textures/gui/pushCannon.png");
        }

        {
            auto e = registry->create();

            auto& t = registry->assign<component::Transform>(e);
            t.setPosition(glm::vec3(0.0f, -2.5f, 0.0f));
            t.setScale(glm::vec3(4.0f, 4.0f, 1.0f));

			auto& uiType = registry->assign<PlayerInteractionType>(e);
			uiType.type = PlayerInteractionResult::loadCannon;

            auto& so = registry->assign<component::SceneObject>(e);
            so.name = "Load Cannon Message";

            auto& gui = registry->assign<component::GuiRenderable>(e);
            gui.texture = Texture2D::cache("res/assets/textures/gui/loadCannon.png"); 
        }

        {
            auto e = registry->create();

            auto& t = registry->assign<component::Transform>(e);
            t.setPosition(glm::vec3(0.0f, -2.5f, 0.0f));
            t.setScale(glm::vec3(4.0f, 4.0f, 1.0f));

			auto& uiType = registry->assign<PlayerInteractionType>(e);
			uiType.type = PlayerInteractionResult::pickUpCannonball;

            auto& so = registry->assign<component::SceneObject>(e);
            so.name = "Pickup Cannonball Message";

            auto& gui = registry->assign<component::GuiRenderable>(e);
            gui.texture = Texture2D::cache("res/assets/textures/gui/pickupCannonball.png");
        }

        {
            auto e = registry->create();

            auto& t = registry->assign<component::Transform>(e);
            t.setPosition(glm::vec3(0.0f, -2.5f, 0.0f));
            t.setScale(glm::vec3(4.0f, 4.0f, 1.0f));

			auto& uiType = registry->assign<PlayerInteractionType>(e);
			uiType.type = PlayerInteractionResult::takeCannonballFromCrate;

            auto& so = registry->assign<component::SceneObject>(e);
            so.name = "Cannonball Crate Message";

            auto& gui = registry->assign<component::GuiRenderable>(e);
            gui.texture = Texture2D::cache("res/assets/textures/gui/cannonballCrate.png");
        }

        {
            auto e = registry->create();

            auto& t = registry->assign<component::Transform>(e);
            t.setPosition(glm::vec3(0.0f, -2.5f, 0.0f));
            t.setScale(glm::vec3(4.0f, 4.0f, 1.0f));

			auto& uiType = registry->assign<PlayerInteractionType>(e);
			uiType.type = PlayerInteractionResult::pickUpMop;

            auto& so = registry->assign<component::SceneObject>(e);
            so.name = "Pickup Mop Message";

            auto& gui = registry->assign<component::GuiRenderable>(e);
            gui.texture = Texture2D::cache("res/assets/textures/gui/pickupMop.png");
        }

        {
            auto e = registry->create();

            auto& t = registry->assign<component::Transform>(e);
            t.setPosition(glm::vec3(0.0f, -2.5f, 0.0f));
            t.setScale(glm::vec3(4.0f, 4.0f, 1.0f));

			auto& uiType = registry->assign<PlayerInteractionType>(e);
			uiType.type = PlayerInteractionResult::cleanGarbagePile;

            auto& so = registry->assign<component::SceneObject>(e);
            so.name = "Clean Garbage Message";

            auto& gui = registry->assign<component::GuiRenderable>(e);
            gui.texture = Texture2D::cache("res/assets/textures/gui/cleanGarbage.png");
        }

        {
            auto e = registry->create();

            auto& t = registry->assign<component::Transform>(e);
            t.setPosition(glm::vec3(0.0f, -2.5f, 0.0f));
            t.setScale(glm::vec3(4.0f, 4.0f, 1.0f));

			auto& uiType = registry->assign<PlayerInteractionType>(e);
			uiType.type = PlayerInteractionResult::cannonFiringSoon;

            auto& so = registry->assign<component::SceneObject>(e);
            so.name = "Cannon Firing Message";
            
            auto& gui = registry->assign<component::GuiRenderable>(e);
            gui.texture = Texture2D::cache("res/assets/textures/gui/cannonFiring.png");
        }

		{
			entt::entity e = registry->create();
			auto& gui = registry->assign<component::GuiRenderable>(e);
			gui.texture = Texture2D::cache("res/assets/textures/gui/reticle.png");

			auto& reticle = registry->assign<Reticle>(e);
			reticle.type = ReticleType::normal;

			component::Transform t;
			t.setPosition(glm::vec3(-3.0f, -1.0f, -2.0f));
			registry->assign<component::Transform>(e, t);

			auto& so = registry->assign<component::SceneObject>(e);
			so.name = "Normal Reticle";
		}

		{
			entt::entity e = registry->create();
			auto& gui = registry->assign<component::GuiRenderable>(e);
			gui.texture = Texture2D::cache("res/assets/textures/gui/invalid_reticle.png");

			auto& reticle = registry->assign<Reticle>(e);
			reticle.type = ReticleType::invalid;

			component::Transform t;
			t.setPosition(glm::vec3(-3.0f, -1.0f, -2.0f));
			registry->assign<component::Transform>(e, t);

			auto& so = registry->assign<component::SceneObject>(e);
			so.name = "Invalid Reticle";
		}

		{
			entt::entity e = registry->create();
			auto& gui = registry->assign<component::GuiRenderable>(e);
			gui.texture = Texture2D::cache("res/assets/textures/gui/VictoryScreen.jpg");

			component::Transform winScreenTransform;
			winScreenTransform.setPosition(glm::vec3(-100.0f, -100.0f, -200.0f));
			registry->assign<component::Transform>(e, winScreenTransform);

			auto& endScreen = registry->assign<EndScreen>(e);
			endScreen.isLoseScreen = false;

			auto& so = registry->assign<component::SceneObject>(e);
			so.name = "Win Screen";
		}

		{
			entt::entity e = registry->create();
			auto& gui = registry->assign<component::GuiRenderable>(e);
			gui.texture = Texture2D::cache("res/assets/textures/gui/LoseScreen.jpg");

			auto& endScreen = registry->assign<EndScreen>(e);
			endScreen.isLoseScreen = true;

			component::Transform loseScreenTransform;
			loseScreenTransform.setPosition(glm::vec3(-100.0f, -100.0f, -200.0f));
			registry->assign<component::Transform>(e, loseScreenTransform);

			auto& so = registry->assign<component::SceneObject>(e);
			so.name = "Lose Screen";
		}

		{
			for (int i = 0; i < 3; i++)
			{
				entt::entity e = registry->create();
				auto& gui = registry->assign<component::GuiRenderable>(e);
				gui.texture = Texture2D::cache("res/assets/textures/gui/garbageTick.png");

				auto& garbageTick = registry->assign<GarbageTick>(e);

				component::Transform garbageTickTransform;
				garbageTickTransform.setPosition(glm::vec3(-100.0f, -100.0f, -200.0f));
				registry->assign<component::Transform>(e, garbageTickTransform);

				auto& so = registry->assign<component::SceneObject>(e);
				so.name = "Garbage Tick" + std::to_string(i);
			}
		}

		{
			auto playerCameraEntity = registry->create();
			registry->assign<component::Transform>(playerCameraEntity);
			auto& camera = registry->assign<component::PlayerCamera>(playerCameraEntity);
			camera.player = 0;
			camera.projection = glm::perspective(glm::radians(60.0f), 16.0f / 9.0f, 0.01f, 1000.0f);
			auto& so = registry->assign<component::SceneObject>(playerCameraEntity);
			so.name = "Player Camera";
		}
	}

	void onUpdate(Timestep dt) override
	{
		auto view = registry->view<Player, component::Transform>();
		for (auto& entity : view)
		{
			auto& player = registry->get<Player>(entity);
			auto& playerTransform = registry->get<component::Transform>(entity);

			glm::vec3 desiredMoveDirection = glm::vec3(0.0f);

			if (Input::isKeyPressed(Key_W))
				desiredMoveDirection += playerTransform.getForward();

			if (Input::isKeyPressed(Key_S))
				desiredMoveDirection += -playerTransform.getForward();

			if (Input::isKeyPressed(Key_A))
				desiredMoveDirection += -playerTransform.getRight();

			if (Input::isKeyPressed(Key_D))
				desiredMoveDirection += playerTransform.getRight();

		    //check if it's 0 because if we normalize a vector with 0 magnitude it breaks
		    if (desiredMoveDirection == glm::vec3(0.0f))
			    player.moveDirection = desiredMoveDirection;
			else
				player.moveDirection = glm::normalize(desiredMoveDirection);
		}
	}

	bool onEvent(Ref<Event> event) override
	{
		switch (event->type)
		{
		case TypeKeyReleased:
		{
			Window& window = oyl::Application::get().getWindow();

			auto e = (oyl::KeyReleasedEvent)* event;
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

				CursorStateRequestEvent cursorRequest;
				cursorRequest.state = isCameraActive ? Cursor_Disabled : Cursor_Enabled;
				postEvent(Event::create(cursorRequest));
			}

			break;
		}
		case TypeKeyPressed:
		{
			Window& window = oyl::Application::get().getWindow();

			auto evt = (oyl::KeyPressedEvent)* event;

			switch (evt.keycode)
			{
			case oyl::Key_E:
			{
				auto playerView = registry->view<Player>();

				for (entt::entity playerEntity : playerView)
				{
					PlayerInteractionRequestEvent playerInteractionRequest;
					playerInteractionRequest.playerEntity = playerEntity;
					postEvent(Event::create(playerInteractionRequest));
				}

				break;
			}
			case oyl::Key_F:
			{
				auto playerView = registry->view<Player>();

				for (entt::entity playerEntity : playerView)
				{
					PlayerDropItemEvent playerDropItem;
					playerDropItem.playerEntity = playerEntity;
					postEvent(Event::create(playerDropItem));
				}

				break;
			}
			/*case oyl::Key_W:
			{
				auto playerView = registry->view<Player, component::Transform, component::RigidBody, component::SceneObject>();

				for (entt::entity playerEntity : playerView)
				{
					PlayerMoveEvent playerMove;
					playerMove.player = &registry->get<Player>(playerEntity);
					playerMove.direction = playerView.get<component::Transform>(playerEntity).getForward();
					postEvent(Event::create(playerMove));
				}

				break;
			}
			case oyl::Key_A:
			{
				auto playerView = registry->view<Player, component::Transform, component::RigidBody, component::SceneObject>();

				for (entt::entity playerEntity : playerView)
				{
					PlayerMoveEvent playerMove;
					playerMove.player = &registry->get<Player>(playerEntity);
					playerMove.direction = -playerView.get<component::Transform>(playerEntity).getRight();
					postEvent(Event::create(playerMove));
				}

				break;
			}
			case oyl::Key_S:
			{
				auto playerView = registry->view<Player, component::Transform, component::RigidBody, component::SceneObject>();

				for (entt::entity playerEntity : playerView)
				{
					PlayerMoveEvent playerMove;
					playerMove.player = &registry->get<Player>(playerEntity);
					playerMove.direction = -playerView.get<component::Transform>(playerEntity).getForward();
					postEvent(Event::create(playerMove));
				}

				break;
			}
			case oyl::Key_D:
			{
				auto playerView = registry->view<Player, component::Transform, component::RigidBody, component::SceneObject>();

				for (entt::entity playerEntity : playerView)
				{
					PlayerMoveEvent playerMove;
					playerMove.player = &registry->get<Player>(playerEntity);
					playerMove.direction = playerView.get<component::Transform>(playerEntity).getRight();
					postEvent(Event::create(playerMove));
				}

				break;
			}*/
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
					auto& player = registry->get<Player>(entity);
					auto& playerTransform = registry->get<component::Transform>(entity);

					playerTransform.rotate(glm::vec3(0.0f, -evt.dx * 0.5f, 0.0f));

					if (player.yRotationClamp > 1)
					{
						if (playerTransform.getRotationEulerY() < player.yRotationClamp)
							playerTransform.setRotationEulerY(player.yRotationClamp);
					}
					else if (player.yRotationClamp < -1)
					{
						if (playerTransform.getRotationEulerY() > player.yRotationClamp)
							playerTransform.setRotationEulerY(player.yRotationClamp);
					}
				}

				auto playerCameraView = registry->view<component::PlayerCamera, component::Transform>();
				for (auto& entity : playerCameraView)
				{
					component::Transform& cameraTransform = registry->get<component::Transform>(entity);

					cameraTransform.rotate(glm::vec3(-evt.dy * 0.5f, 0.0f, 0.0f));

				    //clamp camera up/down rotation
					float cameraRotationClampValue = 70.0f;
				    
				    if (cameraTransform.getRotationEulerX() > cameraRotationClampValue)
						cameraTransform.setRotationEulerX(cameraRotationClampValue);
					else if (cameraTransform.getRotationEulerX() < -cameraRotationClampValue)
						cameraTransform.setRotationEulerX(-cameraRotationClampValue);
				}
			}

			break;
		}
		case TypeTotalGarbageCount:
		{
			auto evt = (TotalGarbageCountEvent)* event;
			if (evt.totalGarbageCount > 1)
			{
				auto guiView = registry->view<component::GuiRenderable, component::Transform>();
				for (auto& guiEntity : guiView)
				{
					auto& renderableTransform = registry->get<component::Transform>(guiEntity);
					renderableTransform.setPosition(glm::vec3(100.0f, 100.0f, 100.0f));
				}

				auto endScreensView = registry->view<component::GuiRenderable, component::Transform, EndScreen>();
				for (auto& endScreenEntity : endScreensView)
				{
					auto& renderableTransform = registry->get<component::Transform>(endScreenEntity);
					auto& endScreen = registry->get<EndScreen>(endScreenEntity);

					renderableTransform.setPosition(glm::vec3(100.0f, 100.0f, 100.0f));

					if (endScreen.isLoseScreen);
					{
						renderableTransform.setPosition(glm::vec3(0.0f, 0.0f, 0.0f));
					}
				}
			}

			break;
		}
		}
		return false;
	}
};

class MainScene : public oyl::Scene
{
public:
	OYL_CTOR(MainScene, oyl::Scene)

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