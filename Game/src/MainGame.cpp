#include <Oyl3D.h>

#include "SandboxLayer.h"
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
#include "GarbageMeterSystem.h"
#include "GameOverCheckSystem.h"
#include "ScrollingTextureLayer.h"

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
		this->listenForEventCategory(EventCategory::Gamepad);
		this->listenForEventCategory((EventCategory)CategoryGarbagePile);
		this->listenForEventCategory((EventCategory)CategoryGameState);

		// listenForEventType(EventType::PhysicsCollisionEnter);
		// listenForEventType(EventType::PhysicsCollisionStay);
		// listenForEventType(EventType::PhysicsCollisionExit);

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
		scheduleSystemUpdate<GarbageMeterSystem>();
		scheduleSystemUpdate<GameOverCheckSystem>();

		for (int i = 0; i < 4; i++)
		{
			{
				auto e = registry->create();

				auto& t = registry->assign<component::Transform>(e);
				t.setPosition(glm::vec3(-30.0f, -2.5f, 0.0f));
				t.setScale(glm::vec3(1.0f, 1.0f, 1.0f));

				auto& uiType = registry->assign<PlayerInteractionType>(e);
				uiType.type = PlayerInteractionResult::pushCannon;

				auto& HUDElement = registry->assign<PlayerHUDElement>(e);
				HUDElement.positionWhenActive = glm::vec3(0.0f, 2.0f, 0.0f);
				HUDElement.playerNum = (PlayerNumber)i;

				auto& so = registry->assign<component::EntityInfo>(e);
				so.name = "Push Cannon Message" + std::to_string(i + 1);

				auto& gui = registry->assign<component::GuiRenderable>(e);
				gui.texture = Texture2D::cache("res/assets/textures/gui/pushCannon.png");
				gui.cullingMask = 0b1 << i;
			}

			{
				auto e = registry->create();

				auto& t = registry->assign<component::Transform>(e);
				t.setPosition(glm::vec3(-30.0f, -2.5f, 0.0f));
				t.setScale(glm::vec3(1.0f, 1.0f, 1.0f));

				auto& uiType = registry->assign<PlayerInteractionType>(e);
				uiType.type = PlayerInteractionResult::loadCannon;

				auto& HUDElement = registry->assign<PlayerHUDElement>(e);
				HUDElement.positionWhenActive = glm::vec3(0.0f, 2.0f, 0.0f);
				HUDElement.playerNum = (PlayerNumber)i;

				auto& so = registry->assign<component::EntityInfo>(e);
				so.name = "Load Cannon Message" + std::to_string(i + 1);

				auto& gui = registry->assign<component::GuiRenderable>(e);
				gui.texture = Texture2D::cache("res/assets/textures/gui/loadCannon.png");
				gui.cullingMask = 0b1 << i;
			}

			{
				auto e = registry->create();

				auto& t = registry->assign<component::Transform>(e);
				t.setPosition(glm::vec3(-30.0f, -2.5f, 0.0f));
				t.setScale(glm::vec3(2.0f, 2.0f, 1.0f));

				auto& uiType = registry->assign<PlayerInteractionType>(e);
				uiType.type = PlayerInteractionResult::takeCannonballFromCrate;

				auto& HUDElement = registry->assign<PlayerHUDElement>(e);
				HUDElement.positionWhenActive = glm::vec3(0.0f, 2.0f, 0.0f);
				HUDElement.playerNum = (PlayerNumber)i;

				auto& so = registry->assign<component::EntityInfo>(e);
				so.name = "Cannonball Crate Message" + std::to_string(i + 1);

				auto& gui = registry->assign<component::GuiRenderable>(e);
				gui.texture = Texture2D::cache("res/assets/textures/gui/cannonballCrate.png");
				gui.cullingMask = 0b1 << i;
			}

			{
				auto e = registry->create();

				auto& t = registry->assign<component::Transform>(e);
				t.setPosition(glm::vec3(-30.0f, -2.5f, 0.0f));
				t.setScale(glm::vec3(0.8f, 0.8f, 1.0f));

				auto& uiType = registry->assign<PlayerInteractionType>(e);
				uiType.type = PlayerInteractionResult::pickUpCannonball;

				auto& HUDElement = registry->assign<PlayerHUDElement>(e);
				HUDElement.positionWhenActive = glm::vec3(0.0f, 2.0f, 0.0f);
				HUDElement.playerNum = (PlayerNumber)i;

				auto& so = registry->assign<component::EntityInfo>(e);
				so.name = "Pickup Cannonball Message" + std::to_string(i + 1);

				auto& gui = registry->assign<component::GuiRenderable>(e);
				gui.texture = Texture2D::cache("res/assets/textures/gui/pickupCannonball.png");
				gui.cullingMask = 0b1 << i;
			}

			{
				auto e = registry->create();

				auto& t = registry->assign<component::Transform>(e);
				t.setPosition(glm::vec3(-30.0f, -2.5f, 0.0f));
				t.setScale(glm::vec3(1.0f, 1.0f, 1.0f));

				auto& uiType = registry->assign<PlayerInteractionType>(e);
				uiType.type = PlayerInteractionResult::pickUpMop;

				auto& HUDElement = registry->assign<PlayerHUDElement>(e);
				HUDElement.positionWhenActive = glm::vec3(0.0f, 2.0f, 0.0f);
				HUDElement.playerNum = (PlayerNumber)i;

				auto& so = registry->assign<component::EntityInfo>(e);
				so.name = "Pickup Mop Message" + std::to_string(i + 1);

				auto& gui = registry->assign<component::GuiRenderable>(e);
				gui.texture = Texture2D::cache("res/assets/textures/gui/pickupMop.png");
				gui.cullingMask = 0b1 << i;
			}

			{
				auto e = registry->create();

				auto& t = registry->assign<component::Transform>(e);
				t.setPosition(glm::vec3(-30.0f, -2.5f, 0.0f));
				t.setScale(glm::vec3(0.8f, 0.8f, 1.0f));

				auto& uiType = registry->assign<PlayerInteractionType>(e);
				uiType.type = PlayerInteractionResult::pickUpCleaningSolution;

				auto& HUDElement = registry->assign<PlayerHUDElement>(e);
				HUDElement.positionWhenActive = glm::vec3(0.0f, 2.0f, 0.0f);
				HUDElement.playerNum = (PlayerNumber)i;

				auto& so = registry->assign<component::EntityInfo>(e);
				so.name = "Pickup Cleaning Solution Message" + std::to_string(i + 1);

				auto& gui = registry->assign<component::GuiRenderable>(e);
				gui.texture = Texture2D::cache("res/assets/textures/gui/pickupCleaningSolution.png");
				gui.cullingMask = 0b1 << i;
			}

			{
				auto e = registry->create();

				auto& t = registry->assign<component::Transform>(e);
				t.setPosition(glm::vec3(-30.0f, -2.5f, 0.0f));
				t.setScale(glm::vec3(1.0f, 1.0f, 1.0f));

				auto& uiType = registry->assign<PlayerInteractionType>(e);
				uiType.type = PlayerInteractionResult::pickUpGloop;

				auto& HUDElement = registry->assign<PlayerHUDElement>(e);
				HUDElement.positionWhenActive = glm::vec3(0.0f, 2.0f, 0.0f);
				HUDElement.playerNum = (PlayerNumber)i;

				auto& so = registry->assign<component::EntityInfo>(e);
				so.name = "Pickup Gloop Message" + std::to_string(i + 1);

				auto& gui = registry->assign<component::GuiRenderable>(e);
				gui.texture = Texture2D::cache("res/assets/textures/gui/pickUpGloop.png");
				gui.cullingMask = 0b1 << i;
			}

			{
				auto e = registry->create();

				auto& t = registry->assign<component::Transform>(e);
				t.setPosition(glm::vec3(-30.0f, -2.5f, 0.0f));
				t.setScale(glm::vec3(1.0f, 1.0f, 1.0f));

				auto& uiType = registry->assign<PlayerInteractionType>(e);
				uiType.type = PlayerInteractionResult::useGloop;

				auto& HUDElement = registry->assign<PlayerHUDElement>(e);
				HUDElement.positionWhenActive = glm::vec3(0.0f, 2.0f, 0.0f);
				HUDElement.playerNum = (PlayerNumber)i;

				auto& so = registry->assign<component::EntityInfo>(e);
				so.name = "Use Gloop Message" + std::to_string(i + 1);

				auto& gui = registry->assign<component::GuiRenderable>(e);
				gui.texture = Texture2D::cache("res/assets/textures/gui/useGloop.png");
				gui.cullingMask = 0b1 << i;
			}

			{
				auto e = registry->create();

				auto& t = registry->assign<component::Transform>(e);
				t.setPosition(glm::vec3(-30.0f, -2.5f, 0.0f));
				t.setScale(glm::vec3(1.0f, 1.0f, 1.0f));

				auto& uiType = registry->assign<PlayerInteractionType>(e);
				uiType.type = PlayerInteractionResult::cleanGarbagePile;

				auto& HUDElement = registry->assign<PlayerHUDElement>(e);
				HUDElement.positionWhenActive = glm::vec3(0.0f, 2.0f, 0.0f);
				HUDElement.playerNum = (PlayerNumber)i;

				auto& so = registry->assign<component::EntityInfo>(e);
				so.name = "Clean Garbage Message" + std::to_string(i + 1);

				auto& gui = registry->assign<component::GuiRenderable>(e);
				gui.texture = Texture2D::cache("res/assets/textures/gui/cleanGarbage.png");
				gui.cullingMask = 0b1 << i;
			}

			{
				auto e = registry->create();

				auto& t = registry->assign<component::Transform>(e);
				t.setPosition(glm::vec3(-30.0f, -2.5f, 0.0f));
				t.setScale(glm::vec3(1.0f, 1.0f, 1.0f));

				auto& uiType = registry->assign<PlayerInteractionType>(e);
				uiType.type = PlayerInteractionResult::cannonFiringSoon;

				auto& HUDElement = registry->assign<PlayerHUDElement>(e);
				HUDElement.positionWhenActive = glm::vec3(0.0f, 2.0f, 0.0f);
				HUDElement.playerNum = (PlayerNumber)i;

				auto& so = registry->assign<component::EntityInfo>(e);
				so.name = "Cannon Firing Message" + std::to_string(i + 1);

				auto& gui = registry->assign<component::GuiRenderable>(e);
				gui.texture = Texture2D::cache("res/assets/textures/gui/cannonFiring.png");
				gui.cullingMask = 0b1 << i;
			}

			{
				entt::entity e = registry->create();
				auto& gui = registry->assign<component::GuiRenderable>(e);
				gui.texture = Texture2D::cache("res/assets/textures/gui/reticle.png");
				gui.cullingMask = 0b1 << i;

				auto& reticle = registry->assign<Reticle>(e);
				reticle.type = ReticleType::normal;

				auto& HUDElement = registry->assign<PlayerHUDElement>(e);
				HUDElement.positionWhenActive = glm::vec3(0.0f, 0.0f, 0.0f);
				HUDElement.playerNum = (PlayerNumber)i;

				component::Transform t;
				t.setPosition(glm::vec3(-30.0f, -1.0f, -2.0f));
				t.setScale(glm::vec3(0.2f, 0.2f, 1.0f));
				registry->assign<component::Transform>(e, t);

				auto& so = registry->assign<component::EntityInfo>(e);
				so.name = "Normal Reticle" + std::to_string(i + 1);
			}

			{
				entt::entity e = registry->create();
				auto& gui = registry->assign<component::GuiRenderable>(e);
				gui.texture = Texture2D::cache("res/assets/textures/gui/invalid_reticle.png");
				gui.cullingMask = 0b1 << i;

				auto& reticle = registry->assign<Reticle>(e);
				reticle.type = ReticleType::invalid;

				auto& HUDElement = registry->assign<PlayerHUDElement>(e);
				HUDElement.positionWhenActive = glm::vec3(0.0f, 0.0f, 0.0f);
				HUDElement.playerNum = (PlayerNumber)i;

				component::Transform t;
				t.setPosition(glm::vec3(-30.0f, -1.0f, -2.0f));
				t.setScale(glm::vec3(0.5f, 0.5f, 1.0f));
				registry->assign<component::Transform>(e, t);

				auto& so = registry->assign<component::EntityInfo>(e);
				so.name = "Invalid Reticle" + std::to_string(i + 1);
			}

			{
				//background for cleaning quicktime event
				entt::entity cleaningQuicktimeEventBackgroundEntity = registry->create();
				auto& backgroundGUI = registry->assign<component::GuiRenderable>(cleaningQuicktimeEventBackgroundEntity);
				backgroundGUI.texture = Texture2D::cache("res/assets/textures/gui/cleaningQuicktimeEventBackground.png");
				backgroundGUI.cullingMask = 0b1 << i;

				component::Transform backgroundTransform;
				backgroundTransform.setPosition(glm::vec3(-30.0f, 3.5f, -10.0f));
				backgroundTransform.setScale(glm::vec3(2.0f, 1.5f, 1.0f));
				registry->assign<component::Transform>(cleaningQuicktimeEventBackgroundEntity, backgroundTransform);

				auto& backgroundHUDElement = registry->assign<PlayerHUDElement>(cleaningQuicktimeEventBackgroundEntity);
				backgroundHUDElement.positionWhenActive = glm::vec3(0.0f, 3.5f, -10.0f);
				backgroundHUDElement.playerNum = (PlayerNumber)i;

				auto& backgroundSceneObject = registry->assign<component::EntityInfo>(cleaningQuicktimeEventBackgroundEntity);
				backgroundSceneObject.name = "Cleaning Quicktime Event Background" + std::to_string(i + 1);



				//indicator for cleaning quicktime event
				entt::entity cleaningQuicktimeEventIndicatorEntity = registry->create();
				auto& indicatorGUI = registry->assign<component::GuiRenderable>(cleaningQuicktimeEventIndicatorEntity);
				indicatorGUI.texture = Texture2D::cache("res/assets/textures/gui/cleaningQuicktimeEventIndicator.png");
				indicatorGUI.cullingMask = 0b1 << i;

				component::Transform indicatorTransform;
				indicatorTransform.setPosition(glm::vec3(-30.0f, 3.6f, -11.0f));
				indicatorTransform.setScale(glm::vec3(1.5f, 2.0f, 1.0f));
				registry->assign<component::Transform>(cleaningQuicktimeEventIndicatorEntity, indicatorTransform);

				CleaningQuicktimeEventIndicator cleaningQuicktimeEventIndicator;
				cleaningQuicktimeEventIndicator.lerpInformation.startPos = glm::vec3(-4.95f, 3.6f, -11.0f);
				cleaningQuicktimeEventIndicator.lerpInformation.destinationPos = glm::vec3(4.95f, 3.6f, -11.0f);
				cleaningQuicktimeEventIndicator.lerpInformation.speed = 2.2f;

				cleaningQuicktimeEventIndicator.cleaningQuicktimeEventBackground = cleaningQuicktimeEventBackgroundEntity;
				registry->assign<CleaningQuicktimeEventIndicator>(cleaningQuicktimeEventIndicatorEntity, cleaningQuicktimeEventIndicator);

				auto& indicatorHUDElement = registry->assign<PlayerHUDElement>(cleaningQuicktimeEventIndicatorEntity);
				indicatorHUDElement.positionWhenActive = glm::vec3(-4.95f, 3.6f, -11.0f);
				indicatorHUDElement.playerNum = (PlayerNumber)i;

				auto& indicatorSceneObject = registry->assign<component::EntityInfo>(cleaningQuicktimeEventIndicatorEntity);
				indicatorSceneObject.name = "Cleaning Quicktime Event Indicator" + std::to_string(i + 1);
			}

			{
				for (int k = 0; k < 3; k++)
				{
					//garbage pile heath bar outline
					entt::entity outlineEntity = registry->create();
					auto& outlineGui = registry->assign<component::GuiRenderable>(outlineEntity);
					outlineGui.texture = Texture2D::cache("res/assets/textures/gui/garbagePileHPBarOutline.png");
					outlineGui.cullingMask = 0b1 << i;

					component::Transform outlineTransform;
					outlineTransform.setPosition(glm::vec3(-30.0f, 0.0f, 0.0f));
					outlineTransform.setScale(glm::vec3(4.0f, 4.0f, 4.0f));
					registry->assign<component::Transform>(outlineEntity, outlineTransform);

					auto& outlineEi = registry->assign<component::EntityInfo>(outlineEntity);
					outlineEi.name = "GarbageHPBarOutline" + std::to_string(k) + " For Player" + std::to_string(i);



					//garbage pile health bar fill
					entt::entity fillEntity = registry->create();
					auto& fillGui = registry->assign<component::GuiRenderable>(fillEntity);
					fillGui.texture = Texture2D::cache("res/assets/textures/gui/garbagePileHPBarFill.png");
					fillGui.cullingMask = 0b1 << i;

					auto& garbageHPBar = registry->assign<GarbagePileHealthBar>(fillEntity);
					garbageHPBar.garbagePileNum = k - 1;
					garbageHPBar.playerNum      = (PlayerNumber)i;
					garbageHPBar.outlineEntity  = outlineEntity;

					if (garbageHPBar.playerNum == PlayerNumber::One || garbageHPBar.playerNum == PlayerNumber::Three) //1 and 3 are on blue team
						garbageHPBar.team = Team::blue;
					else
						garbageHPBar.team = Team::red;

					component::Transform fillTransform;
					fillTransform.setPosition(glm::vec3(-30.0f, 0.0f, 0.0f));
					fillTransform.setScale(glm::vec3(4.0f, 4.0f, 4.0f));
					registry->assign<component::Transform>(fillEntity, fillTransform);

					auto& fillEi = registry->assign<component::EntityInfo>(fillEntity);
					fillEi.name = "GarbageHPBarFill" + std::to_string(k) + " For Player" + std::to_string(i);
				}
			}

			{
				//garbage meter outline
				entt::entity outlineEntity = registry->create();
				auto& outlineGui = registry->assign<component::GuiRenderable>(outlineEntity);
				outlineGui.texture = Texture2D::cache("res/assets/textures/gui/garbageMeterOutline.png");
				outlineGui.cullingMask = 0b1 << i;

				auto& garbageMeter = registry->assign<GarbageMeterDisplay>(outlineEntity);
				garbageMeter.playerNum = (PlayerNumber)i;

				if (garbageMeter.playerNum == PlayerNumber::One || garbageMeter.playerNum == PlayerNumber::Three) //1 and 3 are on blue team
					garbageMeter.team = Team::blue;
				else
					garbageMeter.team = Team::red;

				component::Transform outlineTransform;
				outlineTransform.setPosition(glm::vec3(0.0f, 3.8f, 12.0f));
				outlineTransform.setScale(glm::vec3(2.0f, 2.0f, 1.0f));
				registry->assign<component::Transform>(outlineEntity, outlineTransform);

				auto& outlineEi = registry->assign<component::EntityInfo>(outlineEntity);
				outlineEi.name = "GarbageMeterOutline" + std::to_string(i);

				for (int k = 0; k < 3; k++)
				{
					//garbage meter fill bars
					entt::entity fillEntity = registry->create();

					garbageMeter.garbageMeterBars[k] = fillEntity;

					auto& fillParent = registry->assign<component::Parent>(fillEntity);
					fillParent.parent = outlineEntity;

					auto& fillGui = registry->assign<component::GuiRenderable>(fillEntity);
					fillGui.texture = Texture2D::cache("res/assets/textures/gui/garbageMeterBar.png");
					fillGui.cullingMask = 0b1 << i;

					auto& garbageMeterBar = registry->assign<GarbageMeterBar>(fillEntity);
					garbageMeterBar.garbagePileNum = (k - 1) * -1; //flip the sign so that the left is the front of the ship and the right is the back

					component::Transform fillTransform;
					fillTransform.setPosition(glm::vec3(0.65f * k - 0.65f, 3.8f, 10.0f));
					fillTransform.setScale(glm::vec3(2.0f, 2.0f, 1.0f));
					registry->assign<component::Transform>(fillEntity, fillTransform);

					auto& fillEi = registry->assign<component::EntityInfo>(fillEntity);
					fillEi.name = "GarbageMeterBar" + std::to_string(k) + " For Player" + std::to_string(i);
				}
			}
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
	}

	void onUpdate() override
	{
		auto playerView = registry->view<Player, component::Transform>();
		for (auto& playerEntity : playerView)
		{
			auto& player          = registry->get<Player>(playerEntity);
			auto& playerTransform = registry->get<component::Transform>(playerEntity);

			//player movement
			glm::vec3 desiredMoveDirection = glm::vec3(0.0f);

			if (player.playerNum == PlayerNumber::One)
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

			

			//camera movement
			glm::vec2 rightStick = Input::getGamepadRightStick(player.controllerNum);

			playerTransform.rotate(glm::vec3(0.0f, -rightStick.x * 200.0f * Time::deltaTime(), 0.0f));

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

			auto playerCameraView = registry->view<component::Camera, component::Transform, component::Parent>();
			for (auto& cameraEntity : playerCameraView)
			{
				auto& camera          = registry->get<component::Camera>(cameraEntity);
				auto& cameraTransform = registry->get<component::Transform>(cameraEntity);
				auto& cameraParent    = registry->get<component::Parent>(cameraEntity);

				if (cameraParent.parent != playerEntity)
					continue;

				cameraTransform.rotate(glm::vec3(-rightStick.y * 200.0f * Time::deltaTime(), 0.0f, 0.0f));

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
			
			case oyl::Key::Space:
			{
				auto playerView = registry->view<Player>();
				for (entt::entity playerEntity : playerView)
				{
					auto& player = registry->get<Player>(playerEntity);

					if (player.playerNum == PlayerNumber::One)
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
					playerInteractionRequest.playerEntity = playerEntity;
					postEvent(playerInteractionRequest);

					break;
				}
				case Gamepad::B:
				{
					PlayerDropItemEvent playerDropItem;
					playerDropItem.playerEntity = playerEntity;
					postEvent(playerDropItem);

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

				if (player.playerNum != PlayerNumber::One)
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

				if (camera.player != PlayerNumber::One)
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
			auto evt = event_cast<GameEndEvent>(event);

			auto e = registry->create();

			auto& t = registry->assign<component::Transform>(e);
			t.setPosition(glm::vec3(0.0f, 0.0f, -100.0f));
			t.setScale(glm::vec3(10.0f, 10.0f, 10.0f));

			auto& so = registry->assign<component::EntityInfo>(e);
			so.name = "Game Over Message";

			auto& gui = registry->assign<component::GuiRenderable>(e);

			if (evt.result == GameEndResult::blueWin)
				gui.texture = Texture2D::cache("res/assets/textures/gui/blueWins.png");
			else if (evt.result == GameEndResult::redWin)
				gui.texture = Texture2D::cache("res/assets/textures/gui/redWins.png");
			else //tie game
				gui.texture = Texture2D::cache("res/assets/textures/gui/draw.png");
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
		pushLayer(ScrollingTextureLayer::create());
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