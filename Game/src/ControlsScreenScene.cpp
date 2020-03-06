#include "ControlsScreenScene.h"

using namespace oyl;

void ControlsScreenLayer::onEnter()
{
	listenForEventCategory(EventCategory::Keyboard);
	listenForEventCategory(EventCategory::Gamepad);

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
		so.name = "Controls Screen Background";

		auto& gui = registry->assign<component::GuiRenderable>(e);
		gui.texture = Texture2D::cache("res/assets/textures/menus/ControlsScreen.png");
	}

	{
		auto e = registry->create();

		auto& menuItem = registry->assign<MenuItem>(e);
		menuItem.type = MenuOption::goToMainMenu;

		auto& t = registry->assign<component::Transform>(e);
		t.setPosition(glm::vec3(-6.45f, -3.8f, -1.0f));
		t.setScale(glm::vec3(1.2f, 1.2f, 1.0f));

		auto& so = registry->assign<component::EntityInfo>(e);
		so.name = "Back Prompt";

		auto& gui = registry->assign<component::GuiRenderable>(e);
		gui.texture = Texture2D::cache("res/assets/textures/menus/BackPrompt.png");
	}
}

void ControlsScreenLayer::onUpdate()
{
	
}

bool ControlsScreenLayer::onEvent(const Event& event)
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
		case Gamepad::Back:
		case Gamepad::B:
		{
			Application::get().changeScene("MainMenuScene");
			break;
		}
		}

		break;
	}
	}

	return false;
}

void ControlsScreenLayer::onGuiRender()
{

}