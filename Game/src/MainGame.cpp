#include <Oyl3D.h>

#include "MainGameObjectsInit.h"
#include "MainMenu.h"
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
#include "ScrollingTextureLayer.h"
#include "ThrowableBottleSystem.h"

using namespace oyl;

class MainLayer : public Layer
{
public:
	OYL_CTOR(MainLayer, oyl::Layer)

	bool isCameraActive = false;

	void onEnter() override
	{
		srand(time(NULL));

		listenForEventCategory(EventCategory::Keyboard);
		listenForEventCategory(EventCategory::Mouse);
		listenForEventCategory(EventCategory::Gamepad);
		listenForEventCategory((EventCategory)CategoryGarbagePile);
		listenForEventCategory((EventCategory)CategoryGameState);

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
		scheduleSystemUpdate<GarbagePileGloopIndicatorSystem>();
		scheduleSystemUpdate<GarbageMeterSystem>();
		scheduleSystemUpdate<GameOverCheckSystem>(); 
		scheduleSystemUpdate<ThrowableBottleSystem>();

		Texture2D::cache("res/assets/textures/gui/cleaningQTEUp.png");
		Texture2D::cache("res/assets/textures/gui/cleaningQTEDown.png");

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
				t.setPosition(glm::vec3(-30.0f, 2.0f, 0.0f));
				t.setScale(glm::vec3(1.0f, 1.0f, 1.0f));

				auto& uiType = registry->assign<PlayerInteractionType>(e);
				uiType.type = PlayerInteractionResult::pickUpThrowableBottle;

				auto& HUDElement = registry->assign<PlayerHUDElement>(e);
				HUDElement.positionWhenActive = glm::vec3(0.0f, 2.0f, 0.0f);
				HUDElement.playerNum = (PlayerNumber)i;

				auto& so = registry->assign<component::EntityInfo>(e);
				so.name = "Pickup Bottle Message" + std::to_string(i + 1);

				auto& gui = registry->assign<component::GuiRenderable>(e);
				gui.texture = Texture2D::cache("res/assets/textures/gui/pickUpBottle.png");
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
				auto e = registry->create();

				auto& t = registry->assign<component::Transform>(e);
				t.setPosition(glm::vec3(-30.0f, -2.5f, 0.0f));
				t.setScale(glm::vec3(1.0f, 1.0f, 1.0f));

				auto& uiType = registry->assign<PlayerInteractionType>(e);
				uiType.type = PlayerInteractionResult::needToDropItems;

				auto& HUDElement = registry->assign<PlayerHUDElement>(e);
				HUDElement.positionWhenActive = glm::vec3(0.0f, 2.0f, 0.0f);
				HUDElement.playerNum = (PlayerNumber)i;

				auto& so = registry->assign<component::EntityInfo>(e);
				so.name = "Drop Items Prompt" + std::to_string(i + 1);

				auto& gui = registry->assign<component::GuiRenderable>(e);
				gui.texture = Texture2D::cache("res/assets/textures/gui/dropItemsPrompt.png");
				gui.cullingMask = 0b1 << i;
			}

			{
				auto e = registry->create();

				auto& t = registry->assign<component::Transform>(e);
				t.setPosition(glm::vec3(-30.0f, -2.5f, 0.0f));
				t.setScale(glm::vec3(2.5f, 2.0f, 1.0f));

				auto& uiType = registry->assign<PlayerInteractionType>(e);
				uiType.type = PlayerInteractionResult::needCleaningSolution;

				auto& HUDElement = registry->assign<PlayerHUDElement>(e);
				HUDElement.positionWhenActive = glm::vec3(1.5f, 0.0f, 0.0f);
				HUDElement.playerNum = (PlayerNumber)i;

				auto& so = registry->assign<component::EntityInfo>(e);
				so.name = "Cleaning Solution Icon Prompt" + std::to_string(i + 1);

				auto& gui = registry->assign<component::GuiRenderable>(e);
				if (HUDElement.playerNum == PlayerNumber::One || HUDElement.playerNum == PlayerNumber::Three)
					gui.texture = Texture2D::cache("res/assets/textures/gui/blueCleaningSolutionIcon.png");
				else
					gui.texture = Texture2D::cache("res/assets/textures/gui/redCleaningSolutionIcon.png");
				gui.cullingMask = 0b1 << i;
			}

