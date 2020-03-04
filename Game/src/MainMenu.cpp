#include <Oyl3D.h>

#include "MainMenu.h"

using namespace oyl;

void MainMenuLayer::onEnter()
{
	listenForEventCategory(EventCategory::Keyboard);
	listenForEventCategory(EventCategory::Mouse);
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
		so.name = "Main Menu Background";

		auto& gui = registry->assign<component::GuiRenderable>(e);
		gui.texture = Texture2D::cache("res/assets/textures/menus/MainMenuBackground.png");
	}

	{
		auto e = registry->create();

		auto& t = registry->assign<component::Transform>(e);
		t.setPosition(glm::vec3(0.0f, 1.0f, -1.0f));
		t.setScale(glm::vec3(1.5f, 1.5f, 1.0f));

		auto& so = registry->assign<component::EntityInfo>(e);
		so.name = "Play Game Prompt";

		auto& gui = registry->assign<component::GuiRenderable>(e);
		gui.texture = Texture2D::cache("res/assets/textures/menus/PlayGame.png");
	}
}

void MainMenuLayer::onUpdate()
{

}

void MainMenuLayer::onGuiRender()
{
	/*ImGui::Begin("xdhaha");

	ImGui::SliderFloat("Force Speed", &forceSpeed, 0.1f, 10.0f);

	ImGui::End();*/
}