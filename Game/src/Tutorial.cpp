#include "Tutorial.h"

#include "PlayerSystem.h"
#include "CannonSystem.h"
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
#include "ThrowableBottleSystem.h"

using namespace oyl;

void TutorialLayer::onEnter()
{
	firstSegmentInit  = false;
	secondSegmentInit = false;
	thirdSegmentInit  = false;
	fourthSegmentInit = false;
	fifthSegmentInit  = false;
	sixthSegmentInit  = false;

	listenForEventCategory(EventCategory::Keyboard);
	listenForEventCategory(EventCategory::Gamepad);

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
	scheduleSystemUpdate<ThrowableBottleSystem>();

	firstFrame = true;

	{
		auto e = registry->create();

		auto& t = registry->assign<component::Transform>(e);
		t.setPosition(glm::vec3(0.0f, -2.9f, -20.0f));
		t.setScale(glm::vec3(1.0f, 1.0f, 1.0f));

		auto& so = registry->assign<component::EntityInfo>(e);
		so.name = "Main Menu Prompt";

		auto& gui = registry->assign<component::GuiRenderable>(e);
		gui.texture = Texture2D::cache("res/assets/textures/gui/mainMenuPromptStart.png");
	}

	{
		auto e = registry->create();

		auto& tutorialEndSegmentPrompt = registry->assign<TutorialEndSegmentPrompt>(e);

		auto& t = registry->assign<component::Transform>(e);
		t.setPosition(glm::vec3(0.0f, -2.9f, -20.0f));
		t.setScale(glm::vec3(1.0f, 1.0f, 1.0f));

		auto& so = registry->assign<component::EntityInfo>(e);
		so.name = "Next Segment Prompt";

		auto& gui = registry->assign<component::GuiRenderable>(e);
		gui.texture = Texture2D::cache("res/assets/textures/gui/nextSegmentPromptA.png");
	}

	{
		auto e = registry->create();

		auto& tutorialEndSegmentPrompt = registry->assign<TutorialEndSegmentPrompt>(e);

		auto& t = registry->assign<component::Transform>(e);
		t.setPosition(glm::vec3(0.0f, -2.9f, -20.0f));
		t.setScale(glm::vec3(1.0f, 1.0f, 1.0f));

		auto& so = registry->assign<component::EntityInfo>(e);
		so.name = "Repeat Segment Prompt";

		auto& gui = registry->assign<component::GuiRenderable>(e);
		gui.texture = Texture2D::cache("res/assets/textures/gui/repeatSegmentPromptB.png");
	}
}

void TutorialLayer::onUpdate()
{
	if (firstFrame)
	{
		firstFrame  = false;
		initSegment = true;
		currentSegmentFunc = &TutorialLayer::intro;

		//remove any unwanted players
		auto playerView = registry->view<Player>();
		for (auto& playerEntity : playerView)
		{
			auto& player = registry->get<Player>(playerEntity);

			if (player.playerNum == PlayerNumber::Three || player.playerNum == PlayerNumber::Four)
				registry->destroy(playerEntity);
		}

		//remove any unwanted cameras
		auto cameraView = registry->view<component::Camera>();
		for (auto& cameraEntity : cameraView)
		{
			auto& camera = registry->get<component::Camera>(cameraEntity);

			if (camera.player == PlayerNumber::One)
				tutCameraEntity = cameraEntity;
			else
				registry->destroy(cameraEntity);
		}

		//set initial cannon variables
		auto cannonView = registry->view<Cannon>();
		for (auto& cannonEntity : cannonView)
		{
			auto& cannon          = registry->get<Cannon>(cannonEntity);
			auto& cannonTransform = registry->get<component::Transform>(cannonEntity);

			if (cannon.team == Team::blue)
			{
				initialCannonTrackPos = cannon.trackPosition;
				initialCannonPos      = cannonTransform.getPosition();
			}
		}

		//set throwable bottle respawn timer below normal value otherwise the segment can be replayed before a new bottle spawns
		auto spawnerView = registry->view<RespawnManager>();
		for (auto& spawnerEntity : spawnerView)
		{
			auto& spawner = registry->get<RespawnManager>(spawnerEntity);
			
			if (spawner.type == CarryableItemType::throwableBottle)
				spawner.respawnTimerDuration = 20.0f;
		}
	}

	SetMaxGarbageLevelEvent setMaxGarbageLevel;
	postEvent(setMaxGarbageLevel);

	auto& playerView = registry->view<Player>();
	for (auto& playerEntity : playerView)
	{
		if (registry->get<Player>(playerEntity).playerNum == PlayerNumber::One)
			tutPlayerEntity = playerEntity;
	}

	isSegmentFinished = false; //if a segment is finished, it will be set to true in the segment's function

	(this->*currentSegmentFunc)();

	//show segment prompts when we are finished a segment (waiting for user input)
	auto& segmentPromptView = registry->view<TutorialEndSegmentPrompt, component::GuiRenderable>();
	for (auto& segmentPromptEntity : segmentPromptView)
	{
		auto& segmentPromptGui = registry->get<component::GuiRenderable>(segmentPromptEntity);

		segmentPromptGui.enabled = isSegmentFinished;
	}
}

bool TutorialLayer::onEvent(const Event& event)
{
	switch (event.type)
	{
	case EventType::KeyPressed:
	{
		auto evt = event_cast<KeyPressedEvent>(event);

		switch (evt.keycode)
		{
		case oyl::Key::Escape:
		{
			Application::get().changeScene("MainMenuScene");
			break;
		}

		case oyl::Key::Backspace:
		{
			if (!isSegmentFinished)
				break;

			repeatSegment();
			break;
		}

		case oyl::Key::Enter:
		{
			if (!isSegmentFinished)
				break;

			moveToNextSegment();
			break;
		}
		}

		break;
	}

	case EventType::GamepadButtonPressed:
	{
		auto evt = event_cast<GamepadButtonPressedEvent>(event);

		switch (evt.button)
		{
		case Gamepad::Start:
		{
			Application::get().changeScene("MainMenuScene");
			break;
		}

		case Gamepad::A:
		{
			if (!isSegmentFinished)
				break;

			moveToNextSegment();
			break;
		}

		case Gamepad::Back:
		case Gamepad::B:
		{
			if (!isSegmentFinished)
				break;

			repeatSegment();
			break;
		}
		}

		break;
	}
	}

	return false;
}

