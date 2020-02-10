#include "GarbagePileGloopIndicatorSystem.h"

void GarbagePileGloopIndicatorSystem::onEnter()
{
	this->listenForEventCategory((EventCategory)CategoryGarbagePile);
}

void GarbagePileGloopIndicatorSystem::onExit()
{

}

void GarbagePileGloopIndicatorSystem::onUpdate()
{
	auto gloopIndicatorView = registry->view<GarbagePileGloopIndicator, component::GuiRenderable, component::Transform>();
	for (auto gloopIndicatorEntity : gloopIndicatorView)
	{
		auto& gloopIndicator = registry->get<GarbagePileGloopIndicator>(gloopIndicatorEntity);
		auto& gloopIndicatorGui = registry->get<component::GuiRenderable>(gloopIndicatorEntity);
		auto& gloopIndicatorTransform = registry->get<component::Transform>(gloopIndicatorEntity);

		//go thru each garbage pile
		auto garbagePileView = registry->view<GarbagePile, component::Transform>();
		for (auto garbagePileEntity : garbagePileView)
		{
			auto& garbagePile = registry->get<GarbagePile>(garbagePileEntity);
			auto& garbagePileTransform = registry->get<component::Transform>(garbagePileEntity);

			//check if the garbage pile is the one that the gloop indicator is attached to
			if (gloopIndicator.garbagePileNum == garbagePile.relativePositionOnShip && gloopIndicator.team != garbagePile.team)
			{
				//check if the garbage pile can be glooped
				if (!garbagePile.isGlooped
					&& garbagePile.garbageLevel > 0
					&& (garbagePile.garbageLevel < garbagePile.MAX_GARBAGE_LEVEL || garbagePile.garbageTicks < garbagePile.GARBAGE_TICKS_PER_LEVEL))
				{
					//set the gloop indicator's position to the garbage pile's position
					gloopIndicatorTransform.setPosition(garbagePileTransform.getPosition() + glm::vec3(0.0f, 1.8f, 0.0f));
					gloopIndicator.shouldBeHidden = false;
				}
				else
					gloopIndicator.shouldBeHidden = true;

				break; //break since we already found the garbage pile associated with this gloop indicator
			}
		}

		//find the player for this gloop indicator and check if they are carrying gloop
		auto view = registry->view<Player, component::Transform, component::RigidBody>();
		for (auto& playerEntity : view)
		{
			auto& player = registry->get<Player>(playerEntity);

			if (player.playerNum == gloopIndicator.playerNum)
			{
				if (!(registry->valid(player.primaryCarriedItem)) || !(registry->get<CarryableItem>(player.primaryCarriedItem).type == CarryableItemType::gloop))
					gloopIndicator.shouldBeHidden = true;

				break;
			}
		}

		if (gloopIndicator.shouldBeHidden)
			gloopIndicatorGui.enabled = false;
		else
		{
			auto playerCameraView = registry->view<component::Camera>();
			for (auto playerCameraEntity : playerCameraView)
			{
				auto& playerCamera = registry->get<component::Camera>(playerCameraEntity);
				auto& playerCameraTransform = registry->get<component::Transform>(playerCameraEntity);

				//check if the camera should see the gloop indicator
				if (playerCamera.player == gloopIndicator.playerNum)
				{
					glm::vec3 garbagePileDirection = gloopIndicatorTransform.getPosition() - playerCameraTransform.getPositionGlobal();
					if (garbagePileDirection != glm::vec3(0.0f, 0.0f, 0.0f))
						garbagePileDirection = normalize(garbagePileDirection);

					float playerLookDotGarbagePileDirection = glm::dot(playerCameraTransform.getForwardGlobal(), garbagePileDirection);

					//set screen space position for the gloop indicator (we set the world position based on the garbage pile position above)
					glm::vec3 newPosition = playerCamera.worldToScreenSpace(gloopIndicatorTransform.getPosition());
					gloopIndicatorTransform.setPosition(newPosition);

					//disable any gloop indicators that are behind the camera
					gloopIndicatorGui.enabled = playerLookDotGarbagePileDirection > 0.0f;

					break;
				}
			}
		}
	}
}

bool GarbagePileGloopIndicatorSystem::onEvent(const Event& event)
{
	
	return false;
}