#include <Oyl3D.h>

#include "SandboxLayer.h"
#include "PlayerSystem.h"
#include "CannonSystem.h"
#include "CustomComponents.h"
#include "CustomEvents.h"
#include "PlayerInteractionValidation.h"
#include "GarbagePileSystem.h"
#include "GarbageTickSystem.h"
#include "UIManagerSystem.h"
#include "CannonballSystem.h"
#include "GloopSystem.h"
#include "CleaningQuicktimeEventSystem.h"

using namespace oyl;

class MainLayer : public Layer
{
public:
	OYL_CTOR(MainLayer, oyl::Layer)

	bool isCameraActive = false;

	void onEnter() override
	{
		srand(time(NULL));

		this->listenForEventCategory(EventCategory::Keyboard);
		this->listenForEventCategory(EventCategory::Mouse);
		this->listenForEventCategory((EventCategory) CategoryGarbagePile);

		scheduleSystemUpdate<PlayerSystem>();
		scheduleSystemUpdate<CannonSystem>();
		scheduleSystemUpdate<PlayerInteractionValidationSystem>();
		scheduleSystemUpdate<GarbagePileSystem>();
		scheduleSystemUpdate<GarbageTickSystem>();
		scheduleSystemUpdate<UIManagerSystem>();
		scheduleSystemUpdate<CannonballSystem>();
		scheduleSystemUpdate<GloopSystem>();
		scheduleSystemUpdate<CleaningQuicktimeEventSystem>();

		{
			auto e = registry->create();

            auto& t = registry->assign<component::Transform>(e);
            t.setPosition(glm::vec3(0.0f, -2.5f, 0.0f));
            t.setScale(glm::vec3(1.0f, 1.0f, 1.0f));

			auto& uiType = registry->assign<PlayerInteractionType>(e);
			uiType.type = PlayerInteractionResult::pushCannon;

			auto& HUDElement = registry->assign<PlayerHUDElement>(e);
			HUDElement.positionWhenActive = glm::vec3(0.0f, 2.0f, 0.0f);
			HUDElement.playerNum = 0;

            auto& so = registry->assign<component::EntityInfo>(e);
            so.name = "Push Cannon Message";
            
            auto& gui = registry->assign<component::GuiRenderable>(e);
            gui.texture = Texture2D::cache("res/assets/textures/gui/pushCannon.png");
        }

        {
            auto e = registry->create();

            auto& t = registry->assign<component::Transform>(e);
            t.setPosition(glm::vec3(0.0f, -2.5f, 0.0f));
            t.setScale(glm::vec3(1.0f, 1.0f, 1.0f));

			auto& uiType = registry->assign<PlayerInteractionType>(e);
			uiType.type = PlayerInteractionResult::loadCannon;

			auto& HUDElement = registry->assign<PlayerHUDElement>(e);
			HUDElement.positionWhenActive = glm::vec3(0.0f, 2.0f, 0.0f);
			HUDElement.playerNum = 0;

            auto& so = registry->assign<component::EntityInfo>(e);
            so.name = "Load Cannon Message";

            auto& gui = registry->assign<component::GuiRenderable>(e);
            gui.texture = Texture2D::cache("res/assets/textures/gui/loadCannon.png"); 
        }

        {
            auto e = registry->create();

            auto& t = registry->assign<component::Transform>(e);
            t.setPosition(glm::vec3(0.0f, -2.5f, 0.0f));
            t.setScale(glm::vec3(2.0f, 2.0f, 1.0f));

			auto& uiType = registry->assign<PlayerInteractionType>(e);
			uiType.type = PlayerInteractionResult::takeCannonballFromCrate;

			auto& HUDElement = registry->assign<PlayerHUDElement>(e);
			HUDElement.positionWhenActive = glm::vec3(0.0f, 2.0f, 0.0f);
			HUDElement.playerNum = 0;

            auto& so = registry->assign<component::EntityInfo>(e);
            so.name = "Cannonball Crate Message";

            auto& gui = registry->assign<component::GuiRenderable>(e);
            gui.texture = Texture2D::cache("res/assets/textures/gui/cannonballCrate.png");
        }

		{
			auto e = registry->create();

			auto& t = registry->assign<component::Transform>(e);
			t.setPosition(glm::vec3(0.0f, -2.5f, 0.0f));
			t.setScale(glm::vec3(0.8f, 0.8f, 1.0f));

			auto& uiType = registry->assign<PlayerInteractionType>(e);
			uiType.type = PlayerInteractionResult::pickUpCannonball;

			auto& HUDElement = registry->assign<PlayerHUDElement>(e);
			HUDElement.positionWhenActive = glm::vec3(0.0f, 2.0f, 0.0f);
			HUDElement.playerNum = 0;

			auto& so = registry->assign<component::EntityInfo>(e);
			so.name = "Pickup Cannonball Message";

			auto& gui = registry->assign<component::GuiRenderable>(e);
			gui.texture = Texture2D::cache("res/assets/textures/gui/pickupCannonball.png");
		}

        {
            auto e = registry->create();

            auto& t = registry->assign<component::Transform>(e);
            t.setPosition(glm::vec3(0.0f, -2.5f, 0.0f));
            t.setScale(glm::vec3(1.0f, 1.0f, 1.0f));

			auto& uiType = registry->assign<PlayerInteractionType>(e);
			uiType.type = PlayerInteractionResult::pickUpMop;

			auto& HUDElement = registry->assign<PlayerHUDElement>(e);
			HUDElement.positionWhenActive = glm::vec3(0.0f, 2.0f, 0.0f);
			HUDElement.playerNum = 0;

            auto& so = registry->assign<component::EntityInfo>(e);
            so.name = "Pickup Mop Message";

            auto& gui = registry->assign<component::GuiRenderable>(e);
            gui.texture = Texture2D::cache("res/assets/textures/gui/pickupMop.png");
        }

		{
			auto e = registry->create();

			auto& t = registry->assign<component::Transform>(e);
			t.setPosition(glm::vec3(0.0f, -2.5f, 0.0f));
			t.setScale(glm::vec3(0.8f, 0.8f, 1.0f));

			auto& uiType = registry->assign<PlayerInteractionType>(e);
			uiType.type = PlayerInteractionResult::pickUpCleaningSolution;

			auto& HUDElement = registry->assign<PlayerHUDElement>(e);
			HUDElement.positionWhenActive = glm::vec3(0.0f, 2.0f, 0.0f);
			HUDElement.playerNum = 0;

			auto& so = registry->assign<component::EntityInfo>(e);
			so.name = "Pickup Cleaning Solution Message";

			auto& gui = registry->assign<component::GuiRenderable>(e);
			gui.texture = Texture2D::cache("res/assets/textures/gui/pickupCleaningSolution.png");
		}

		{
			auto e = registry->create();

			auto& t = registry->assign<component::Transform>(e);
			t.setPosition(glm::vec3(0.0f, -2.5f, 0.0f));
			t.setScale(glm::vec3(1.0f, 1.0f, 1.0f));

			auto& uiType = registry->assign<PlayerInteractionType>(e);
			uiType.type = PlayerInteractionResult::pickUpGloop;

			auto& HUDElement = registry->assign<PlayerHUDElement>(e);
			HUDElement.positionWhenActive = glm::vec3(0.0f, 2.0f, 0.0f);
			HUDElement.playerNum = 0;

			auto& so = registry->assign<component::EntityInfo>(e);
			so.name = "Pickup Gloop Message";

			auto& gui = registry->assign<component::GuiRenderable>(e);
			gui.texture = Texture2D::cache("res/assets/textures/gui/pickUpGloop.png");
		}

		{
			auto e = registry->create();

			auto& t = registry->assign<component::Transform>(e);
			t.setPosition(glm::vec3(0.0f, -2.5f, 0.0f));
			t.setScale(glm::vec3(1.0f, 1.0f, 1.0f));

			auto& uiType = registry->assign<PlayerInteractionType>(e);
			uiType.type = PlayerInteractionResult::useGloop;

			auto& HUDElement = registry->assign<PlayerHUDElement>(e);
			HUDElement.positionWhenActive = glm::vec3(0.0f, 2.0f, 0.0f);
			HUDElement.playerNum = 0;

			auto& so = registry->assign<component::EntityInfo>(e);
			so.name = "Use Gloop Message";

			auto& gui = registry->assign<component::GuiRenderable>(e);
			gui.texture = Texture2D::cache("res/assets/textures/gui/useGloop.png");
		}

        {
            auto e = registry->create();

            auto& t = registry->assign<component::Transform>(e);
            t.setPosition(glm::vec3(0.0f, -2.5f, 0.0f));
            t.setScale(glm::vec3(1.0f, 1.0f, 1.0f));

			auto& uiType = registry->assign<PlayerInteractionType>(e);
			uiType.type = PlayerInteractionResult::cleanGarbagePile;

			auto& HUDElement = registry->assign<PlayerHUDElement>(e);
			HUDElement.positionWhenActive = glm::vec3(0.0f, 2.0f, 0.0f);
			HUDElement.playerNum = 0;

            auto& so = registry->assign<component::EntityInfo>(e);
            so.name = "Clean Garbage Message";

            auto& gui = registry->assign<component::GuiRenderable>(e);
            gui.texture = Texture2D::cache("res/assets/textures/gui/cleanGarbage.png");
        }

        {
            auto e = registry->create();

            auto& t = registry->assign<component::Transform>(e);
            t.setPosition(glm::vec3(0.0f, -2.5f, 0.0f));
            t.setScale(glm::vec3(1.0f, 1.0f, 1.0f));

			auto& uiType = registry->assign<PlayerInteractionType>(e);
			uiType.type = PlayerInteractionResult::cannonFiringSoon;

			auto& HUDElement = registry->assign<PlayerHUDElement>(e);
			HUDElement.positionWhenActive = glm::vec3(0.0f, 2.0f, 0.0f);
			HUDElement.playerNum = 0;

            auto& so = registry->assign<component::EntityInfo>(e);
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

			auto& HUDElement = registry->assign<PlayerHUDElement>(e);
			HUDElement.positionWhenActive = glm::vec3(0.0f, 0.0f, 0.0f);
			HUDElement.playerNum = 0;

			component::Transform t;
			t.setPosition(glm::vec3(-3.0f, -1.0f, -2.0f));
			t.setScale(glm::vec3(0.2f, 0.2f, 1.0f));
			registry->assign<component::Transform>(e, t);

			auto& so = registry->assign<component::EntityInfo>(e);
			so.name = "Normal Reticle";
		}

		{
			entt::entity e = registry->create();
			auto& gui = registry->assign<component::GuiRenderable>(e);
			gui.texture = Texture2D::cache("res/assets/textures/gui/invalid_reticle.png");

			auto& reticle = registry->assign<Reticle>(e);
			reticle.type = ReticleType::invalid;

			auto& HUDElement = registry->assign<PlayerHUDElement>(e);
			HUDElement.positionWhenActive = glm::vec3(0.0f, 0.0f, 0.0f);
			HUDElement.playerNum = 0;

			component::Transform t;
			t.setPosition(glm::vec3(-3.0f, -1.0f, -2.0f));
			t.setScale(glm::vec3(0.5f, 0.5f, 1.0f));
			registry->assign<component::Transform>(e, t);

			auto& so = registry->assign<component::EntityInfo>(e);
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

			auto& so = registry->assign<component::EntityInfo>(e);
			so.name = "Win Screen";
		}

		{
			entt::entity e = registry->create();
			auto& gui = registry->assign<component::GuiRenderable>(e);
			gui.texture = Texture2D::cache("res/assets/textures/gui/YouLose.jpg");

			auto& endScreen = registry->assign<EndScreen>(e);
			endScreen.isLoseScreen = true;

			component::Transform loseScreenTransform;
			loseScreenTransform.setPosition(glm::vec3(-100.0f, -100.0f, -200.0f));
			registry->assign<component::Transform>(e, loseScreenTransform);

			auto& so = registry->assign<component::EntityInfo>(e);
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

				auto& so = registry->assign<component::EntityInfo>(e);
				so.name = "Garbage Tick" + std::to_string(i);
			}
		}

