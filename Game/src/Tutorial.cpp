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
#include "ScrollingTextureLayer.h"
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

			if (camera.player != PlayerNumber::One)
				registry->destroy(cameraEntity);
		}
	}

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
	if (initSegment)
	{
		currentSegment    = TutorialSegment::intro;
		initSegment       = false;
		isSegmentFinished = false;

		segmentTimer1 = 8.0f;
	}

	auto& playerTransform = registry->get<component::Transform>(tutPlayerEntity);

	segmentTimer1 -= Time::deltaTime();
	if (segmentTimer1 > 0.0f)
		return;

	initSegment = true;
	currentSegmentFunc = &TutorialLayer::segment1;
}

void TutorialLayer::segment1()
{
	if (initSegment)
	{
		currentSegment    = TutorialSegment::segment1;
		initSegment       = false;
		isSegmentFinished = false;
	}

	auto& playerTransform = registry->get<component::Transform>(tutPlayerEntity);

	

	isSegmentFinished = true;
}

void TutorialLayer::segment2()
{
	if (initSegment)
	{
		currentSegment    = TutorialSegment::segment2;
		initSegment       = false;
		isSegmentFinished = false;
	}

	auto& playerTransform = registry->get<component::Transform>(tutPlayerEntity);



	isSegmentFinished = true;
}

void TutorialLayer::segment3()
{
	if (initSegment)
	{
		currentSegment    = TutorialSegment::segment3;
		initSegment       = false;
		isSegmentFinished = false;
	}

	auto& playerTransform = registry->get<component::Transform>(tutPlayerEntity);



	isSegmentFinished = true;
}

void TutorialLayer::segment4()
{
	if (initSegment)
	{
		currentSegment    = TutorialSegment::segment4;
		initSegment       = false;
		isSegmentFinished = false;
	}

	auto& playerTransform = registry->get<component::Transform>(tutPlayerEntity);



	isSegmentFinished = true;
}

void TutorialLayer::segment5()
{
	if (initSegment)
	{
		currentSegment    = TutorialSegment::segment5;
		initSegment       = false;
		isSegmentFinished = false;
	}

	auto& playerTransform = registry->get<component::Transform>(tutPlayerEntity);



	isSegmentFinished = true;
}

void TutorialLayer::segment6()
{
	if (initSegment)
	{
		currentSegment    = TutorialSegment::segment6;
		initSegment       = false;
		isSegmentFinished = false;
	}

	auto& playerTransform = registry->get<component::Transform>(tutPlayerEntity);



	isSegmentFinished = true;
}

void TutorialLayer::outro()
{
	if (initSegment)
	{
		currentSegment    = TutorialSegment::outro;
		initSegment       = false;
		isSegmentFinished = false;

		segmentTimer1 = 8.0f;
	}

	auto& playerTransform = registry->get<component::Transform>(tutPlayerEntity);

	segmentTimer1 -= Time::deltaTime();
	if (segmentTimer1 > 0.0f)
		return;

	Application::get().changeScene("MainMenuScene");
}