void TutorialLayer::onGuiRender()
{

}

void TutorialLayer::intro()
{
	auto& playerTransform = registry->get<component::Transform>(tutPlayerEntity);

	if (initSegment)
	{
		TutorialSegmentStartedEvent segmentStarted;
		segmentStarted.segmentNum = 0;
		postEvent(segmentStarted);

		currentSegment    = TutorialSegment::intro;
		initSegment       = false;
		isSegmentFinished = false;

		segmentTimer1 = 5.0f; //intro shit
	}

	segmentTimer1 -= Time::deltaTime();
	if (segmentTimer1 > 0.0f)
		return;

	initSegment = true;
	currentSegmentFunc = &TutorialLayer::segment1;
}

void TutorialLayer::segment1()
{
	auto& playerTransform = registry->get<component::Transform>(tutPlayerEntity);
	auto& cameraTransform = registry->get<component::Transform>(tutCameraEntity);

	if (initSegment)
	{
		TutorialSegmentStartedEvent segmentStarted;
		segmentStarted.segmentNum = 1;
		postEvent(segmentStarted);

		currentSegment    = TutorialSegment::segment1;
		initSegment       = false;
		isSegmentFinished = false;

		if (!firstSegmentInit)
		{
			firstSegmentInit = true;

			playerSegment1Pos = playerTransform.getPosition();
			playerSegment1Rot = playerTransform.getRotation();
			cameraSegment1Rot = cameraTransform.getRotation();
		}

		playerTransform.setPosition(playerSegment1Pos);
		playerTransform.setRotation(playerSegment1Rot);
		cameraTransform.setRotation(cameraSegment1Rot);

		segmentBool1  = true;
		segmentTimer1 = 4.0f; //"left stick to look, right stick to move"
		segmentTimer2 = 2.5f; //"A to jump"
		segmentTimer3 = 1.2f; //delay before ending segment
		segmentInterpolationParam1 = 0.0f;
	}

	segmentTimer1 -= Time::deltaTime();
	if (segmentTimer1 > 0.0f)
		return;

	glm::vec3 targetPos1 = glm::vec3(12.4f, playerTransform.getPositionY(), -4.15f);
	bool isFinished1;

	movePlayerTowardPos(targetPos1, &isFinished1);
	if (!isFinished1)
		return;
	
	if (playerTransform.getRotationEulerY() > 68.0f)
	{
		playerTransform.rotate(glm::vec3(0.0f, -0.3f, 0.0f));
		return;
	}

	segmentTimer2 -= Time::deltaTime();
	if (segmentTimer2 > 0.0f)
		return;

	if (segmentBool1)
	{
		segmentBool1 = false;

		PlayerJumpEvent playerJump;
		playerJump.playerEntity = tutPlayerEntity;
		postEvent(playerJump);
	}

	segmentTimer3 -= Time::deltaTime();
	if (segmentTimer3 > 0.0f)
		return;

	isSegmentFinished = true;
}