			{
				auto e = registry->create();

				auto& t = registry->assign<component::Transform>(e);
				t.setPosition(glm::vec3(-30.0f, -2.5f, 0.0f));
				t.setScale(glm::vec3(3.0f, 3.0f, 1.0f));

				auto& uiType = registry->assign<PlayerInteractionType>(e);
				uiType.type = PlayerInteractionResult::needMop;

				auto& HUDElement = registry->assign<PlayerHUDElement>(e);
				HUDElement.positionWhenActive = glm::vec3(1.3f, 0.0f, 0.0f);
				HUDElement.playerNum = (PlayerNumber)i;

				auto& so = registry->assign<component::EntityInfo>(e);
				so.name = "Mop Icon Prompt" + std::to_string(i + 1);

				auto& gui = registry->assign<component::GuiRenderable>(e);
				if (HUDElement.playerNum == PlayerNumber::One || HUDElement.playerNum == PlayerNumber::Three)
					gui.texture = Texture2D::cache("res/assets/textures/gui/blueMopIcon.png");
				else
					gui.texture = Texture2D::cache("res/assets/textures/gui/redMopIcon.png");
				gui.cullingMask = 0b1 << i;
			}

			{
				//throw bottle prompt
				auto e = registry->create();

				auto& t = registry->assign<component::Transform>(e);
				t.setPosition(glm::vec3(-30.0f, -2.0f, 0.0f));
				t.setScale(glm::vec3(0.7f, 0.7f, 1.0f));

				auto& HUDElement = registry->assign<PlayerHUDElement>(e);
				HUDElement.positionWhenActive = glm::vec3(0.0f, -2.0f, 0.0f);
				HUDElement.playerNum          = (PlayerNumber)i;

				auto& throwBottlePrompt = registry->assign<ThrowBottlePrompt>(e);

				auto& so = registry->assign<component::EntityInfo>(e);
				so.name = "Throw Bottle Prompt " + std::to_string(i + 1);

				auto& gui = registry->assign<component::GuiRenderable>(e);
				gui.texture = Texture2D::cache("res/assets/textures/gui/throwBottlePrompt.png");
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
				//cleaning QTE-related stuff (each player gets one)
				entt::entity cancelQTEPromptEntity = registry->create();

				auto& cancelQTEPromptGui = registry->assign<component::GuiRenderable>(cancelQTEPromptEntity);
				cancelQTEPromptGui.texture     = Texture2D::cache("res/assets/textures/gui/cancelCleaningQTEPrompt.png");
				cancelQTEPromptGui.cullingMask = 0b1 << i;
				cancelQTEPromptGui.enabled     = false;

				auto& cancelQTETransform = registry->assign<component::Transform>(cancelQTEPromptEntity);
				cancelQTETransform.setPosition(glm::vec3(0.0f, -2.0f, -20.0f));
				cancelQTETransform.setScale(glm::vec3(0.7f, 0.7f, 1.0f));

				auto& cancelQTEEntityInfo = registry->assign<component::EntityInfo>(cancelQTEPromptEntity);
				cancelQTEEntityInfo.name  = "Cancel Cleaning Prompt" + std::to_string(i + 1);



				entt::entity QTEEntity = registry->create();

				auto& QTEGui = registry->assign<component::GuiRenderable>(QTEEntity);
				QTEGui.texture      = Texture2D::get("cleaningQTEUp");
				QTEGui.cullingMask  = 0b1 << i;
				QTEGui.enabled      = false;

				auto& QTETransform = registry->assign<component::Transform>(QTEEntity);
				QTETransform.setPosition(glm::vec3(0.0f, 0.0f, -20.0f));
				QTETransform.setScale(glm::vec3(3.0f, 3.0f, 1.0f));

				auto& cleaningQTE = registry->assign<CleaningQuicktimeEvent>(QTEEntity);
				cleaningQTE.playerNum = (PlayerNumber)i;
				cleaningQTE.cancelPromptEntity = cancelQTEPromptEntity;

				auto& QTEEntityInfo = registry->assign<component::EntityInfo>(QTEEntity);
				QTEEntityInfo.name = "Cleaning Quicktime Event Display" + std::to_string(i + 1);
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
				for (int k = 0; k < 3; k++)
				{
					//garbage pile gloop indicator
					entt::entity indicatorEntity = registry->create();

					auto& indicatorGui = registry->assign<component::GuiRenderable>(indicatorEntity);
					indicatorGui.texture = Texture2D::cache("res/assets/textures/gui/gloopIndicator.png");
					indicatorGui.cullingMask = 0b1 << i;

					auto& gloopIndicator = registry->assign<GarbagePileGloopIndicator>(indicatorEntity);
					gloopIndicator.garbagePileNum = k - 1;
					gloopIndicator.playerNum = (PlayerNumber)i;

					if (gloopIndicator.playerNum == PlayerNumber::One || gloopIndicator.playerNum == PlayerNumber::Three) //1 and 3 are on blue team
						gloopIndicator.team = Team::blue;
					else
						gloopIndicator.team = Team::red;

					component::Transform indicatorTransform;
					indicatorTransform.setPosition(glm::vec3(-30.0f, 0.0f, -5.0f));
					indicatorTransform.setScale(glm::vec3(2.0f, 2.0f, 2.0f));
					registry->assign<component::Transform>(indicatorEntity, indicatorTransform);

					auto& indicatorEi = registry->assign<component::EntityInfo>(indicatorEntity);
					indicatorEi.name = "Gloop Indicator " + std::to_string(k) + " For Player" + std::to_string(i);
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
				outlineTransform.setPosition(glm::vec3(0.0f, 3.8f, -20.0f));
				outlineTransform.setScale(glm::vec3(2.0f, 2.0f, 1.0f));
				registry->assign<component::Transform>(outlineEntity, outlineTransform);

				auto& outlineEi = registry->assign<component::EntityInfo>(outlineEntity);
				outlineEi.name = "GarbageMeterOutline" + std::to_string(i);

				for (int k = 0; k < 3; k++)
				{
					//garbage meter fill bars
					entt::entity fillEntity = registry->create();

					garbageMeter.garbageMeterBars[k] = fillEntity;

					auto& fillGui = registry->assign<component::GuiRenderable>(fillEntity);
					fillGui.texture = Texture2D::cache("res/assets/textures/gui/garbageMeterBar.png");
					fillGui.cullingMask = 0b1 << i;

					auto& garbageMeterBar = registry->assign<GarbageMeterBar>(fillEntity);
					garbageMeterBar.garbagePileNum = (k - 1) * -1; //flip the sign so that the left is the front of the ship and the right is the back

					auto& fillTransform = registry->assign<component::Transform>(fillEntity);
					fillTransform.setPosition(glm::vec3(0.65f * k - 0.65f, 3.8f, 1.0f));
					fillTransform.setScale(glm::vec3(2.0f, 2.0f, 1.0f));
					fillTransform.setParent(outlineEntity);

					auto& fillEi = registry->assign<component::EntityInfo>(fillEntity);
					fillEi.name = "GarbageMeterBar" + std::to_string(k) + " For Player" + std::to_string(i);
				}
			}
		}
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

