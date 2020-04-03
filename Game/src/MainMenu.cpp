#include "MainMenu.h"
#include "PersistentVariables.h"

using namespace oyl;

void MainMenuLayer::onEnter()
{
	delayBeforeAcceptingInputCountdown = 0.2f;

	listenForEventCategory(EventCategory::Keyboard);
	listenForEventCategory(EventCategory::Gamepad);

	{
		auto cameraEntity = registry->create();

		auto& cameraTransform = registry->assign<component::Transform>(cameraEntity);
		cameraTransform.setPosition(glm::vec3(0.0f, 0.0f, 0.0f));

		auto& camera = registry->assign<component::Camera>(cameraEntity);
		camera.cullingMask = 0b1111;

		auto& so = registry->assign<component::EntityInfo>(cameraEntity);
		so.name = "Camera";
	}

	{
		auto e = registry->create();

		auto& t = registry->assign<component::Transform>(e);
		t.setPosition(glm::vec3(0.0f, 3.65f, -1.0f));
		t.setScale(glm::vec3(4.0f, 4.0f, 1.0f));

		auto& so = registry->assign<component::EntityInfo>(e);
		so.name = "Washbucklers Logo";

		auto& gui = registry->assign<component::GuiRenderable>(e);
		gui.texture = Texture2D::cache("res/assets/textures/menus/Logo.png");
	}

	{
		auto e = registry->create();

		auto& menuItem = registry->assign<MenuItem>(e);
		menuItem.type = MenuOption::playGame;

		auto& t = registry->assign<component::Transform>(e);
		t.setPosition(glm::vec3(0.0f, 1.0f, -1.0f));
		t.setScale(glm::vec3(1.0f, 1.0f, 1.0f));

		auto& so = registry->assign<component::EntityInfo>(e);
		so.name = "Play Game Prompt";

		auto& gui = registry->assign<component::GuiRenderable>(e);
		gui.texture = Texture2D::cache("res/assets/textures/menus/PlayGamePrompt.png");
	}

	{
		auto e = registry->create();

		auto& menuItem = registry->assign<MenuItem>(e);
		menuItem.type = MenuOption::tutorial;

		auto& t = registry->assign<component::Transform>(e);
		t.setPosition(glm::vec3(0.0f, 0.2f, -1.0f));
		t.setScale(glm::vec3(1.0f, 1.0f, 1.0f));

		auto& so = registry->assign<component::EntityInfo>(e);
		so.name = "Tutorial Prompt";

		auto& gui = registry->assign<component::GuiRenderable>(e);
		gui.texture = Texture2D::cache("res/assets/textures/menus/TutorialPrompt.png");
	}

	{
		auto e = registry->create();

		auto& menuItem = registry->assign<MenuItem>(e);
		menuItem.type = MenuOption::controls;

		auto& t = registry->assign<component::Transform>(e);
		t.setPosition(glm::vec3(0.0f, -0.3f, -1.0f));
		t.setScale(glm::vec3(1.0f, 1.0f, 1.0f));

		auto& so = registry->assign<component::EntityInfo>(e);
		so.name = "Controls Prompt";

		auto& gui = registry->assign<component::GuiRenderable>(e);
		gui.texture = Texture2D::cache("res/assets/textures/menus/ControlsPrompt.png");
	}

	{
		auto e = registry->create();

		auto& menuItem = registry->assign<MenuItem>(e);
		menuItem.type = MenuOption::settings;

		auto& t = registry->assign<component::Transform>(e);
		t.setPosition(glm::vec3(0.0f, -1.6f, -1.0f));
		t.setScale(glm::vec3(1.0f, 1.0f, 1.0f));

		auto& so = registry->assign<component::EntityInfo>(e);
		so.name = "Settings Prompt";

		auto& gui = registry->assign<component::GuiRenderable>(e);
		gui.texture = Texture2D::cache("res/assets/textures/menus/SettingsPrompt.png");
	}

	{
		auto e = registry->create();

		auto& menuItem = registry->assign<MenuItem>(e);
		menuItem.type = MenuOption::credits;

		auto& t = registry->assign<component::Transform>(e);
		t.setPosition(glm::vec3(0.0f, -2.9f, -1.0f));
		t.setScale(glm::vec3(1.0f, 1.0f, 1.0f));

		auto& so = registry->assign<component::EntityInfo>(e);
		so.name = "Credits Prompt";

		auto& gui = registry->assign<component::GuiRenderable>(e);
		gui.texture = Texture2D::cache("res/assets/textures/menus/CreditsPrompt.png");
	}

	{
		auto e = registry->create();

		auto& menuItem = registry->assign<MenuItem>(e);
		menuItem.type = MenuOption::exit;

		auto& t = registry->assign<component::Transform>(e);
		t.setPosition(glm::vec3(0.0f, -4.2f, -1.0f));
		t.setScale(glm::vec3(1.0f, 1.0f, 1.0f));

		auto& so = registry->assign<component::EntityInfo>(e);
		so.name = "Exit Game Prompt";

		auto& gui = registry->assign<component::GuiRenderable>(e);
		gui.texture = Texture2D::cache("res/assets/textures/menus/ExitPrompt.png");
	}
}