void TutorialLayer::segment2()
{
	auto& player          = registry->get<Player>(tutPlayerEntity);
	auto& playerTransform = registry->get<component::Transform>(tutPlayerEntity);
	auto& cameraTransform = registry->get<component::Transform>(tutCameraEntity);

	if (initSegment)
	{
		TutorialSegmentStartedEvent segmentStarted;
		segmentStarted.segmentNum = 2;
		postEvent(segmentStarted);

		currentSegment    = TutorialSegment::segment2;
		initSegment       = false;
		isSegmentFinished = false;

		if (!secondSegmentInit)
		{
			secondSegmentInit = true;

			playerSegment2    = &player;
			playerSegment2Pos = playerTransform.getPosition();
			playerSegment2Rot = playerTransform.getRotation();
			cameraSegment2Rot = cameraTransform.getRotation();
		}

		player = *playerSegment2;
		playerTransform.setPosition(playerSegment2Pos);
		playerTransform.setRotation(playerSegment2Rot);
		cameraTransform.setRotation(cameraSegment2Rot);

		segmentTimer1 = 5.0f; //"lets grab cleaning solution and a mop"
		segmentTimer2 = 0.4f; //delay before grabbing cleaning solution
		segmentTimer3 = 0.4f; //delay before grabbing mop
		segmentTimer4 = 2.5f; //"there are 3 garbage piles on the ship"
		segmentTimer5 = 1.2f; //"one at the front"
		segmentTimer6 = 1.2f; //"one in the middle"
		segmentTimer7 = 1.2f; //"one in the back"

		segmentBool1 = true;
		segmentBool2 = true;
		segmentBool3 = true;
		segmentBool4 = true;
		segmentBool5 = true;
		segmentBool6 = true;
		segmentBool7 = true;
		segmentBool8 = true;
	}

	segmentTimer1 -= Time::deltaTime();
	if (segmentTimer1 > 0.0f)
		return;

	//move toward cleaning solution
	if (segmentBool1)
	{
		glm::vec3 targetPos = glm::vec3(11.73f, playerTransform.getPositionY(), -4.65f);
		bool isFinished;

		movePlayerTowardPos(targetPos, &isFinished);
		if (!isFinished)
			return;
		else
		{
			segmentBool1 = false;

			//grab cleaning solution
			PlayerInteractionRequestEvent playerInteractionRequest;
			playerInteractionRequest.playerEntity           = tutPlayerEntity;
			playerInteractionRequest.itemClassificatonToUse = PlayerItemClassification::any;
			postEvent(playerInteractionRequest);
		}
	}

	segmentTimer2 -= Time::deltaTime();
	if (segmentTimer2 > 0.0f)
		return;

	//rotate toward mop
	if (segmentBool2)
	{
		segmentBool2 = false;
		playerTransform.rotate(glm::vec3(0.0f, 73.9f, 0.0f)); //TODO: rotate over time
		return;
	}

	//move toward mop
	if (segmentBool3)
	{
		glm::vec3 targetPos = glm::vec3(10.45f, playerTransform.getPositionY(), -2.13f);
		bool isFinished;

		movePlayerTowardPos(targetPos, &isFinished);
		if (!isFinished)
			return;
		else
		{
			segmentBool3 = false;

			//grab mop
			PlayerInteractionRequestEvent playerInteractionRequest;
			playerInteractionRequest.playerEntity           = tutPlayerEntity;
			playerInteractionRequest.itemClassificatonToUse = PlayerItemClassification::any;
			postEvent(playerInteractionRequest);
		}
	}

	segmentTimer3 -= Time::deltaTime();
	if (segmentTimer3 > 0.0f)
		return;

	//rotate toward the exit of captain's quarters
	if (segmentBool4)
	{
		segmentBool4 = false;
		playerTransform.rotate(glm::vec3(0.0f, -59.2f, 0.0f)); //TODO: rotate over time
		return;
	}

	//move toward middle garbage pile
	if (segmentBool5)
	{
		glm::vec3 targetPos = glm::vec3(2.0f, playerTransform.getPositionY(), -3.85f);
		bool isFinished;

		movePlayerTowardPos(targetPos, &isFinished);
		if (!isFinished)
			return;
		else
			segmentBool5 = false;
	}

	segmentTimer4 -= Time::deltaTime();
	if (segmentTimer4 > 0.0f)
		return; 
	
	//rotate toward the front garbage pile
	if (segmentBool6)
	{
		segmentBool6 = false;
		playerTransform.rotate(glm::vec3(0.0f, 11.0f, 0.0f)); //TODO: rotate over time
		cameraTransform.rotate(glm::vec3(10.4f, 0.0f, 0.0f)); //TODO: rotate over time
		return;
	}

	segmentTimer5 -= Time::deltaTime();
	if (segmentTimer5 > 0.0f)
		return;

	//rotate toward the middle garbage pile
	if (segmentBool7)
	{
		segmentBool7 = false;
		cameraTransform.rotate(glm::vec3(-60.0f, 0.0f, 0.0f)); //TODO: rotate over time
		return;
	}

	segmentTimer6 -= Time::deltaTime();
	if (segmentTimer6 > 0.0f)
		return;

	//rotate toward the back garbage pile
	if (segmentBool8)
	{
		segmentBool8 = false;
		playerTransform.rotate(glm::vec3(0.0f, 170.0f, 0.0f)); //TODO: rotate over time
		cameraTransform.rotate(glm::vec3(79.0f, 0.0f, 0.0f)); //TODO: rotate over time
		return;
	}

	segmentTimer7 -= Time::deltaTime();
	if (segmentTimer7 > 0.0f)
		return;

	isSegmentFinished = true;
}

void TutorialLayer::segment3()
{
	auto& player          = registry->get<Player>(tutPlayerEntity);
	auto& playerTransform = registry->get<component::Transform>(tutPlayerEntity);
	auto& cameraTransform = registry->get<component::Transform>(tutCameraEntity);

	if (initSegment)
	{
		TutorialSegmentStartedEvent segmentStarted;
		segmentStarted.segmentNum = 3;
		postEvent(segmentStarted);

		currentSegment = TutorialSegment::segment3;
		initSegment = false;
		isSegmentFinished = false;

		if (!thirdSegmentInit)
		{
			thirdSegmentInit = true;

			playerSegment3    = &player;
			playerSegment3Pos = playerTransform.getPosition();
			playerSegment3Rot = playerTransform.getRotation();
			cameraSegment3Rot = cameraTransform.getRotation();
		}

		player = *playerSegment3;
		playerTransform.setPosition(playerSegment3Pos);
		playerTransform.setRotation(playerSegment3Rot);
		cameraTransform.setRotation(cameraSegment3Rot);

		segmentTimer1 = 6.0f; //"garbage pile health bar blah blah blah clean with X"
		segmentTimer2 = 4.0f; //"to continue cleaning flick the stick"
		segmentTimer3 = 0.13f; //delay between cleaning cycles
		segmentTimer4 = 0.0f;
		segmentTimer5 = 0.0f;
		segmentTimer6 = 0.0f;
		segmentTimer7 = 0.0f;
		segmentTimer8 = 0.0f;

		segmentBool1 = true;
		segmentBool2 = true;
		segmentBool3 = true;
		segmentBool4 = true;
		segmentBool5 = true;
		segmentBool6 = true;
		segmentBool7 = true;
		segmentBool8 = true;

		if (player.secondaryCarriedItem == entt::null)
		{
			//force player to be holding a cleaning solution
			auto carryableItemView = registry->view<CarryableItem>();
			for (auto itemEntity : carryableItemView)
			{
				auto& carryable          = registry->get<CarryableItem>(itemEntity);
				auto& carryableTransform = registry->get<component::Transform>(itemEntity);

				if (   carryable.type == CarryableItemType::cleaningSolution 
					&& carryable.team == player.team
					&& carryableTransform.getPositionY() > -999.0f) //ensure it's not a prefab entity
				{
					PlayerForceItemPickUpEvent forceItemPickUp;
					forceItemPickUp.playerEntity = tutPlayerEntity;
					forceItemPickUp.itemEntity   = itemEntity;
					postEvent(forceItemPickUp);

					break;
				}
			}
		}
	}

	//rotate toward the middle garbage pile
	if (segmentBool1)
	{
		segmentBool1 = false;
		playerTransform.rotate(glm::vec3(0.0f, -170.0f, 0.0f)); //TODO: rotate over time
		cameraTransform.rotate(glm::vec3(-69.0f, 0.0f, 0.0f)); //TODO: rotate over time
		return;
	}

	segmentTimer1 -= Time::deltaTime();
	if (segmentTimer1 > 0.0f)
		return;

	//use cleaning solution
	if (segmentBool2)
	{
		segmentBool2 = false;
		
		PlayerInteractionRequestEvent playerInteractionRequest;
		playerInteractionRequest.playerEntity           = tutPlayerEntity;
		playerInteractionRequest.itemClassificatonToUse = PlayerItemClassification::any;
		postEvent(playerInteractionRequest);
	}

	segmentTimer2 -= Time::deltaTime();
	if (segmentTimer2 > 0.0f)
		return;

	if (segmentBool3)
	{
		if (player.state != PlayerState::inCleaningQuicktimeEvent)
		{
			segmentBool3 = false;
			return;
		}

		segmentTimer3 -= Time::deltaTime();
		if (segmentTimer3 < 0.0f)
		{
			segmentTimer3 = 0.13f;

			PerformCleaningEvent performCleaning;
			performCleaning.playerEntity = tutPlayerEntity;
			postEvent(performCleaning);
		}

		return;
	}

	isSegmentFinished = true;
}

