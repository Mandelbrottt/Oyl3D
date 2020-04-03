#include <Oyl3D.h>

#include "PersistentVariables.h"

#include "MainGameObjectsInit.h"
#include "MainGameGuiInit.h"
#include "ScrollingTextureLayer.h"

#include "SplashScreen.h"
#include "MainMenu.h"
#include "Tutorial.h"
#include "ControlsScreenScene.h"
#include "SettingsMenu.h"
#include "GameEndMenu.h"

#include "PlayerSystem.h"
#include "CannonSystem.h"
#include "CustomComponents.h"
#include "CustomEvents.h"
#include "PlayerInteractionValidation.h"
#include "GarbagePileSystem.h"
#include "UIManagerSystem.h"
#include "CannonballSystem.h"
#include "GloopSystem.h"
#include "CleaningQuicktimeEventSystem.h"
#include "ItemRespawnSystem.h"
#include "CameraBreathingSystem.h"
#include "GarbagePileHealthBarSystem.h"
#include "GarbagePileGloopIndicatorSystem.h"
#include "GarbageMeterSystem.h"
#include "GameOverCheckSystem.h"
#include "ThrowableBottleSystem.h"

using namespace oyl;

class MainLayer : public Layer
{
public:
	OYL_CTOR(MainLayer, oyl::Layer)

	bool isCameraActive;

	void onEnter() override
	{
		srand(time(NULL));

		isCameraActive = false;

		listenForEventCategory(EventCategory::Keyboard);
		listenForEventCategory(EventCategory::Mouse);
		listenForEventCategory(EventCategory::Gamepad);
		listenForEventCategory((EventCategory)CategoryGarbagePile);
		listenForEventCategory((EventCategory)CategoryGameState);

		scheduleSystemUpdate<PlayerSystem>();
		scheduleSystemUpdate<CannonSystem>();
		scheduleSystemUpdate<PlayerInteractionValidationSystem>();
		scheduleSystemUpdate<GarbagePileSystem>();
		scheduleSystemUpdate<UIManagerSystem>();
		scheduleSystemUpdate<CannonballSystem>();
		scheduleSystemUpdate<GloopSystem>();
		scheduleSystemUpdate<CleaningQuicktimeEventSystem>();
		scheduleSystemUpdate<ItemRespawnSystem>();
		scheduleSystemUpdate<CameraBreathingSystem>();
		scheduleSystemUpdate<GarbagePileHealthBarSystem>();
		scheduleSystemUpdate<GarbagePileGloopIndicatorSystem>();
		scheduleSystemUpdate<GarbageMeterSystem>();
		scheduleSystemUpdate<GameOverCheckSystem>(); 
		scheduleSystemUpdate<ThrowableBottleSystem>();
	}

