#include "GarbagePileHealthBarSystem.h"

void GarbagePileHealthBarSystem::onEnter()
{

}

void GarbagePileHealthBarSystem::onExit()
{

}

void GarbagePileHealthBarSystem::onUpdate()
{
	auto garbageHPBarView = registry->view<GarbagePileHealthBar, component::GuiRenderable, component::Transform>();
	for (auto garbageHPBarEntity : garbageHPBarView)
	{
		auto& garbageHPBar          = registry->get<GarbagePileHealthBar>(garbageHPBarEntity);
		auto& garbageHPBarGui       = registry->get<component::GuiRenderable>(garbageHPBarEntity);
		auto& garbageHPBarTransform = registry->get<component::Transform>(garbageHPBarEntity);

		//go thru each garbage pile
		auto garbagePileView = registry->view<GarbagePile, component::Transform>();
		for (auto garbagePileEntity : garbagePileView)
		{
			auto& garbagePile          = registry->get<GarbagePile>(garbagePileEntity);
			auto& garbagePileTransform = registry->get<component::Transform>(garbagePileEntity);

			//check if the garbage pile is the one that the HP bar is attached to
			if (garbageHPBar.garbagePileNum == garbagePile.relativePositionOnShip && garbageHPBar.team == garbagePile.team)
			{
				//set the HP bar position to the garbage pile's position
				garbageHPBarTransform.setPosition(garbagePileTransform.getPosition() + glm::vec3(0.0f, 1.0f, 0.0f));
				break;
			}
		}

		auto playerCameraView = registry->view<component::Camera>();
		for (auto playerCameraEntity : playerCameraView)
		{
			auto& playerCamera          = registry->get<component::Camera>(playerCameraEntity);
			auto& playerCameraTransform = registry->get<component::Transform>(playerCameraEntity);

			//check if the camera should see the HP bar
			if (playerCamera.player == garbageHPBar.playerNum)
			{
				glm::vec3 garbagePileDirection = garbageHPBarTransform.getPosition() - playerCameraTransform.getPositionGlobal();
				if (garbagePileDirection != glm::vec3(0.0f, 0.0f, 0.0f))
					garbagePileDirection = normalize(garbagePileDirection);

				float playerLookDotGarbagePileDirection = glm::dot(playerCameraTransform.getForwardGlobal(), garbagePileDirection);
				//set screen space position for the HP bar (we set the world position based on the garbage pile position above)
				garbageHPBarTransform.setPosition(playerCamera.worldToScreenSpace(garbageHPBarTransform.getPosition()));

				garbageHPBarGui.enabled = playerLookDotGarbagePileDirection > 0.0f;

				break;
			}
		}
	}
}

bool GarbagePileHealthBarSystem::onEvent(const Event& event)
{
	switch (event.type)
	{
		case (EventType)TypeGarbageCleaned:
		{


			break;
		}
	}
	return false;
}