void TutorialLayer::segment4()
{
	auto& player          = registry->get<Player>(tutPlayerEntity);
	auto& playerTransform = registry->get<component::Transform>(tutPlayerEntity);
	auto& cameraTransform = registry->get<component::Transform>(tutCameraEntity);

	if (initSegment)
	{
		TutorialSegmentStartedEvent segmentStarted;
		segmentStarted.segmentNum = 4;
		postEvent(segmentStarted);

		currentSegment    = TutorialSegment::segment4;
		initSegment       = false;
		isSegmentFinished = false;

		if (!fourthSegmentInit)
		{
			fourthSegmentInit = true;

			playerSegment4    = &player;
			playerSegment4Pos = playerTransform.getPosition();
			playerSegment4Rot = playerTransform.getRotation();
			cameraSegment4Rot = cameraTransform.getRotation();
		}

		player = *playerSegment4;
		playerTransform.setPosition(playerSegment4Pos);
		playerTransform.setRotation(playerSegment4Rot);
		cameraTransform.setRotation(cameraSegment4Rot);

		segmentTimer1 = 1.0f;
		segmentTimer2 = 3.0f; //"down here you'll find the garbage bin and cannon"
		segmentTimer3 = 0.8f; //delay before grabbing garbage ball
		segmentTimer4 = 0.7f; //delay before moving towards the cannon
		segmentTimer5 = 0.9f; //"load the cannon by pressing X"
		segmentTimer6 = 6.0f; //"cannon fires on a timer. push it to aim at different garbage piles by pressing X"
		segmentTimer7 = 8.0f; //delay before ending segment
		segmentTimer8 = 0.0f;

		segmentBool1 = true;
		segmentBool2 = true;
		segmentBool3 = true;
		segmentBool4 = true;
		segmentBool5 = true;
		segmentBool6 = true;
		segmentBool7 = true;
		segmentBool8 = true;
		segmentBool9 = true;

		//reset cannon variables to initial values
		auto cannonView = registry->view<Cannon>();
		for (auto& cannonEntity : cannonView)
		{
			auto& cannon          = registry->get<Cannon>(cannonEntity);
			auto& cannonTransform = registry->get<component::Transform>(cannonEntity);

			if (cannon.team == Team::blue)
			{
				cannon.trackPosition = initialCannonTrackPos;
				cannonTransform.setPosition(initialCannonPos);
			}
		}
	}

	segmentTimer1 -= Time::deltaTime();
	if (segmentTimer1 > 0.0f)
		return;

	//move to the side of the middle garbage pile
	if (segmentBool1)
	{
		glm::vec3 targetPos = glm::vec3(2.0f, playerTransform.getPositionY(), -5.28f);
		bool isFinished;

		movePlayerTowardPos(targetPos, &isFinished);
		if (!isFinished)
			return;
		else
		{
			segmentBool1 = false;
			cameraTransform.rotate(glm::vec3(30.0f, 0.0f, 0.0f)); //TODO: rotate over time

			//drop mop
			CancelButtonPressedEvent cancelButtonPressed;
			cancelButtonPressed.playerEntity = tutPlayerEntity;
			postEvent(cancelButtonPressed);
		}
	}

	//move to the top of the stairs to the lower deck
	if (segmentBool2)
	{
		glm::vec3 targetPos = glm::vec3(-13.76f, playerTransform.getPositionY(), -4.84f);
		bool isFinished;

		movePlayerTowardPos(targetPos, &isFinished);
		if (!isFinished)
			return;
		else
		{
			segmentBool2 = false;
			playerTransform.rotate(glm::vec3(0.0f, 88.0f, 0.0f)); //TODO: rotate over time
		}
	}

	//move down the stairs to the lower deck
	if (segmentBool3)
	{
		glm::vec3 targetPos = glm::vec3(-13.76f, -3.17f, 0.05f);
		bool isFinished;

		movePlayerTowardPos(targetPos, &isFinished);
		if (!isFinished)
			return;
		else
		{
			segmentBool3 = false;
			playerTransform.rotate(glm::vec3(0.0f, 110.0f, 0.0f)); //TODO: rotate over time
		}
	}

	segmentTimer2 -= Time::deltaTime();
	if (segmentTimer2 > 0.0f)
		return;

	//move to the garbage bin
	if (segmentBool4)
	{
		glm::vec3 targetPos = glm::vec3(-6.91f, playerTransform.getPositionY(), -4.6f);
		bool isFinished;

		movePlayerTowardPos(targetPos, &isFinished);
		if (!isFinished)
			return;
		else
			segmentBool4 = false;
	}

	if (segmentBool5)
	{
		segmentTimer3 -= Time::deltaTime();
		if (segmentTimer3 > 0.0f)
			return;
		else
		{
			segmentBool5 = false;

			//grab a cannonball
			PlayerInteractionRequestEvent playerInteractionRequest;
			playerInteractionRequest.playerEntity           = tutPlayerEntity;
			playerInteractionRequest.itemClassificatonToUse = PlayerItemClassification::any;
			postEvent(playerInteractionRequest);
		}
	}

	segmentTimer4 -= Time::deltaTime();
	if (segmentTimer4 > 0.0f)
		return;

	//rotate towards cannon
	if (segmentBool6)
	{
		segmentBool6 = false;
		playerTransform.rotate(glm::vec3(0.0f, -50.0f, 0.0f)); //TODO: rotate over time
		return;
	}

	//move towards the cannon
	if (segmentBool7)
	{
		glm::vec3 targetPos = glm::vec3(-4.64f, playerTransform.getPositionY(), 0.51f);
		bool isFinished;

		movePlayerTowardPos(targetPos, &isFinished);
		if (!isFinished)
			return;
		else
			segmentBool7 = false;
	}

	segmentTimer5 -= Time::deltaTime();
	if (segmentTimer5 > 0.0f)
		return;

	if (segmentBool8)
	{
		segmentBool8 = false;

		//load the cannon
		PlayerInteractionRequestEvent playerInteractionRequest;
		playerInteractionRequest.playerEntity           = tutPlayerEntity;
		playerInteractionRequest.itemClassificatonToUse = PlayerItemClassification::any;
		postEvent(playerInteractionRequest);
	}

	segmentTimer6 -= Time::deltaTime();
	if (segmentTimer6 > 0.0f)
		return;

	//push the cannon
	PlayerInteractionRequestEvent playerInteractionRequest;
	playerInteractionRequest.playerEntity           = tutPlayerEntity;
	playerInteractionRequest.itemClassificatonToUse = PlayerItemClassification::any;
	postEvent(playerInteractionRequest);

	segmentTimer7 -= Time::deltaTime();
	if (segmentTimer7 > 0.0f)
		return;

	isSegmentFinished = true;
}

