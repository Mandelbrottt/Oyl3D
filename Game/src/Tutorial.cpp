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
		case oyl::Key::Backspace:
		{
			Application::get().changeScene("MainMenuScene");
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

			break;
		}

		case Gamepad::Back:
		case Gamepad::B:
		{
			if (!isSegmentFinished)
				break;

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

		segmentTimer1 = 1.0f; //6
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
		segmentTimer1 = 1.0f; //4
		segmentTimer2 = 1.5f; //2.5
		segmentTimer3 = 1.2f;
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

		segmentTimer1 = 1.0f; //5
		segmentTimer2 = 0.4f;
		segmentTimer3 = 0.4f;
		segmentTimer4 = 1.0f; //2
		segmentTimer5 = 0.5f;
		segmentTimer6 = 0.5f;
		segmentTimer7 = 0.5f;

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
	auto& playerTransform = registry->get<component::Transform>(tutPlayerEntity);
	auto& cameraTransform = registry->get<component::Transform>(tutCameraEntity);

	if (initSegment)
	{
		currentSegment    = TutorialSegment::segment3;
		initSegment       = false;
		isSegmentFinished = false;

		segmentTimer1 = 0.5f; //5
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

		segmentTimer1 = 0.0f;
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

		segmentTimer1 = 0.0f;
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

		segmentTimer1 = 0.0f;
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

		segmentTimer1 = 5.0f;
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