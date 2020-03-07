#include "GameEndMenu.h"

using namespace oyl;

void GameEndLayer::onEnter()
{
	listenForEventCategory(EventCategory::Keyboard);
	listenForEventCategory(EventCategory::Gamepad);

	selectedMenuItemType = MenuOption::goToMainMenu;

	{
		auto cameraEntity = registry->create();

		auto& cameraTransform = registry->assign<component::Transform>(cameraEntity);
		cameraTransform.setPosition(glm::vec3(0.0f, 0.0f, 0.0f));

		auto& camera = registry->assign<component::Camera>(cameraEntity);
		camera.cullingMask = 0b1111;
	}

	{
		auto e = registry->create();

		auto& t = registry->assign<component::Transform>(e);
		t.setPosition(glm::vec3(0.0f, 0.0f, 0.0f));
		t.setScale(glm::vec3(10.0f, 10.0f, 1.0f));

		auto& so = registry->assign<component::EntityInfo>(e);
		so.name = "Game End Background";

		auto& gui = registry->assign<component::GuiRenderable>(e);
		gui.texture = Texture2D::cache("res/assets/textures/menus/MainMenuBackground.png");
	}

	{
		auto e = registry->create();

		auto& menuItem = registry->assign<MenuItem>(e);
		menuItem.type = MenuOption::goToMainMenu;

		auto& t = registry->assign<component::Transform>(e);
		t.setPosition(glm::vec3(0.0f, 1.0f, -1.0f));
		t.setScale(glm::vec3(1.0f, 1.0f, 1.0f));

		auto& so = registry->assign<component::EntityInfo>(e);
		so.name = "Go To Main Menu Prompt";

		auto& gui = registry->assign<component::GuiRenderable>(e);
		gui.texture = Texture2D::cache("res/assets/textures/menus/goToMainMenu.png");
	}

	{
		auto e = registry->create();

		auto& menuItem = registry->assign<MenuItem>(e);
		menuItem.type = MenuOption::playAgain;

		auto& t = registry->assign<component::Transform>(e);
		t.setPosition(glm::vec3(0.0f, -4.0f, -1.0f));
		t.setScale(glm::vec3(1.0f, 1.0f, 1.0f));

		auto& so = registry->assign<component::EntityInfo>(e);
		so.name = "Play Again Prompt";

		auto& gui = registry->assign<component::GuiRenderable>(e);
		gui.texture = Texture2D::cache("res/assets/textures/menus/playAgain.png");
	}
}

void GameEndLayer::onUpdate()
{
	auto menuItemsView = registry->view<MenuItem>();
	for (auto& menuItemEntity : menuItemsView)
	{
		auto& menuItem = registry->get<MenuItem>(menuItemEntity);
		auto& menuItemTransform = registry->get<component::Transform>(menuItemEntity);

		if (menuItem.type == selectedMenuItemType)
			menuItemTransform.setScale(glm::vec3(1.5f, 1.5f, 1.0f));
		else
			menuItemTransform.setScale(glm::vec3(1.0f));
	}
}

bool GameEndLayer::onEvent(const Event& event)
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

			switch (selectedMenuItemType)
			{
			case MenuOption::goToMainMenu:
			{
				selectedMenuItemType = MenuOption::playAgain;
				break;
			}
			case MenuOption::playAgain:
			{
				selectedMenuItemType = MenuOption::goToMainMenu;
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

			switch (selectedMenuItemType)
			{
			case MenuOption::goToMainMenu:
			{
				selectedMenuItemType = MenuOption::playAgain;
				break;
			}
			case MenuOption::playAgain:
			{
				selectedMenuItemType = MenuOption::goToMainMenu;
				break;
			}
			}

			break;
		}

		case oyl::Key::Enter:
		{
			switch (selectedMenuItemType)
			{
			case MenuOption::goToMainMenu:
			{
				Application::get().changeScene("MainScene");
				break;
			}
			case MenuOption::playAgain:
			{
				Application::get().changeScene("MainMenuScene");
				break;
			}
			}
		}
		}

		break;
	}

	case EventType::GamepadStickMoved:
	{
		auto evt = event_cast<GamepadStickMovedEvent>(event);

		//moved stick down
		if (evt.dy > 0.0f)
		{
			
		}
		//moved stick up
		else if (evt.dy < 0.0f)
		{
			
		}

		break;
	}

	case EventType::GamepadButtonPressed:
	{
		auto evt = event_cast<GamepadButtonPressedEvent>(event);

		switch (evt.button)
		{
		case Gamepad::Start:
		case Gamepad::A:
		{
			switch (selectedMenuItemType)
			{
			case MenuOption::goToMainMenu:
			{
				Application::get().changeScene("MainScene");
				break;
			}
			case MenuOption::playAgain:
			{
				Application::get().changeScene("MainMenuScene");
				break;
			}
			}

			break;
		}
		case Gamepad::B:
		{


			break;
		}
		}

		break;
	}
	}

	return false;
}

void GameEndLayer::onGuiRender()
{

}