void TutorialLayer::segment5()
{
	auto& player          = registry->get<Player>(tutPlayerEntity);
	auto& playerTransform = registry->get<component::Transform>(tutPlayerEntity);
	auto& cameraTransform = registry->get<component::Transform>(tutCameraEntity);

	if (initSegment)
	{
		TutorialSegmentStartedEvent segmentStarted;
		segmentStarted.segmentNum = 5;
		postEvent(segmentStarted);

		currentSegment    = TutorialSegment::segment5;
		initSegment       = false;
		isSegmentFinished = false;

		//drop any items
		CancelButtonPressedEvent cancelButtonPressed;
		cancelButtonPressed.playerEntity = tutPlayerEntity;
		postEvent(cancelButtonPressed);

		if (!fifthSegmentInit)
		{
			fifthSegmentInit = true;

			playerSegment5    = &player;

			//same position and rotation as the first segment (in captain's quarters)
			playerSegment5Pos = playerSegment1Pos;
			playerSegment5Rot = playerSegment1Rot;
			cameraSegment5Rot = cameraSegment1Rot;
		}

		player = *playerSegment5;
		playerTransform.setPosition(playerSegment5Pos);
		playerTransform.setRotation(playerSegment5Rot);
		cameraTransform.setRotation(cameraSegment5Rot);

		segmentTimer1 = 2.0f; //"alright lets go grab some gloop"
		segmentTimer2 = 0.7f; //delay before grabbing gloop
		segmentTimer3 = 0.6f; //delay before walking towards captains quarters exit
		segmentTimer4 = 7.0f; //"when holding gloop, you can see which piles on the enemy ship can be glooped"
		segmentTimer5 = 1.3f; //delay before using gloop
		segmentTimer6 = 9.0f; //"each bottle has 2 uses, and piles stay glooped for one full cleaning cycle"
		segmentTimer7 = 0.0f;
		segmentTimer8 = 0.0f;

		segmentBool1  = true;
		segmentBool2  = true;
		segmentBool3  = true;
		segmentBool4  = true;
		segmentBool5  = true;
		segmentBool6  = true;
		segmentBool7  = true;
		segmentBool8  = true;
		segmentBool9  = true;
		segmentBool10 = true;

		//ensure gloop starts at spawn
		auto carryableItemView = registry->view<CarryableItem>();
		for (auto itemEntity : carryableItemView)
		{
			auto& carryable = registry->get<CarryableItem>(itemEntity);
			auto& carryableTransform = registry->get<component::Transform>(itemEntity);

			if (   carryable.type == CarryableItemType::gloop 
				&& carryable.hasBeenCarried == false
				&& carryable.team == player.team
				&& carryableTransform.getPositionY() > -999.0f) //ensure it's not a prefab entity
			{
				carryableTransform.setPosition(glm::vec3(9.46f, 2.08f, -5.95f));

				carryableTransform.setRotation(glm::vec3(0.0f, 0.0f, 0.0f));
				carryableTransform.rotate(glm::vec3(0.0f, 83.82f, 0.0f));
			}
		}
	}

	segmentTimer1 -= Time::deltaTime();
	if (segmentTimer1 > 0.0f)
		return;

	//move to the gloop spawn
	if (segmentBool1)
	{
		glm::vec3 targetPos = glm::vec3(11.36f, playerTransform.getPositionY(), -5.71f);
		bool isFinished;

		movePlayerTowardPos(targetPos, &isFinished);
		if (!isFinished)
			return;
		else
			segmentBool1 = false;
	}

	//rotate towards gloop spawn
	if (segmentBool2)
	{
		segmentBool2 = false;
		cameraTransform.rotate(glm::vec3(24.0f, 0.0f, 0.0f)); //TODO: rotate over time
		playerTransform.rotate(glm::vec3(0.0f, -7.0f, 0.0f)); //TODO: rotate over time
	}

	segmentTimer2 -= Time::deltaTime();
	if (segmentTimer2 > 0.0f)
		return;

	if (segmentBool3)
	{
		segmentBool3 = false;

		//grab gloop
		PlayerInteractionRequestEvent playerInteractionRequest;
		playerInteractionRequest.playerEntity           = tutPlayerEntity;
		playerInteractionRequest.itemClassificatonToUse = PlayerItemClassification::any;
		postEvent(playerInteractionRequest);
	}

	segmentTimer3 -= Time::deltaTime();
	if (segmentTimer3 > 0.0f)
		return;

	//move towards captains quarters exit
	if (segmentBool4)
	{
		glm::vec3 targetPos = glm::vec3(10.56f, playerTransform.getPositionY(), -2.58f);
		bool isFinished;

		movePlayerTowardPos(targetPos, &isFinished);
		if (!isFinished)
			return;
		else
			segmentBool4 = false;
	}

	//move towards the middle of the ship
	if (segmentBool5)
	{
		glm::vec3 targetPos = glm::vec3(2.35f, playerTransform.getPositionY(), -2.58f);
		bool isFinished;

		movePlayerTowardPos(targetPos, &isFinished);
		if (!isFinished)
			return;
		else
			segmentBool5 = false;
	}

	//rotate towards enemy ship
	if (segmentBool6)
	{
		segmentBool6 = false;
		playerTransform.rotate(glm::vec3(0.0f, 97.0f, 0.0f)); //TODO: rotate over time
	}

	segmentTimer4 -= Time::deltaTime();
	if (segmentTimer4 > 0.0f)
		return;

	//move towards the plank connecting the two ships
	if (segmentBool7)
	{
		glm::vec3 targetPos = glm::vec3(-5.7f, playerTransform.getPositionY(), 1.2f);
		bool isFinished;

		movePlayerTowardPos(targetPos, &isFinished);
		if (!isFinished)
			return;
		else
			segmentBool7 = false;
	}

	//move across the plank to the enemy's ship
	if (segmentBool8)
	{
		glm::vec3 targetPos = glm::vec3(-5.7f, playerTransform.getPositionY(), 19.21f);
		bool isFinished;

		movePlayerTowardPos(targetPos, &isFinished);
		if (!isFinished)
			return;
		else
			segmentBool8 = false;
	}

	//move beside the enemy's middle garbage pile
	if (segmentBool9)
	{
		glm::vec3 targetPos = glm::vec3(1.1f, playerTransform.getPositionY(), 22.55f);
		bool isFinished;

		movePlayerTowardPos(targetPos, &isFinished);
		if (!isFinished)
			return;
		else
			segmentBool9 = false;
	}

	//rotate towards enemy's middle garbage pile
	if (segmentBool10)
	{
		segmentBool10 = false;
		cameraTransform.rotate(glm::vec3(-64.0f, 0.0f, 0.0f)); //TODO: rotate over time
	}

	segmentTimer5 -= Time::deltaTime();
	if (segmentTimer5 > 0.0f)
		return;

	//use gloop on enemy's middle garbage pile
	PlayerInteractionRequestEvent playerInteractionRequest;
	playerInteractionRequest.playerEntity           = tutPlayerEntity;
	playerInteractionRequest.itemClassificatonToUse = PlayerItemClassification::any;
	postEvent(playerInteractionRequest);

	segmentTimer6 -= Time::deltaTime();
	if (segmentTimer6 > 0.0f)
		return;

	isSegmentFinished = true;
}

