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
	firstFrame = true;

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

	if (initSegment)
	{
		currentSegment    = TutorialSegment::segment1;
		initSegment       = false;
		isSegmentFinished = false;

		playerTransform.setPosition(glm::vec3(15.45f, 1.00f, -3.0f));
		playerTransform.rotate(glm::vec3(0.0f, 88.0f, 0.0f) - playerTransform.getRotationEuler());

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

	movePlayerToPos(targetPos1, &isFinished1);
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
	auto& playerTransform = registry->get<component::Transform>(tutPlayerEntity);
	auto& cameraTransform = registry->get<component::Transform>(tutCameraEntity);

	if (initSegment)
	{
		currentSegment    = TutorialSegment::segment2;
		initSegment       = false;
		isSegmentFinished = false;

		playerTransform.setPosition(glm::vec3(12.46f, 1.00f, -4.13f));
		playerTransform.rotate(glm::vec3(0.0f, 67.9f, 0.0f) - playerTransform.getRotationEuler());

		cameraTransform.rotate(glm::vec3(-15.0f, 0.0f, 0.0f) - cameraTransform.getRotationEuler());

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

		movePlayerToPos(targetPos, &isFinished);
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

		movePlayerToPos(targetPos, &isFinished);
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

		movePlayerToPos(targetPos, &isFinished);
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
		currentSegment = TutorialSegment::segment3;
		initSegment = false;
		isSegmentFinished = false;

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
	auto& playerTransform = registry->get<component::Transform>(tutPlayerEntity);
	auto& cameraTransform = registry->get<component::Transform>(tutCameraEntity);

	if (initSegment)
	{
		currentSegment    = TutorialSegment::segment4;
		initSegment       = false;
		isSegmentFinished = false;

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
	}

	segmentTimer1 -= Time::deltaTime();
	if (segmentTimer1 > 0.0f)
		return;

	//move to the side of the middle garbage pile
	if (segmentBool1)
	{
		glm::vec3 targetPos = glm::vec3(2.0f, playerTransform.getPositionY(), -5.28f);
		bool isFinished;

		movePlayerToPos(targetPos, &isFinished);
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

		movePlayerToPos(targetPos, &isFinished);
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

		movePlayerToPos(targetPos, &isFinished);
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

		movePlayerToPos(targetPos, &isFinished);
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

		movePlayerToPos(targetPos, &isFinished);
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
	auto& playerTransform = registry->get<component::Transform>(tutPlayerEntity);
	auto& cameraTransform = registry->get<component::Transform>(tutCameraEntity);

	if (initSegment)
	{
		currentSegment    = TutorialSegment::segment5;
		initSegment       = false;
		isSegmentFinished = false;

		segmentTimer1 = 30.0f; //delay for audio
		segmentTimer2 = 0.0f;
		segmentTimer3 = 0.0f;
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
	}

	segmentTimer1 -= Time::deltaTime();
	if (segmentTimer1 > 0.0f)
		return;

	isSegmentFinished = true;
}

void TutorialLayer::segment6()
{
	auto& playerTransform = registry->get<component::Transform>(tutPlayerEntity);
	auto& cameraTransform = registry->get<component::Transform>(tutCameraEntity);

	if (initSegment)
	{
		currentSegment    = TutorialSegment::segment6;
		initSegment       = false;
		isSegmentFinished = false;

		segmentTimer1 = 30.0f; //delay for audio
		segmentTimer2 = 0.0f;
		segmentTimer3 = 0.0f;
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
	}

	segmentTimer1 -= Time::deltaTime();
	if (segmentTimer1 > 0.0f)
		return;

	isSegmentFinished = true;
}

void TutorialLayer::outro()
{
	auto& playerTransform = registry->get<component::Transform>(tutPlayerEntity);

	if (initSegment)
	{
		currentSegment    = TutorialSegment::outro;
		initSegment       = false;
		isSegmentFinished = false;

		segmentTimer1 = 6.0f; //"Ill see yall later have fun"
	}

	segmentTimer1 -= Time::deltaTime();
	if (segmentTimer1 > 0.0f)
		return;

	Application::get().changeScene("MainMenuScene");
}

void TutorialLayer::movePlayerToPos(glm::vec3 a_targetPos, bool* a_isFinished)
{
	auto& playerTransform = registry->get<component::Transform>(tutPlayerEntity);

	float dist = glm::distance(playerTransform.getPosition(), a_targetPos);
	float interpolationParam = 0.08f / dist;

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
		*a_isFinished = true;
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