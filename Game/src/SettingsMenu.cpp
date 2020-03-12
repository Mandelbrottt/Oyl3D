#include "SettingsMenu.h"
#include "PersistentVariables.h"

using namespace oyl;

void SettingsMenuLayer::onEnter()
{
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
		t.setPosition(glm::vec3(0.0f, 0.0f, 0.0f));
		t.setScale(glm::vec3(10.0f, 10.0f, 1.0f));

		auto& so = registry->assign<component::EntityInfo>(e);
		so.name = "Settings Screen Background";

		auto& gui = registry->assign<component::GuiRenderable>(e);
		gui.texture = Texture2D::cache("res/assets/textures/menus/SettingsBackground.png");
	}

	for (int i = 0; i < 4; i++)
	{
		{
			auto e = registry->create();

			auto& knob = registry->assign<SensitivityKnob>(e);
			knob.controllerNum = i;

			auto& t = registry->assign<component::Transform>(e);
			t.setPosition(glm::vec3(1.0f, 4.0f - (i * 2.0f), -20.0f));
			t.setScale(glm::vec3(1.3f, 1.3f, 1.0f));

			auto& so = registry->assign<component::EntityInfo>(e);
			so.name = "Sensitivity Knob " + std::to_string(i + 1);

			auto& gui = registry->assign<component::GuiRenderable>(e);
			gui.texture = Texture2D::cache("res/assets/textures/menus/SensitivityKnob.png");
		}
		{
			auto e = registry->create();

			auto& t = registry->assign<component::Transform>(e);
			t.setPosition(glm::vec3(1.0f, 4.0f - (i * 2.0f), -15.0f));
			t.setScale(glm::vec3(2.0f, 1.0f, 1.0f));

			auto& so = registry->assign<component::EntityInfo>(e);
			so.name = "Sensitivity Slider " + std::to_string(i + 1);

			auto& gui = registry->assign<component::GuiRenderable>(e);
			gui.texture = Texture2D::cache("res/assets/textures/menus/SensitivitySlider.png");
		}
		{
			auto e = registry->create();

			auto& t = registry->assign<component::Transform>(e);
			t.setPosition(glm::vec3(-7.3f, 4.0f - (i * 2.0f), -10.0f));
			t.setScale(glm::vec3(1.0f, 1.0f, 1.0f));

			auto& so = registry->assign<component::EntityInfo>(e);
			so.name = "Player Number " + std::to_string(i + 1) + " Prompt";

			auto& gui = registry->assign<component::GuiRenderable>(e);
			gui.texture = Texture2D::cache("res/assets/textures/menus/P" + std::to_string(i + 1) + ".png");
		}
	}

	{
		auto e = registry->create();

		auto& menuItem = registry->assign<MenuItem>(e);

		auto& t = registry->assign<component::Transform>(e);
		t.setPosition(glm::vec3(-6.45f, -3.8f, -1.0f));
		t.setScale(glm::vec3(1.2f, 1.2f, 1.0f));

		auto& so = registry->assign<component::EntityInfo>(e);
		so.name = "Back Prompt";

		auto& gui = registry->assign<component::GuiRenderable>(e);
		gui.texture = Texture2D::cache("res/assets/textures/menus/BackPrompt.png");
	}
}

void SettingsMenuLayer::onUpdate()
{
	sliderChangeDelayCountdown -= Time::deltaTime();

	auto knobView = registry->view<SensitivityKnob>();
	for (auto& knobEntity : knobView)
	{
		auto& knob          = registry->get<SensitivityKnob>(knobEntity);
		auto& knobTransform = registry->get<component::Transform>(knobEntity);

		knobTransform.setPositionX(-5.36f + (1.272f * PersistentVariables::controllerSensitivities[knob.controllerNum]));
	}
}

bool SettingsMenuLayer::onEvent(const Event& event)
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
		/*case oyl::Key::Left:
		case oyl::Key::A:
		{
			if (sliderChangeDelayCountdown > 0.0f)
				break;

			sliderChangeDelayCountdown = SLIDER_CHANGE_DELAY;


			break;
		}
		case oyl::Key::Right:
		case oyl::Key::D:
		{
			if (sliderChangeDelayCountdown > 0.0f)
				break;

			sliderChangeDelayCountdown = SLIDER_CHANGE_DELAY;


			break;
		}*/
		}

		break;
	}

	case EventType::GamepadStickMoved:
	{
		auto evt = event_cast<GamepadStickMovedEvent>(event);

		if (evt.stick != Gamepad::LeftStick)
			break;
		if (sliderChangeDelayCountdown > 0.0f)
			break;

		sliderChangeDelayCountdown = SLIDER_CHANGE_DELAY;

		//moved stick left
		if (evt.dx > 0.0f && Input::getGamepadLeftStick(evt.gid).x < -0.1f)
		{
			if (PersistentVariables::controllerSensitivities[evt.gid] > 0)
				PersistentVariables::controllerSensitivities[evt.gid]--;
		}
		//moved stick right
		else if (evt.dx < 0.0f && Input::getGamepadLeftStick(evt.gid).x > 0.1f)
		{
			if (PersistentVariables::controllerSensitivities[evt.gid] < 10)
				PersistentVariables::controllerSensitivities[evt.gid]++;
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

void SettingsMenuLayer::onGuiRender()
{

}