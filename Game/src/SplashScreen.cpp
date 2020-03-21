#include "SplashScreen.h"

using namespace oyl;

void SplashScreenLayer::onEnter()
{
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
		t.setPosition(glm::vec3(0.0f, 0.0f, -10.0f));
		t.setScale(glm::vec3(10.0f, 10.0f, 10.0f));

		auto& so = registry->assign<component::EntityInfo>(e);
		so.name = "Jampacked Logo";

		auto& gui = registry->assign<component::GuiRenderable>(e);
		gui.texture = Texture2D::cache("res/assets/textures/menus/JampackedLogo.png");
	}
}

void SplashScreenLayer::onUpdate()
{
	changeSceneCountdown -= Time::deltaTime();

	if (changeSceneCountdown < 0.0f)
		Application::get().changeScene("MainMenuScene");
}

bool SplashScreenLayer::onEvent(const Event& event)
{

	return false;
}

void SplashScreenLayer::onGuiRender()
{

}