void TutorialLayer::segment6()
{
	auto& player          = registry->get<Player>(tutPlayerEntity);
	auto& playerTransform = registry->get<component::Transform>(tutPlayerEntity);
	auto& cameraTransform = registry->get<component::Transform>(tutCameraEntity);

	if (initSegment)
	{
		TutorialSegmentStartedEvent segmentStarted;
		segmentStarted.segmentNum = 6;
		postEvent(segmentStarted);

		currentSegment    = TutorialSegment::segment6;
		initSegment       = false;
		isSegmentFinished = false;

		if (!sixthSegmentInit)
		{
			sixthSegmentInit = true;

			playerSegment6    = &player;
			playerSegment6Pos = playerTransform.getPosition();
			playerSegment6Rot = playerTransform.getRotation();
			cameraSegment6Rot = cameraTransform.getRotation();
		}

		player = *playerSegment6;
		playerTransform.setPosition(playerSegment6Pos);
		playerTransform.setRotation(playerSegment6Rot);
		cameraTransform.setRotation(cameraSegment6Rot);

		segmentTimer1 = 0.9f; //delay before moving toward bottle spawn
		segmentTimer2 = 9.0f; //"there's a throwable bottle that spawns between the ships, use it to disrupt the enemy"
		segmentTimer3 = 0.8f; //delay after picking up the bottle (before turning towards the enemy)
		segmentTimer4 = 3.0f; //"press right trigger to throw the bottle"
		segmentTimer5 = 2.0f; //"bring the mop back to your ship" (will be continued as the player walks back to their ship so this doesn't need to be the full duration)
		segmentTimer6 = 4.0f; //"press B or right bumper to drop"
		segmentTimer7 = 0.0f;
		segmentTimer8 = 0.0f;
		segmentTimer9 = 0.0f;

		segmentBool1  = true;
		segmentBool2  = true;
		segmentBool3  = true;
		segmentBool4  = true;
		segmentBool5  = true;
		segmentBool6  = true;
		segmentBool7  = true;
		segmentBool8  = true;
		segmentBool9  = true;
		segmentBool10 = true;
		segmentBool11 = true;

		//ensure throwable bottle starts at spawn
		auto carryableItemView = registry->view<CarryableItem>();
		for (auto itemEntity : carryableItemView)
		{
			auto& carryable          = registry->get<CarryableItem>(itemEntity);
			auto& carryableTransform = registry->get<component::Transform>(itemEntity);

			if (carryable.type == CarryableItemType::throwableBottle && carryableTransform.getPositionY() > -999.0f) //ensure it's not a prefab entity
				carryableTransform.setPosition(glm::vec3(-7.04f, 0.61f, 10.14f));
		}

		//get player 2 (enemy that's standing still waiting to be hit by a bottle)
		entt::entity playerTwoEntity;
		
		auto playerView = registry->view<Player>();
		for (auto& playerEntity : playerView)
		{
			auto& player = registry->get<Player>(playerEntity);

			if (player.playerNum == PlayerNumber::Two)
				playerTwoEntity = playerEntity;
		}

		auto& playerTwo          = registry->get<Player>(playerTwoEntity);
		auto& playerTwoTransform = registry->get<component::Transform>(playerTwoEntity);

		bool playerTwoHasPrimary   = playerTwo.primaryCarriedItem   != entt::null;
		bool playerTwoHasSecondary = playerTwo.secondaryCarriedItem != entt::null;

		carryableItemView = registry->view<CarryableItem>();
		for (auto itemEntity : carryableItemView)
		{
			auto& carryable          = registry->get<CarryableItem>(itemEntity);
			auto& carryableTransform = registry->get<component::Transform>(itemEntity);

			//force player 2 to hold a mop
			if (   !playerTwoHasPrimary
				&& carryable.type == CarryableItemType::mop
				&& carryable.team == playerTwo.team
				&& carryableTransform.getPositionY() > -999.0f) //ensure it's not a prefab entity
			{
				PlayerForceItemPickUpEvent forceItemPickUp;
				forceItemPickUp.playerEntity = playerTwoEntity;
				forceItemPickUp.itemEntity   = itemEntity;
				postEvent(forceItemPickUp);

				playerTwoHasPrimary = true;
			}
			//force player 2 to hold a cleaning solution
			else if (   !playerTwoHasSecondary 
				     && carryable.type == CarryableItemType::cleaningSolution 
				     && carryable.team == playerTwo.team
				     && carryableTransform.getPositionY() > -999.0f) //ensure it's not a prefab entity
			{
				PlayerForceItemPickUpEvent forceItemPickUp;
				forceItemPickUp.playerEntity = playerTwoEntity;
				forceItemPickUp.itemEntity   = itemEntity;
				postEvent(forceItemPickUp);

				playerTwoHasSecondary = true;
			}
		}

		//reset player two position
		playerTwoTransform.setPosition(
			glm::vec3(
				3.75f, 
				playerTwoTransform.getPositionY(), 
				22.1f));
	}

	//rotate towards friendly ship
	if (segmentBool1)
	{
		segmentBool1 = false;
		cameraTransform.rotate(glm::vec3(45.0f, 0.0f, 0.0f)); //TODO: rotate over time
		playerTransform.rotate(glm::vec3(0.0f, 194.0f, 0.0f)); //TODO: rotate over time
	}

	segmentTimer1 -= Time::deltaTime();
	if (segmentTimer1 > 0.0f)
		return;

	//move towards the plank between the two ships
	if (segmentBool2)
	{
		glm::vec3 targetPos = glm::vec3(-5.7f, playerTransform.getPositionY(), 19.21f);
		bool isFinished;

		movePlayerTowardPos(targetPos, &isFinished);
		if (!isFinished)
			return;
		else
			segmentBool2 = false;
	}

	//move towards the bottle spawn, at the middle of the plank
	if (segmentBool3)
	{
		glm::vec3 targetPos = glm::vec3(-6.6f, playerTransform.getPositionY(), 11.5f);
		bool isFinished;

		movePlayerTowardPos(targetPos, &isFinished);
		if (!isFinished)
			return;
		else
		{
			segmentBool3 = false;

			//look at the bottle
			cameraTransform.rotate(glm::vec3(-41.0f, 0.0f, 0.0f)); //TODO: rotate over time
		}
	}

	segmentTimer2 -= Time::deltaTime();
	if (segmentTimer2 > 0.0f)
		return;

	//pick up the bottle
	{
		PlayerInteractionRequestEvent playerInteractionRequest;
		playerInteractionRequest.playerEntity = tutPlayerEntity;
		playerInteractionRequest.itemClassificatonToUse = PlayerItemClassification::any;
		postEvent(playerInteractionRequest);
	}

	segmentTimer3 -= Time::deltaTime();
	if (segmentTimer3 > 0.0f)
		return;

	//rotate towards the enemy
	if (segmentBool4)
	{
		segmentBool4 = false;
		cameraTransform.rotate(glm::vec3(34.0f, 0.0f, 0.0f)); //TODO: rotate over time
		playerTransform.rotate(glm::vec3(0.0f, -113.0f, 0.0f)); //TODO: rotate over time
	}

	//move across the plank and back on the enemy's ship
	if (segmentBool5)
	{
		glm::vec3 targetPos = glm::vec3(-5.7f, playerTransform.getPositionY(), 19.21f);
		bool isFinished;

		movePlayerTowardPos(targetPos, &isFinished);
		if (!isFinished)
			return;
		else
			segmentBool5 = false;
	}

	//move toward the enemy
	if (segmentBool6)
	{
		glm::vec3 targetPos = glm::vec3(1.4f, playerTransform.getPositionY(), 21.6f);
		bool isFinished;

		movePlayerTowardPos(targetPos, &isFinished);
		if (!isFinished)
			return;
		else
			segmentBool6 = false;
	}

	segmentTimer4 -= Time::deltaTime();
	if (segmentTimer4 > 0.0f)
		return;

	//throw bottle
	{
		PlayerInteractionRequestEvent playerInteractionRequest;
		playerInteractionRequest.playerEntity           = tutPlayerEntity;
		playerInteractionRequest.itemClassificatonToUse = PlayerItemClassification::primary;
		postEvent(playerInteractionRequest);
	}

	segmentTimer5 -= Time::deltaTime();
	if (segmentTimer5 > 0.0f)
		return;

	//rotate towards the mop on the ground
	if (segmentBool7)
	{
		segmentBool7 = false;
		cameraTransform.rotate(glm::vec3(-44.0f, 0.0f, 0.0f)); //TODO: rotate over time
	}

	//move to pick up mop
	if (segmentBool8)
	{
		glm::vec3 targetPos = glm::vec3(1.9f, playerTransform.getPositionY(), 19.3f);
		bool isFinished;

		movePlayerTowardPos(targetPos, &isFinished);
		if (!isFinished)
			return;
		else
		{
			segmentBool8 = false;

			//rotate towards friendly ship
			cameraTransform.rotate(glm::vec3(24.0f, 0.0f, 0.0f)); //TODO: rotate over time
			playerTransform.rotate(glm::vec3(0.0f, 120.0f, 0.0f)); //TODO: rotate over time
		}
	}

	//move towards the plank between the two ships
	if (segmentBool9)
	{
		glm::vec3 targetPos = glm::vec3(-5.7f, playerTransform.getPositionY(), 19.21f);
		bool isFinished;

		movePlayerTowardPos(targetPos, &isFinished);
		if (!isFinished)
			return;
		else
			segmentBool9 = false;
	}

	//move across the plank to the friendly ship
	if (segmentBool10)
	{
		glm::vec3 targetPos = glm::vec3(-5.7f, playerTransform.getPositionY(), 0.92f);
		bool isFinished;

		movePlayerTowardPos(targetPos, &isFinished);
		if (!isFinished)
			return;
		else
			segmentBool10 = false;
	}

	//move to the front of the friendly ship
	if (segmentBool11)
	{
		glm::vec3 targetPos = glm::vec3(13.6f, playerTransform.getPositionY(), -4.4f);
		bool isFinished;

		movePlayerTowardPos(targetPos, &isFinished);
		if (!isFinished)
			return;
		else
			segmentBool11 = false;
	}

	segmentTimer6 -= Time::deltaTime();
	if (segmentTimer6 > 0.0f)
		return;

	//drop the mop
	PlayerDropItemRequestEvent playerDropItemRequest;
	playerDropItemRequest.playerEntity             = tutPlayerEntity;
	playerDropItemRequest.itemClassificationToDrop = PlayerItemClassification::primary;
	playerDropItemRequest.forceDrop                = false;
	postEvent(playerDropItemRequest);

	isSegmentFinished = true;
}

