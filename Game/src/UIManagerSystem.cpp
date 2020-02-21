#include "UIManagerSystem.h"

void UIManagerSystem::onEnter()
{
	listenForEventCategory((EventCategory)CategoryPlayer);
	listenForEventCategory((EventCategory)CategoryGarbagePile);
}

void UIManagerSystem::onExit()
{
}

void UIManagerSystem::onUpdate()
{

}

bool UIManagerSystem::onEvent(const Event& event)
{
	switch (event.type)
	{
		case (EventType)TypePlayerInteractResult:
		{
			auto evt = event_cast<PlayerInteractResultEvent>(event);
			switch (evt.interactionType)
			{
				case PlayerInteractionResult::nothing:
				{
					enableValidUIType(PlayerInteractionResult::nothing, evt.playerNum);
					break;
				}
				case PlayerInteractionResult::invalid:
				{
					enableInvalidUIType(PlayerInteractionResult::invalid, evt.playerNum);
					break;
				}
				case PlayerInteractionResult::cleanGarbagePile:
				{
					enableValidUIType(PlayerInteractionResult::cleanGarbagePile, evt.playerNum);
					break;
				}
				case PlayerInteractionResult::takeCannonballFromCrate:
				{
					enableValidUIType(PlayerInteractionResult::takeCannonballFromCrate, evt.playerNum);
					break;
				}
				case PlayerInteractionResult::pickUpCleaningSolution:
				{
					enableValidUIType(PlayerInteractionResult::pickUpCleaningSolution, evt.playerNum);
					break;
				}
				case PlayerInteractionResult::pickUpMop:
				{
					enableValidUIType(PlayerInteractionResult::pickUpMop, evt.playerNum);
					break;
				}
				case PlayerInteractionResult::pickUpGloop:
				{
					enableValidUIType(PlayerInteractionResult::pickUpGloop, evt.playerNum);
					break;
				}
				case PlayerInteractionResult::pickUpThrowableBottle:
				{
					enableValidUIType(PlayerInteractionResult::pickUpThrowableBottle, evt.playerNum);
					break;
				}
				case PlayerInteractionResult::pickUpCannonball:
				{
					enableValidUIType(PlayerInteractionResult::pickUpCannonball, evt.playerNum);
					break;
				}
				case PlayerInteractionResult::useGloop:
				{
					enableValidUIType(PlayerInteractionResult::useGloop, evt.playerNum);
					break;
				}

				case PlayerInteractionResult::needCleaningSolution:
				{
					enableInvalidUIType(PlayerInteractionResult::needCleaningSolution, evt.playerNum);
					break;
				}
				case PlayerInteractionResult::needMop:
				{
					enableInvalidUIType(PlayerInteractionResult::needMop, evt.playerNum);
					break;
				}
				case PlayerInteractionResult::needToDropItems:
				{
					enableInvalidUIType(PlayerInteractionResult::needToDropItems, evt.playerNum);
					break;
				}
				case PlayerInteractionResult::cannonFiringSoon:
				{
					enableInvalidUIType(PlayerInteractionResult::cannonFiringSoon, evt.playerNum);
					break;
				}

				case PlayerInteractionResult::loadCannon:
				{
					enableValidUIType(PlayerInteractionResult::loadCannon, evt.playerNum);
					break;
				}
				case PlayerInteractionResult::pushCannon:
				{
					enableValidUIType(PlayerInteractionResult::pushCannon, evt.playerNum);
					break;
				}
			}

			break;
		}
	}

	return false;
}

void UIManagerSystem::enableValidUIType(PlayerInteractionResult a_type, PlayerNumber a_playerNum)
{
	auto hudView = registry->view<component::GuiRenderable, component::Transform, PlayerInteractionType, PlayerHUDElement>();
	for (auto& guiEntity : hudView)
	{
		auto& interactionType     = registry->get<PlayerInteractionType>(guiEntity);
		auto& renderableTransform = registry->get<component::Transform>(guiEntity);
		auto& HUDElement          = registry->get<PlayerHUDElement>(guiEntity);

		if (HUDElement.playerNum != a_playerNum)
			continue;

		if (interactionType.type == a_type)
			renderableTransform.setPosition(HUDElement.positionWhenActive);
		else
			renderableTransform.setPosition(glm::vec3(100.0f, 100.0f, 100.0f));
	}

	auto reticleView = registry->view<component::GuiRenderable, component::Transform, Reticle>();
	for (auto& reticleEntity : reticleView)
	{
		auto& reticle          = registry->get<Reticle>(reticleEntity);
		auto& reticleTransform = registry->get<component::Transform>(reticleEntity);
		auto& HUDElement       = registry->get<PlayerHUDElement>(reticleEntity);

		if (HUDElement.playerNum != a_playerNum)
			continue;

		if (reticle.type == ReticleType::normal)
			reticleTransform.setPosition(HUDElement.positionWhenActive);
		else
			reticleTransform.setPosition(glm::vec3(100.0f, 100.0f, 100.0f));
	}
}

void UIManagerSystem::enableInvalidUIType(PlayerInteractionResult a_type, PlayerNumber a_playerNum)
{
	auto hudView = registry->view<component::GuiRenderable, component::Transform, PlayerInteractionType>();
	for (auto& guiEntity : hudView)
	{
		auto& interactionType     = registry->get<PlayerInteractionType>(guiEntity);
		auto& renderableTransform = registry->get<component::Transform>(guiEntity);
		auto& HUDElement          = registry->get<PlayerHUDElement>(guiEntity);

		if (HUDElement.playerNum != a_playerNum)
			continue;

		if (interactionType.type == a_type)
			renderableTransform.setPosition(HUDElement.positionWhenActive);
		else
			renderableTransform.setPosition(glm::vec3(100.0f, 100.0f, 100.0f));
	}

	auto reticleView = registry->view<component::GuiRenderable, component::Transform, Reticle>();
	for (auto& reticleEntity : reticleView)
	{
		auto& reticle          = registry->get<Reticle>(reticleEntity);
		auto& reticleTransform = registry->get<component::Transform>(reticleEntity);
		auto& HUDElement       = registry->get<PlayerHUDElement>(reticleEntity);

		if (HUDElement.playerNum != a_playerNum)
			continue;

		if (reticle.type == ReticleType::invalid)
			reticleTransform.setPosition(HUDElement.positionWhenActive);
		else
			reticleTransform.setPosition(glm::vec3(100.0f, 100.0f, 100.0f));
	}
}