			//Application::get().changeScene("MainScene");

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

			if (!player.isCameraLocked)
			{
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

				auto playerCameraView = registry->view<component::Camera, component::Transform>();
				for (auto& cameraEntity : playerCameraView)
				{
					auto& camera          = registry->get<component::Camera>(cameraEntity);
					auto& cameraTransform = registry->get<component::Transform>(cameraEntity);

					if (cameraTransform.getParentEntity() != playerEntity)
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
					if (registry->get<Player>(playerEntity).playerNum == PlayerNumber::One)
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
					if (registry->get<Player>(playerEntity).playerNum == PlayerNumber::One)
					{
						CancelButtonPressedEvent cancelButtonPressed;
						cancelButtonPressed.playerEntity = playerEntity;
						postEvent(cancelButtonPressed);
					}
				}

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
					if (registry->get<Player>(playerEntity).playerNum == PlayerNumber::One)
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

				if (player.playerNum != PlayerNumber::One || player.isCameraLocked)
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
		pushLayer(MainGameObjectsInitLayer::create());
		pushLayer(ScrollingTextureLayer::create());
	}
};

class Game : public oyl::Application
{
public:
    Game()
    {
		//registerScene<MainMenuScene>();
        registerScene<MainScene>();
    }

    virtual void onExit() { }
};

oyl::Application* oyl::createApplication()
{
	return new Game();
}