void TutorialLayer::outro()
{
	auto& playerTransform = registry->get<component::Transform>(tutPlayerEntity);

	if (initSegment)
	{
		TutorialSegmentStartedEvent segmentStarted;
		segmentStarted.segmentNum = 7;
		postEvent(segmentStarted);

		currentSegment    = TutorialSegment::outro;
		initSegment       = false;
		isSegmentFinished = false;

		segmentTimer1 = 7.0f; //"Ill see yall later. SUU WHOOP"
	}

	segmentTimer1 -= Time::deltaTime();
	if (segmentTimer1 > 0.0f)
		return;

	Application::get().changeScene("MainMenuScene");
}

void TutorialLayer::movePlayerTowardPos(glm::vec3 a_targetPos, bool* a_isFinished)
{
	auto& playerTransform = registry->get<component::Transform>(tutPlayerEntity);

	float dist = glm::distance(playerTransform.getPosition(), a_targetPos);
	float interpolationParam = 0.1f / dist;

	if (interpolationParam < 1.0f)
	{
		*a_isFinished = false;

		playerTransform.setPosition(
			glm::mix(
				playerTransform.getPosition(),
				a_targetPos,
				interpolationParam));
	}
	else
	{
		*a_isFinished = true;

		playerTransform.setPosition(a_targetPos);
	}
}