void MainMenuLayer::onUpdate()
{
	delayBeforeAcceptingInputCountdown -= Time::deltaTime();
	changeMenuOptionCountdown          -= Time::deltaTime();

	auto menuItemsView = registry->view<MenuItem>();
	for (auto& menuItemEntity : menuItemsView)
	{
		auto& menuItem          = registry->get<MenuItem>(menuItemEntity);
		auto& menuItemTransform = registry->get<component::Transform>(menuItemEntity);

		if (menuItem.type == PersistentVariables::mainMenuSelectedOption)
			menuItemTransform.setScale(glm::vec3(1.05f, 1.05f, 1.05f));
		else
			menuItemTransform.setScale(glm::vec3(0.8f));
	}
}

bool MainMenuLayer::onEvent(const Event& event)
{
	switch (event.type)
	{
	case EventType::KeyPressed:
	{
		auto evt = event_cast<KeyPressedEvent>(event);

		switch (evt.keycode)
		{
		case oyl::Key::Down:
		case oyl::Key::S:
		{
			if (changeMenuOptionCountdown > 0.0f)
				break;

			changeMenuOptionCountdown = CHANGE_MENU_OPTION_DELAY;

			switch (PersistentVariables::mainMenuSelectedOption)
			{
			case MenuOption::playGame:
			{
				PersistentVariables::mainMenuSelectedOption = MenuOption::tutorial;
				break;
			}
			case MenuOption::tutorial:
			{
				PersistentVariables::mainMenuSelectedOption = MenuOption::controls;
				break;
			}
			case MenuOption::controls:
			{
				PersistentVariables::mainMenuSelectedOption = MenuOption::settings;
				break;
			}
			case MenuOption::settings:
			{
				PersistentVariables::mainMenuSelectedOption = MenuOption::credits;
				break;
			}
			case MenuOption::credits:
			{
				PersistentVariables::mainMenuSelectedOption = MenuOption::exit;
				break;
			}
			case MenuOption::exit:
			{
				PersistentVariables::mainMenuSelectedOption = MenuOption::playGame;
				break;
			}
			}

			break;
		}

		case oyl::Key::Up:
		case oyl::Key::W:
		{
			if (changeMenuOptionCountdown > 0.0f)
				break;

			changeMenuOptionCountdown = CHANGE_MENU_OPTION_DELAY;

			switch (PersistentVariables::mainMenuSelectedOption)
			{
			case MenuOption::playGame:
			{
				PersistentVariables::mainMenuSelectedOption = MenuOption::exit;
				break;
			}
			case MenuOption::tutorial:
			{
				PersistentVariables::mainMenuSelectedOption = MenuOption::playGame;
				break;
			}
			case MenuOption::controls:
			{
				PersistentVariables::mainMenuSelectedOption = MenuOption::tutorial;
				break;
			}
			case MenuOption::settings:
			{
				PersistentVariables::mainMenuSelectedOption = MenuOption::controls;
				break;
			}
			case MenuOption::credits:
			{
				PersistentVariables::mainMenuSelectedOption = MenuOption::settings;
				break;
			}
			case MenuOption::exit:
			{
				PersistentVariables::mainMenuSelectedOption = MenuOption::credits;
				break;
			}
			}

			break;
		}

		case oyl::Key::Enter:
		{
			if (delayBeforeAcceptingInputCountdown > 0.0f)
				break;

			switch (PersistentVariables::mainMenuSelectedOption)
			{
			case MenuOption::playGame:
			{
				Application::get().changeScene("MainScene");
				break;
			}
			case MenuOption::tutorial:
			{
				Application::get().changeScene("TutorialScene");
				break;
			}
			case MenuOption::controls:
			{
				Application::get().changeScene("ControlsScreenScene");
				break;
			}
			case MenuOption::settings:
			{
				Application::get().changeScene("SettingsMenuScene");
				break;
			}
			case MenuOption::credits:
			{
				Application::get().changeScene("CreditsScene");
				break;
			}
			case MenuOption::exit:
			{
				//TODO: close the game

				break;
			}
			}

			break;
		}
		}

		break;
	}

	case EventType::GamepadStickMoved:
	{
		auto evt = event_cast<GamepadStickMovedEvent>(event);

		if (evt.stick != Gamepad::LeftStick)
			break;
		if (changeMenuOptionCountdown > 0.0f)
			break;

		changeMenuOptionCountdown = CHANGE_MENU_OPTION_DELAY;

		//moved stick down
		if (evt.dy > 0.0f && Input::getGamepadLeftStick(evt.gid).y > 0.1f)
		{
			switch (PersistentVariables::mainMenuSelectedOption)
			{
			case MenuOption::playGame:
			{
				PersistentVariables::mainMenuSelectedOption = MenuOption::tutorial;
				break;
			}
			case MenuOption::tutorial:
			{
				PersistentVariables::mainMenuSelectedOption = MenuOption::controls;
				break;
			}
			case MenuOption::controls:
			{
				PersistentVariables::mainMenuSelectedOption = MenuOption::settings;
				break;
			}
			case MenuOption::settings:
			{
				PersistentVariables::mainMenuSelectedOption = MenuOption::credits;
				break;
			}
			case MenuOption::credits:
			{
				PersistentVariables::mainMenuSelectedOption = MenuOption::exit;
				break;
			}
			case MenuOption::exit:
			{
				PersistentVariables::mainMenuSelectedOption = MenuOption::playGame;
				break;
			}
			}
		}
		//moved stick up
		else if (evt.dy < 0.0f && Input::getGamepadLeftStick(evt.gid).y < -0.1f)
		{
			switch (PersistentVariables::mainMenuSelectedOption)
			{
			case MenuOption::playGame:
			{
				PersistentVariables::mainMenuSelectedOption = MenuOption::exit;
				break;
			}
			case MenuOption::tutorial:
			{
				PersistentVariables::mainMenuSelectedOption = MenuOption::playGame;
				break;
			}
			case MenuOption::controls:
			{
				PersistentVariables::mainMenuSelectedOption = MenuOption::tutorial;
				break;
			}
			case MenuOption::settings:
			{
				PersistentVariables::mainMenuSelectedOption = MenuOption::controls;
				break;
			}
			case MenuOption::credits:
			{
				PersistentVariables::mainMenuSelectedOption = MenuOption::settings;
				break;
			}
			case MenuOption::exit:
			{
				PersistentVariables::mainMenuSelectedOption = MenuOption::credits;
				break;
			}
			}
		}

		break;
	}

	case EventType::GamepadButtonPressed:
	{
		if (delayBeforeAcceptingInputCountdown > 0.0f)
			break;

		auto evt = event_cast<GamepadButtonPressedEvent>(event);

		switch (evt.button)
		{
		case Gamepad::Start:
		case Gamepad::A:
		{
			switch (PersistentVariables::mainMenuSelectedOption)
			{
			case MenuOption::playGame:
			{
				Application::get().changeScene("MainScene");
				break;
			}
			case MenuOption::tutorial:
			{
				Application::get().changeScene("TutorialScene");
				break;
			}
			case MenuOption::controls:
			{
				Application::get().changeScene("ControlsScreenScene");
				break;
			}
			case MenuOption::settings:
			{
				Application::get().changeScene("SettingsMenuScene");
				break;
			}
			case MenuOption::credits:
			{
				Application::get().changeScene("CreditsScene");
				break;
			}
			case MenuOption::exit:
			{
				//TODO: close the game

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

void MainMenuLayer::onGuiRender()
{
	/*ImGui::Begin("xdhaha");

	ImGui::SliderFloat("Force Speed", &forceSpeed, 0.1f, 10.0f);

	ImGui::End();*/
}