		{
			//background for cleaning quicktime event
			entt::entity cleaningQuicktimeEventBackgroundEntity = registry->create();
			auto& backgroundGUI = registry->assign<component::GuiRenderable>(cleaningQuicktimeEventBackgroundEntity);
			backgroundGUI.texture = Texture2D::cache("res/assets/textures/gui/cleaningQuicktimeEventBackground.png");

			component::Transform backgroundTransform;
			backgroundTransform.setPosition(glm::vec3(-30.0f, 3.5f, 0.0f));
			backgroundTransform.setScale(glm::vec3(2.0f, 1.5f, 1.0f));
			registry->assign<component::Transform>(cleaningQuicktimeEventBackgroundEntity, backgroundTransform);

			auto& backgroundHUDElement = registry->assign<PlayerHUDElement>(cleaningQuicktimeEventBackgroundEntity);
			backgroundHUDElement.positionWhenActive = glm::vec3(0.0f, 3.5f, 0.0f);
			backgroundHUDElement.playerNum = 0;

			auto& backgroundSceneObject = registry->assign<component::EntityInfo>(cleaningQuicktimeEventBackgroundEntity);
			backgroundSceneObject.name = "Cleaning Quicktime Event Background";



			//indicator for cleaning quicktime event
			entt::entity cleaningQuicktimeEventIndicatorEntity = registry->create();
			auto& indicatorGUI = registry->assign<component::GuiRenderable>(cleaningQuicktimeEventIndicatorEntity);
			indicatorGUI.texture = Texture2D::cache("res/assets/textures/gui/cleaningQuicktimeEventIndicator.png");

			component::Transform indicatorTransform;
			indicatorTransform.setPosition(glm::vec3(-30.0f, 3.6f, -1.0f));
			indicatorTransform.setScale(glm::vec3(1.5f, 2.0f, 1.0f));
			registry->assign<component::Transform>(cleaningQuicktimeEventIndicatorEntity, indicatorTransform);

			CleaningQuicktimeEventIndicator cleaningQuicktimeEventIndicator;
			cleaningQuicktimeEventIndicator.lerpInformation.startPos = glm::vec3(-4.95f, 3.6f, -1.0f);
			cleaningQuicktimeEventIndicator.lerpInformation.destinationPos = glm::vec3(4.95f, 3.6f, -1.0f);
			cleaningQuicktimeEventIndicator.lerpInformation.speed = 1.0f;

			cleaningQuicktimeEventIndicator.cleaningQuicktimeEventBackground = cleaningQuicktimeEventBackgroundEntity;
			registry->assign<CleaningQuicktimeEventIndicator>(cleaningQuicktimeEventIndicatorEntity, cleaningQuicktimeEventIndicator);

			auto& indicatorHUDElement = registry->assign<PlayerHUDElement>(cleaningQuicktimeEventIndicatorEntity);
			indicatorHUDElement.positionWhenActive = glm::vec3(-4.95f, 3.6f, -1.0f);
			indicatorHUDElement.playerNum = 0;

			auto& indicatorSceneObject = registry->assign<component::EntityInfo>(cleaningQuicktimeEventIndicatorEntity);
			indicatorSceneObject.name = "Cleaning Quicktime Event Indicator";
		}
	}

	void onUpdate() override
	{
		auto view = registry->view<Player, component::Transform>();
		for (auto& entity : view)
		{
			auto& player = registry->get<Player>(entity);
			auto& playerTransform = registry->get<component::Transform>(entity);

			glm::vec3 desiredMoveDirection = glm::vec3(0.0f);

			if (Input::isKeyPressed(Key::W))
				desiredMoveDirection += playerTransform.getForward();

			if (Input::isKeyPressed(Key::S))
				desiredMoveDirection += -playerTransform.getForward();

			if (Input::isKeyPressed(Key::A))
				desiredMoveDirection += -playerTransform.getRight();

			if (Input::isKeyPressed(Key::D))
				desiredMoveDirection += playerTransform.getRight();

		    //check if it's 0 because if we normalize a vector with 0 magnitude it breaks
		    if (desiredMoveDirection == glm::vec3(0.0f))
			    player.moveDirection = desiredMoveDirection;
			else
				player.moveDirection = glm::normalize(desiredMoveDirection);
		}
	}

	bool onEvent(const Event& event) override
	{
		switch (event.type)
		{
		case EventType::KeyReleased:
		{
			Window& window = oyl::Application::get().getWindow();

			auto e = event_cast<KeyReleasedEvent>(event);
			if (e.keycode == oyl::Key::F11)
			{
				// TODO: Make Event Request
				if (window.getWindowState() == oyl::WindowState::Windowed)
					window.setWindowState(oyl::WindowState::Fullscreen);
				else
					window.setWindowState(oyl::WindowState::Windowed);
			}
			else if (e.keycode == oyl::Key::F7)
			{
				window.setVsync(!window.isVsync());
			}
			else if (e.keycode == oyl::Key::C)
			{
				isCameraActive = !isCameraActive;

				CursorStateRequestEvent cursorRequest;
				cursorRequest.state = isCameraActive ? CursorState::Disabled : CursorState::Normal;
				postEvent(cursorRequest);
			}

			break;
		}
		case EventType::KeyPressed:
		{
			Window& window = oyl::Application::get().getWindow();

			auto evt = event_cast<KeyPressedEvent>(event);

			switch (evt.keycode)
			{
			case oyl::Key::E:
			{
				auto playerView = registry->view<Player>();
				for (entt::entity playerEntity : playerView)
				{
					PlayerInteractionRequestEvent playerInteractionRequest;
					playerInteractionRequest.playerEntity = playerEntity;
					postEvent(playerInteractionRequest);
				}

				break;
			}
			case oyl::Key::F:
			{
				auto playerView = registry->view<Player>();
				for (entt::entity playerEntity : playerView)
				{
					PlayerDropItemEvent playerDropItem;
					playerDropItem.playerEntity = playerEntity;
					postEvent(playerDropItem);
				}

				break;
			}
			case oyl::Key::G:
			{
				//G key changes player's teams for debugging TODO: remove for working version
				auto playerView = registry->view<Player>();
				for (entt::entity playerEntity : playerView)
				{
					auto& player = registry->get<Player>(playerEntity);

					if (player.team == Team::blue)
					{
						player.team = Team::red;
						std::cout << "SWITCHED TO RED TEAM\n";
					}
					else
					{
						player.team = Team::blue;
						std::cout << "SWITCHED TO BLUE TEAM\n";
					}
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
		case EventType::MouseMoved:
		{
			if (isCameraActive)
			{
				auto evt = event_cast<MouseMovedEvent>(event);

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
		/*case TypeTotalGarbageCount:
		{
			auto evt = (TotalGarbageCountEvent)* event;
			if (evt.totalGarbageCount >= 15)
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

					if (endScreen.isLoseScreen)
					{
						renderableTransform.setPosition(glm::vec3(0.0f, 0.0f, 0.0f));
					}
				}
			}

			break;
		}*/
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