void TutorialLayer::moveToNextSegment()
{
	initSegment = true;
	switch (currentSegment)
	{
	case TutorialSegment::segment1:
	{
		currentSegmentFunc = &TutorialLayer::segment2;
		break;
	}
	case TutorialSegment::segment2:
	{
		currentSegmentFunc = &TutorialLayer::segment3;
		break;
	}
	case TutorialSegment::segment3:
	{
		currentSegmentFunc = &TutorialLayer::segment4;
		break;
	}
	case TutorialSegment::segment4:
	{
		currentSegmentFunc = &TutorialLayer::segment5;
		break;
	}
	case TutorialSegment::segment5:
	{
		currentSegmentFunc = &TutorialLayer::segment6;
		break;
	}
	case TutorialSegment::segment6:
	{
		currentSegmentFunc = &TutorialLayer::outro;
		break;
	}
	}
}

void TutorialLayer::repeatSegment()
{
	initSegment = true;
	switch (currentSegment)
	{
	case TutorialSegment::segment1:
	{
		currentSegmentFunc = &TutorialLayer::segment1;
		break;
	}
	case TutorialSegment::segment2:
	{
		currentSegmentFunc = &TutorialLayer::segment2;
		break;
	}
	case TutorialSegment::segment3:
	{
		currentSegmentFunc = &TutorialLayer::segment3;
		break;
	}
	case TutorialSegment::segment4:
	{
		currentSegmentFunc = &TutorialLayer::segment4;
		break;
	}
	case TutorialSegment::segment5:
	{
		currentSegmentFunc = &TutorialLayer::segment5;
		break;
	}
	case TutorialSegment::segment6:
	{
		currentSegmentFunc = &TutorialLayer::segment6;
		break;
	}
	}
}