	void onUpdate() override
	{
		auto playerView = registry->view<Player, component::Transform>();
		for (auto& playerEntity : playerView)
		{
			auto& player          = registry->get<Player>(playerEntity);
			auto& playerTransform = registry->get<component::Transform>(playerEntity);

			if (player.state == PlayerState::stunned)
				continue;

			//player movement
			glm::vec3 desiredMoveDirection = glm::vec3(0.0f);

			if (player.playerNum == PersistentVariables::activePlayerNum)
			{
				if (Input::isKeyPressed(Key::W))
					desiredMoveDirection += playerTransform.getForward();

				if (Input::isKeyPressed(Key::S))
					desiredMoveDirection += -playerTransform.getForward();

				if (Input::isKeyPressed(Key::A))
					desiredMoveDirection += -playerTransform.getRight();

				if (Input::isKeyPressed(Key::D))
					desiredMoveDirection += playerTransform.getRight();
			}
			
			desiredMoveDirection += Input::getGamepadLeftStickY((uint)player.controllerNum) * -playerTransform.getForward();
			desiredMoveDirection += Input::getGamepadLeftStickX((uint)player.controllerNum) * playerTransform.getRight();

		    //check if it's 0 because if we normalize a vector with 0 magnitude it breaks
		    if (desiredMoveDirection == glm::vec3(0.0f))
			    player.moveDirection = desiredMoveDirection;
			else
				player.moveDirection = glm::normalize(desiredMoveDirection);

			if (!player.isCameraLocked)
			{
				//camera movement
				glm::vec2 rightStick = Input::getGamepadRightStick(player.controllerNum);

				playerTransform.rotate(glm::vec3(
					0.0f, 
					-rightStick.x * (100.0f + (PersistentVariables::controllerSensitivities[player.controllerNum] * 45.0f)) * 0.016f, 
					0.0f));

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

				auto playerCameraView = registry->view<component::Camera, component::Transform>();
				for (auto& cameraEntity : playerCameraView)
				{
					auto& camera          = registry->get<component::Camera>(cameraEntity);
					auto& cameraTransform = registry->get<component::Transform>(cameraEntity);

					if (cameraTransform.getParentEntity() != playerEntity)
						continue;

					cameraTransform.rotate(glm::vec3(
						-rightStick.y * (100.0f + (PersistentVariables::controllerSensitivities[player.controllerNum] * 45.0f)) * 0.016f,
						0.0f, 
						0.0f));

					//clamp camera up/down rotation
					float cameraRotationClampValueX = 70.0f;

					if (cameraTransform.getRotationEulerX() > cameraRotationClampValueX)
						cameraTransform.setRotationEulerX(cameraRotationClampValueX);
					else if (cameraTransform.getRotationEulerX() < -cameraRotationClampValueX)
						cameraTransform.setRotationEulerX(-cameraRotationClampValueX);

					break;
				}
			}
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
			auto evt = event_cast<KeyPressedEvent>(event);

			switch (evt.keycode)
			{
			case oyl::Key::E:
			{
				auto playerView = registry->view<Player>();
				for (entt::entity playerEntity : playerView)
				{
					if (registry->get<Player>(playerEntity).playerNum == PersistentVariables::activePlayerNum)
					{
						PlayerInteractionRequestEvent playerInteractionRequest;
						playerInteractionRequest.playerEntity           = playerEntity;
						playerInteractionRequest.itemClassificatonToUse = PlayerItemClassification::any;
						postEvent(playerInteractionRequest);
					}
				}

				break;
			}

			case oyl::Key::F:
			{
				auto playerView = registry->view<Player>();
				for (entt::entity playerEntity : playerView)
				{
					if (registry->get<Player>(playerEntity).playerNum == PersistentVariables::activePlayerNum)
					{
						CancelButtonPressedEvent cancelButtonPressed;
						cancelButtonPressed.playerEntity = playerEntity;
						postEvent(cancelButtonPressed);
					}
				}

				break;
			}

			case oyl::Key::P:
			{
				//P key changes active player for KBM control TODO: remove for final version
				if (PersistentVariables::activePlayerNum == PlayerNumber::Four)
					PersistentVariables::activePlayerNum = PlayerNumber::One;
				else
					PersistentVariables::activePlayerNum = (PlayerNumber)((int)PersistentVariables::activePlayerNum + 1);

				break;
			}

			case oyl::Key::G:
			{
				//G key changes player's teams for debugging TODO: remove for final version
				auto playerView = registry->view<Player>();
				for (entt::entity playerEntity : playerView)
				{
					auto& player = registry->get<Player>(playerEntity);

					if (player.team == Team::blue)
					{
						player.team = Team::red;
						OYL_LOG("SWITCHED TO RED TEAM");
					}
					else
					{
						player.team = Team::blue;
						OYL_LOG("SWITCHED TO BLUE TEAM");
					}
				}

				break;
			}

			case oyl::Key::T:
			{
				auto playerView = registry->view<Player>();
				for (entt::entity playerEntity : playerView)
				{
					if (registry->get<Player>(playerEntity).playerNum == PersistentVariables::activePlayerNum)
					{
						PlayerInteractionRequestEvent playerInteractionRequest;
						playerInteractionRequest.playerEntity           = playerEntity;
						playerInteractionRequest.itemClassificatonToUse = PlayerItemClassification::primary;
						postEvent(playerInteractionRequest);

						break;
					}
				}

				break;
			}
			
			case oyl::Key::Space:
			{
				auto playerView = registry->view<Player>();
				for (entt::entity playerEntity : playerView)
				{
					auto& player = registry->get<Player>(playerEntity);

					if (player.playerNum == PersistentVariables::activePlayerNum)
					{
						PlayerJumpEvent playerJump;
						playerJump.playerEntity = playerEntity;
						postEvent(playerJump);
						break;
					}
				}

				break;
			}
			}
			break;
		}

		case EventType::GamepadTriggerPressed:
		{
			auto evt = event_cast<GamepadTriggerPressedEvent>(event);
			auto playerView = registry->view<Player>();
			for (entt::entity playerEntity : playerView)
			{
				auto& player = registry->get<Player>(playerEntity);

				if (player.controllerNum != evt.gid || evt.dx <= 0.0f)
					continue;

				switch (evt.trigger)
				{
				case Gamepad(1):
				{
					PlayerInteractionRequestEvent playerInteractionRequest;
					playerInteractionRequest.playerEntity           = playerEntity;
					playerInteractionRequest.itemClassificatonToUse = PlayerItemClassification::primary;
					postEvent(playerInteractionRequest);

					break;
				}
				case Gamepad(0):
				{
					PlayerInteractionRequestEvent playerInteractionRequest;
					playerInteractionRequest.playerEntity           = playerEntity;
					playerInteractionRequest.itemClassificatonToUse = PlayerItemClassification::secondary;
					postEvent(playerInteractionRequest);

					break;
				}
				}
			}

			break;
		}

		case EventType::GamepadButtonPressed:
		{
			auto evt = event_cast<GamepadButtonPressedEvent>(event);
			
			auto playerView = registry->view<Player>();
			for (entt::entity playerEntity : playerView)
			{
				auto& player = registry->get<Player>(playerEntity);

				if (player.controllerNum != evt.gid)
					continue;
				if (evt.repeatCount != 0)
					break;

				switch (evt.button)
				{
				case Gamepad::A:
				{
					PlayerJumpEvent playerJump;
					playerJump.playerEntity = playerEntity;
					postEvent(playerJump);

					break;
				}
				case Gamepad::X:
				{
					PlayerInteractionRequestEvent playerInteractionRequest;
					playerInteractionRequest.playerEntity           = playerEntity;
					playerInteractionRequest.itemClassificatonToUse = PlayerItemClassification::any;
					postEvent(playerInteractionRequest);

					break;
				}
				case Gamepad::B:
				{
					CancelButtonPressedEvent cancelButtonPressed;
					cancelButtonPressed.playerEntity = playerEntity;
					postEvent(cancelButtonPressed);

					break;
				}
				case Gamepad::RightBumper:
				{
					PlayerDropItemRequestEvent playerDropItemRequest;
					playerDropItemRequest.playerEntity             = playerEntity;
					playerDropItemRequest.itemClassificationToDrop = PlayerItemClassification::primary;
					playerDropItemRequest.forceDrop                = false;
					postEvent(playerDropItemRequest);

					break;
				}
				case Gamepad::LeftBumper:
				{
					PlayerDropItemRequestEvent playerDropItemRequest;
					playerDropItemRequest.playerEntity             = playerEntity;
					playerDropItemRequest.itemClassificationToDrop = PlayerItemClassification::secondary;
					playerDropItemRequest.forceDrop                = false;
					postEvent(playerDropItemRequest);

					break;
				}
				}

				break;
			}

			break;
		}

		case EventType::MouseMoved:
		{
			if (!isCameraActive)
				break;

			auto evt = event_cast<MouseMovedEvent>(event);

			auto playerView = registry->view<Player, component::Transform>();
			for (auto& playerEntity : playerView)
			{
				auto& player          = registry->get<Player>(playerEntity);
				auto& playerTransform = registry->get<component::Transform>(playerEntity);

				if (player.playerNum != PersistentVariables::activePlayerNum || player.isCameraLocked)
					continue;

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

			auto playerCameraView = registry->view<component::Camera, component::Transform>();
			for (auto& cameraEntity : playerCameraView)
			{
				auto& camera          = registry->get<component::Camera>(cameraEntity);
				auto& cameraTransform = registry->get<component::Transform>(cameraEntity);

				if (camera.player != PersistentVariables::activePlayerNum)
					continue;

				cameraTransform.rotate(glm::vec3(-evt.dy * 0.5f, 0.0f, 0.0f));

				//clamp camera up/down rotation
				float cameraRotationClampValue = 70.0f;
				    
				if (cameraTransform.getRotationEulerX() > cameraRotationClampValue)
					cameraTransform.setRotationEulerX(cameraRotationClampValue);
				else if (cameraTransform.getRotationEulerX() < -cameraRotationClampValue)
					cameraTransform.setRotationEulerX(-cameraRotationClampValue);
			}

			break;
		}

		case (EventType)TypeGameEnd:
		{
			Application::get().changeScene("GameEndScene");
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
		pushLayer(MainGameObjectsInitLayer::create());
		pushLayer(MainGameGuiInitLayer::create());
		pushLayer(ScrollingTextureLayer::create());
	}
};

class Game : public oyl::Application
{
public:
    Game()
    {
		//registerScene<SplashScreenScene>();
		registerScene<MainMenuScene>();
		registerScene<TutorialScene>();
		registerScene<ControlsScreenScene>();
		registerScene<SettingsMenuScene>();
        registerScene<MainScene>();
		registerScene<GameEndScene>();
    }

    virtual void onExit() { }
};

oyl::Application* oyl::createApplication()
{
	return new Game();
}