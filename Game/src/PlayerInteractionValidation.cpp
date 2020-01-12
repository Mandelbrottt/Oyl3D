#include "PlayerInteractionValidation.h"

void PlayerInteractionValidationSystem::onEnter()
{
	this->listenForEventCategory((EventCategory)CategoryPlayer);
	this->listenForEventCategory((EventCategory)CategoryQuicktimeCleaningEvent);
}

void PlayerInteractionValidationSystem::onExit()
{
    
}

void PlayerInteractionValidationSystem::onUpdate()
{
	auto playerView = registry->view<Player, component::Transform>();
	for (entt::entity playerEntity : playerView)
	{
		performRaycastAndValidateForPlayer(playerEntity);

		//check if player did anything to cancel out of their cleaning quicktime event state
		auto& player = registry->get<Player>(playerEntity);
		if (player.state == PlayerState::inCleaningQuicktimeEvent)
		{
			if (   !registry->valid(player.primaryCarriedItem)
				|| registry->get<CarryableItem>(player.primaryCarriedItem).type != CarryableItemType::mop
				|| !registry->valid(player.interactableEntity)
				|| !registry->has<GarbagePile>(player.interactableEntity))
			{
				PlayerStateChangeEvent playerStateChange;
				playerStateChange.playerEntity = playerEntity;
				playerStateChange.newState     = PlayerState::idle;
				postEvent(playerStateChange);

				//TODO: deactivate CleaningQuicktimeEvent UI for player
			}
		}
	}
}

bool PlayerInteractionValidationSystem::onEvent(const Event& event)
{
	switch (event.type)
	{
		case (EventType)TypePlayerInteractionRequest:
	    {
			auto evt = event_cast<PlayerInteractionRequestEvent>(event);
			performInteractionForPlayer(evt.playerEntity);
	        
			break;
	    }
		case (EventType)TypePlayerDropItem:
		{
			auto evt = event_cast<PlayerDropItemEvent>(event);
			dropPlayerCarriedItems(evt.playerEntity);
			
			break;
		}
		case (EventType)TypeQuicktimeCleaningEventResult:
		{
			auto evt = event_cast<QuicktimeCleaningEventResultEvent>(event);
			if (evt.wasSuccessful)
			{
				auto& player = registry->get<Player>(evt.playerEntity);
				if (!registry->valid(player.interactableEntity) || !registry->has<GarbagePile>(player.interactableEntity))
				{
					std::cout << "WARNING: PLAYER'S INTERACTABLE ENTITY AFTER COMPLETING CLEANING QUICKTIME EVENT WAS AN INVALID ENTITY!\n";
					break;
				}
				auto& garbagePile = registry->get<GarbagePile>(player.interactableEntity);

				std::cout << "CLEANING!\n";

				if (garbagePile.garbageTicks < garbagePile.GARBAGE_TICKS_PER_LEVEL)
				{
					//if only a mop is needed for cleaning, drop the cleaning solution if the player is carrying one (mop animation uses both hands)
					if (registry->valid(player.secondaryCarriedItem) && registry->get<CarryableItem>(player.secondaryCarriedItem).type == CarryableItemType::cleaningSolution)
						dropPlayerCarriedItems(evt.playerEntity, true, CarryableItemType::cleaningSolution);
				}
				else //garbage ticks >= max ticks, so cleaning solution is used
				{
					registry->destroy(player.secondaryCarriedItem);
					player.secondaryCarriedItem = entt::null;
				}

				//TODO: deactivate CleaningQuicktimeEvent UI for player

				RequestToCleanGarbageEvent requestToCleanGarbage;
				requestToCleanGarbage.garbagePileEntity = player.interactableEntity;
				postEvent(requestToCleanGarbage);

				PlayerStateChangeEvent playerStateChange;
				playerStateChange.playerEntity = evt.playerEntity;
				playerStateChange.newState = PlayerState::cleaning;
				postEvent(playerStateChange);
			}
			else //!wasSuccessful
			{
				dropPlayerCarriedItems(evt.playerEntity);

				PlayerStateChangeEvent playerStateChange;
				playerStateChange.playerEntity = evt.playerEntity;
				playerStateChange.newState = PlayerState::idle;
				postEvent(playerStateChange);
			}

			break;
		}
	}

	return false;
}

void PlayerInteractionValidationSystem::performRaycastAndValidateForPlayer(entt::entity a_playerEntity)
{
	auto& player          = registry->get<Player>(a_playerEntity);
	auto& playerTransform = registry->get<component::Transform>(a_playerEntity);

	if (player.state == PlayerState::pushing || player.state == PlayerState::cleaning)
	{
		//player cannot switch out of pushing or cleaning states
		player.interactableEntity = entt::null;

		PlayerInteractResultEvent playerInteractResult;
		playerInteractResult.interactionType = PlayerInteractionResult::nothing;
		postEvent(playerInteractResult);

		return;
	}

	//get the camera associated with the player
	entt::entity playersCameraEntity = entt::null;

	auto cameraView = registry->view<component::PlayerCamera, component::Transform>();
	for (entt::entity cameraEntity : cameraView)
	{
		if (player.playerNum == registry->get<component::PlayerCamera>(cameraEntity).player)
			playersCameraEntity = cameraEntity;
	}
	if (!registry->valid(playersCameraEntity))
	{
		std::cout << "WARNING: PLAYER " << player.playerNum << " DOES NOT HAVE AN ASSOCIATED CAMERA\n";
		return;
	}

	auto& playerCameraTransform = registry->get<component::Transform>(playersCameraEntity);

	auto ray = RayTest::Closest(playerCameraTransform.getPositionGlobal(), playerCameraTransform.getForwardGlobal(), 1.4f);
	if (ray->hasHit)
	{
		entt::entity raycastHitEntity = ray->hitObject.entity;
		if (registry->has<component::EntityInfo>(raycastHitEntity))
		{
			auto& ei = registry->get<component::EntityInfo>(raycastHitEntity);
			OYL_LOG("RAYCAST HIT ENTITY: {}", ei.name);
		}

		if (registry->has<CarryableItem>(raycastHitEntity))
			validateCarryableItemInteraction(a_playerEntity, raycastHitEntity);

		else if (registry->has<GarbagePile>(raycastHitEntity))
			validateGarbagePileInteraction(a_playerEntity, raycastHitEntity);

		else if (registry->has<CannonballCrate>(raycastHitEntity))
			validateCannonballCrateInteraction(a_playerEntity, raycastHitEntity);

		else if (registry->has<Cannon>(raycastHitEntity))
			validateCannonInteraction(a_playerEntity, raycastHitEntity);

		else
		{
			//no valid interactions
			player.interactableEntity = entt::null;

			PlayerInteractResultEvent playerInteractResult;
			playerInteractResult.interactionType = PlayerInteractionResult::nothing;
			postEvent(playerInteractResult);
		}

	}
	else //no ray hits
	{
		//no valid interactions
		player.interactableEntity = entt::null;

		PlayerInteractResultEvent playerInteractResult;
		playerInteractResult.interactionType = PlayerInteractionResult::nothing;
		postEvent(playerInteractResult);
	}
}

void PlayerInteractionValidationSystem::validateCarryableItemInteraction(entt::entity a_playerEntity, entt::entity a_carryableItemEntity)
{
	auto& player          = registry->get<Player>(a_playerEntity);
	auto& playerTransform = registry->get<component::Transform>(a_playerEntity);

	auto& carryableItem          = registry->get<CarryableItem>(a_carryableItemEntity);
	auto& carryableItemTransform = registry->get<component::Transform>(a_carryableItemEntity);

	if (!carryableItem.isBeingCarried)
	{
		switch (carryableItem.type)
		{
			case CarryableItemType::cannonball:
			{
				if (!registry->valid(player.primaryCarriedItem) && !registry->valid(player.secondaryCarriedItem))
				{
					player.interactableEntity = a_carryableItemEntity;

					PlayerInteractResultEvent playerInteractResult;
					playerInteractResult.interactionType = PlayerInteractionResult::pickUpCannonball;
					postEvent(playerInteractResult);

					return;
				}
				break;
			}
			case CarryableItemType::mop:
			{
				if (!registry->valid(player.primaryCarriedItem))
				{
					player.interactableEntity = a_carryableItemEntity;

					PlayerInteractResultEvent playerInteractResult;
					playerInteractResult.interactionType = PlayerInteractionResult::pickUpMop;
					postEvent(playerInteractResult);

					return;
				}

				break;
			}
			case CarryableItemType::cleaningSolution:
			{
				if (!registry->valid(player.secondaryCarriedItem)
					&& (!registry->valid(player.primaryCarriedItem) || registry->get<CarryableItem>(player.primaryCarriedItem).type == CarryableItemType::mop))
				{
					player.interactableEntity = a_carryableItemEntity;

					PlayerInteractResultEvent playerInteractResult;
					playerInteractResult.interactionType = PlayerInteractionResult::pickUpCleaningSolution;
					postEvent(playerInteractResult);

					return;
				}

				break;
			}
			case CarryableItemType::gloop:
			{
				if (!registry->valid(player.primaryCarriedItem) && !registry->valid(player.secondaryCarriedItem))
				{
					player.interactableEntity = a_carryableItemEntity;

					PlayerInteractResultEvent playerInteractResult;
					playerInteractResult.interactionType = PlayerInteractionResult::pickUpGloop;
					postEvent(playerInteractResult);

					return;
				}
				break;
			}
		}
	}

	//no early return means there is no valid result
	PlayerInteractResultEvent playerInteractResult;
	playerInteractResult.interactionType = PlayerInteractionResult::invalid;
	postEvent(playerInteractResult);
}

void PlayerInteractionValidationSystem::validateGarbagePileInteraction(entt::entity a_playerEntity, entt::entity a_garbagePileEntity)
{
	auto& player          = registry->get<Player>(a_playerEntity);
	auto& playerTransform = registry->get<component::Transform>(a_playerEntity);

	auto& garbagePile          = registry->get<GarbagePile>(a_garbagePileEntity);
	auto& garbagePileTransform = registry->get<component::Transform>(a_garbagePileEntity);

	if (garbagePile.garbageLevel > 0)
	{
		if (garbagePile.team == player.team)
		{
			//garbage ticks less than max means the player needs a mop to clean
			if (garbagePile.garbageTicks < garbagePile.GARBAGE_TICKS_PER_LEVEL)
			{
				if (   registry->valid(player.primaryCarriedItem)
					&& registry->get<CarryableItem>(player.primaryCarriedItem).type == CarryableItemType::mop
					&& registry->get<CarryableItem>(player.primaryCarriedItem).team == player.team)
				{
					player.interactableEntity = a_garbagePileEntity;

					PlayerInteractResultEvent playerInteractResult;
					playerInteractResult.interactionType = PlayerInteractionResult::cleanGarbagePile;
					postEvent(playerInteractResult);

					return;
				}
			}
			//garbage ticks at max means the player needs cleaning solution to be able to clean
			else //garbage ticks at max
			{
				if (   registry->valid(player.secondaryCarriedItem)
					&& registry->get<CarryableItem>(player.secondaryCarriedItem).type == CarryableItemType::cleaningSolution
					&& registry->get<CarryableItem>(player.secondaryCarriedItem).team == player.team)
				{
					player.interactableEntity = a_garbagePileEntity;

					PlayerInteractResultEvent playerInteractResult;
					playerInteractResult.interactionType = PlayerInteractionResult::cleanGarbagePile;
					postEvent(playerInteractResult);

					return;
				}
			}
		}
		else //garbagePile.team != player.team
		{
			if (   !garbagePile.isGlooped
				&& registry->valid(player.primaryCarriedItem)
				&& registry->get<CarryableItem>(player.primaryCarriedItem).type == CarryableItemType::gloop
				&& registry->get<CarryableItem>(player.primaryCarriedItem).team == player.team)
			{
				player.interactableEntity = a_garbagePileEntity;

				PlayerInteractResultEvent playerInteractResult;
				playerInteractResult.interactionType = PlayerInteractionResult::useGloop;
				postEvent(playerInteractResult);

				return;
			}
		}
	}

	//no early return means there is no valid result
	PlayerInteractResultEvent playerInteractResult;
	playerInteractResult.interactionType = PlayerInteractionResult::invalid;
	postEvent(playerInteractResult);
}

void PlayerInteractionValidationSystem::validateCannonballCrateInteraction(entt::entity a_playerEntity, entt::entity a_cannonballCrateEntity)
{
	auto& player          = registry->get<Player>(a_playerEntity);
	auto& playerTransform = registry->get<component::Transform>(a_playerEntity);

	auto& cannonballCrate          = registry->get<CannonballCrate>(a_cannonballCrateEntity);
	auto& cannonballCrateTransform = registry->get<component::Transform>(a_cannonballCrateEntity);

	if (   !registry->valid(player.primaryCarriedItem)
		&& !registry->valid(player.secondaryCarriedItem)
		&& cannonballCrate.team == player.team)
	{
		player.interactableEntity = a_cannonballCrateEntity;

		PlayerInteractResultEvent playerInteractResult;
		playerInteractResult.interactionType = PlayerInteractionResult::takeCannonballFromCrate;
		postEvent(playerInteractResult);

		return;
	}

	//no early return means there is no valid result
	PlayerInteractResultEvent playerInteractResult;
	playerInteractResult.interactionType = PlayerInteractionResult::invalid;
	postEvent(playerInteractResult);
}

void PlayerInteractionValidationSystem::validateCannonInteraction(entt::entity a_playerEntity, entt::entity a_cannonEntity)
{
	auto& player          = registry->get<Player>(a_playerEntity);
	auto& playerTransform = registry->get<component::Transform>(a_playerEntity);

	auto& cannon = registry->get<Cannon>(a_cannonEntity);
	auto& cannonTransform = registry->get<component::Transform>(a_cannonEntity);

	if (player.team == cannon.team)
	{
		//cannon can be loaded with a cannonball
		if (   registry->valid(player.primaryCarriedItem)
			&& registry->get<CarryableItem>(player.primaryCarriedItem).type == CarryableItemType::cannonball
			&& registry->get<CarryableItem>(player.primaryCarriedItem).team == player.team
			&& cannon.isLoaded == false)
		{
			player.interactableEntity = a_cannonEntity;

			PlayerInteractResultEvent playerInteractResult;
			playerInteractResult.interactionType = PlayerInteractionResult::loadCannon;
			postEvent(playerInteractResult);

			return;
		}
		else if (cannon.state == CannonState::firingSoon)
		{
			player.interactableEntity = entt::null;

			PlayerInteractResultEvent playerInteractResult;
			playerInteractResult.interactionType = PlayerInteractionResult::cannonFiringSoon;
			postEvent(playerInteractResult);

			return;
		}
		else if (   cannon.state == CannonState::doingNothing
			     && !registry->valid(player.primaryCarriedItem)
			     && !registry->valid(player.secondaryCarriedItem))
		{
			float playerForwardDotCannonRight = glm::dot(playerTransform.getForward(), cannonTransform.getRight());

			bool isCannonOnLeftSideOfTrack = (cannon.cannonTrackPosition == -1)
				? true : false;
			bool isCannonOnRightSideOfTrack = (cannon.cannonTrackPosition == 1)
				? true : false;

			//check if player is on the right side of the cannon (will be pushing towards the left)
			if (playerForwardDotCannonRight < -0.65f && !isCannonOnLeftSideOfTrack)
			{
				player.interactableEntity = a_cannonEntity;

				PlayerInteractResultEvent playerInteractResult;
				playerInteractResult.interactionType = PlayerInteractionResult::pushCannon;
				postEvent(playerInteractResult);

				return;
			}
			//check if player is on the left side of the cannon (will be pushing towards the right)
			else if (playerForwardDotCannonRight > 0.65f && !isCannonOnRightSideOfTrack)
			{
				player.interactableEntity = a_cannonEntity;

				PlayerInteractResultEvent playerInteractResult;
				playerInteractResult.interactionType = PlayerInteractionResult::pushCannon;
				postEvent(playerInteractResult);

				return;
			}
		}
	}

	//no early return means there is no valid result
	PlayerInteractResultEvent playerInteractResult;
	playerInteractResult.interactionType = PlayerInteractionResult::invalid;
	postEvent(playerInteractResult);
}

void PlayerInteractionValidationSystem::performInteractionForPlayer(entt::entity a_playerEntity)
{
	auto& player = registry->get<Player>(a_playerEntity);
	auto& playerTransform = registry->get<component::Transform>(a_playerEntity);

	//check if there's a valid entity the player can interact with first
	if (player.interactableEntity == entt::null)
		return;

	if (registry->has<CarryableItem>(player.interactableEntity))
	{
		auto& carryableItem = registry->get<CarryableItem>(player.interactableEntity);
		auto& carryableItemTransform = registry->get<component::Transform>(player.interactableEntity);

		glm::vec3 itemNewPosition = glm::vec3(0.0f);
		glm::vec3 itemNewRotation = glm::vec3(0.0f);

		switch (carryableItem.type)
		{
		case CarryableItemType::cannonball:
		{
			player.primaryCarriedItem = player.interactableEntity;

			std::cout << "PICKED UP CANNONBALL!\n";

			itemNewPosition = glm::vec3(0.0f, 0.35f, -0.75f);
			itemNewRotation = glm::vec3(0.0f, 0.0f, 0.0f);

			break;
		}
		case CarryableItemType::mop:
		{
			player.primaryCarriedItem = player.interactableEntity;

			std::cout << "PICKED UP MOP!\n";

			itemNewPosition = glm::vec3(0.45f, -0.15f, -1.4f);
			itemNewRotation = glm::vec3(-107.0f, -69.5f, 106.0f);

			break;
		}
		case CarryableItemType::cleaningSolution:
		{
			player.secondaryCarriedItem = player.interactableEntity;

			std::cout << "PICKED UP CLEANING SOLUTION!\n";

			itemNewPosition = glm::vec3(-0.25f, 0.4f, -0.7f);
			itemNewRotation = glm::vec3(0.0f, 0.0f, 0.0f);

			break;
		}
		case CarryableItemType::gloop:
		{
			player.primaryCarriedItem = player.interactableEntity;

			std::cout << "PICKED UP GLOOP!\n";

			itemNewPosition = glm::vec3(0.0f, 0.25f, -0.75f);
			itemNewRotation = glm::vec3(0.0f, 0.0f, 0.0f);

			break;
		}
		default:
		{
			std::cout << "INVALID ITEM TYPE: TRIED TO PICK UP AN ITEM BUT THE ITEM TYPE WAS NOT A VALID OPTION!\n";
			break;
		}
		}

		carryableItem.isBeingCarried = true;
		auto& carryableItemParent = registry->get_or_assign<component::Parent>(player.interactableEntity);
		carryableItemParent.parent = a_playerEntity;

		carryableItemTransform.setRotationEuler(itemNewRotation);
		carryableItemTransform.setPosition(itemNewPosition);

		return;
	}

	else if (registry->has<GarbagePile>(player.interactableEntity))
	{
		auto& garbagePile = registry->get<GarbagePile>(player.interactableEntity);
		auto& garbagePileTransform = registry->get<component::Transform>(player.interactableEntity);

		if (garbagePile.team != player.team)
		{
			std::cout << "USED GLOOP\n";

			garbagePile.isGlooped = true;

			UseGloopEvent useGloop;
			useGloop.gloopEntity = player.primaryCarriedItem;
			postEvent(useGloop);
		}
		//if the player and garbage pile are the same team, cleaning is handled in the OnEvent function in this file under the cleaning quicktime event
	}

	else if (registry->has<CannonballCrate>(player.interactableEntity))
	{
		auto carryableItemView = registry->view<CarryableItem, component::Transform>();
		for (auto& carryableItemEntity : carryableItemView)
		{
			auto& carryableItem = registry->get<CarryableItem>(carryableItemEntity);
			auto& carryableItemTransform = registry->get<component::Transform>(carryableItemEntity);

			if (carryableItem.type == CarryableItemType::cannonball && !carryableItem.isActive && !carryableItem.isBeingCarried)
			{
				player.primaryCarriedItem = carryableItemEntity;

				carryableItem.isBeingCarried = true;
				carryableItem.isActive = true;

				std::cout << "OBTAINED CANNONBALL!\n";

				auto& cannonballParent = registry->get_or_assign<component::Parent>(carryableItemEntity);
				cannonballParent.parent = a_playerEntity;

				carryableItemTransform.setRotationEuler(glm::vec3(0.0f));
				carryableItemTransform.setPosition(glm::vec3(0.0f, 0.35f, -0.75f));

				return;
			}
		}
	}

	else if (registry->has<Cannon>(player.interactableEntity))
	{
		auto& cannon = registry->get<Cannon>(player.interactableEntity);
		auto& cannonTransform = registry->get<component::Transform>(player.interactableEntity);

		//cannon can be loaded with a cannonball
		if (registry->valid(player.primaryCarriedItem)
			&& registry->get<CarryableItem>(player.primaryCarriedItem).type == CarryableItemType::cannonball
			&& cannon.isLoaded == false)
		{
			cannon.isLoaded = true;

			auto& carriedItem = registry->get<CarryableItem>(player.primaryCarriedItem);
			auto& carriedItemParent = registry->get<component::Parent>(player.primaryCarriedItem);
			auto& carriedItemTransform = registry->get<component::Transform>(player.primaryCarriedItem);

			if (carriedItemParent.parent == a_playerEntity)
			{
				std::cout << "LOADED CANNON!\n";

				carriedItemParent.parent = entt::null;
				player.primaryCarriedItem = entt::null;

				carriedItem.isBeingCarried = false;
				carriedItem.isActive = false;
				carriedItemTransform.setPosition(glm::vec3(1000.0f, 1000.0f, 1000.0f));

				return;
			}
		}
		else
		{
			float playerForwardDotCannonRight = glm::dot(playerTransform.getForward(), cannonTransform.getRight());

			bool isCannonOnLeftSideOfTrack = (cannon.cannonTrackPosition == -1)
				? true : false;
			bool isCannonOnRightSideOfTrack = (cannon.cannonTrackPosition == 1)
				? true : false;

			std::cout << "dot: " << playerForwardDotCannonRight << "\n";

			//check if player is on the right side of the cannon (will be pushing towards the left)
			if (playerForwardDotCannonRight < -0.65f)
			{
				//set direction-specific variables in the if statement and non-direction specific variables outside
				player.adjustingPositionStateData.destinationPos = cannonTransform.getRight() * glm::vec3(1.2f, 0.0f, 0.0f) + cannonTransform.getPosition();
				player.pushingStateData.destinationPos = -cannonTransform.getRight() * glm::vec3(cannon.pushDistance, 0.0f, 0.0f) + player.adjustingPositionStateData.destinationPos;

				cannon.pushStateData.destinationPos = -cannonTransform.getRight() * glm::vec3(cannon.pushDistance, 0.0f, 0.0f) + cannonTransform.getPosition();
				cannon.cannonTrackPosition--;

				//hacky way to limit the camera but it works for now
				player.yRotationClamp = 50.0f;
				if (playerTransform.getRotationEulerY() < player.yRotationClamp)
					playerTransform.setRotationEulerY(player.yRotationClamp);
			}
			//player is on the left side of the cannon (will be pushing towards the right)
			else
			{
				//set direction-specific variables in the if statement and non-direction specific variables outside
				player.adjustingPositionStateData.destinationPos = -cannonTransform.getRight() * glm::vec3(1.2f, 0.0f, 0.0f) + cannonTransform.getPosition();
				player.pushingStateData.destinationPos = cannonTransform.getRight() * glm::vec3(cannon.pushDistance, 0.0f, 0.0f) + player.adjustingPositionStateData.destinationPos;

				cannon.pushStateData.destinationPos = cannonTransform.getRight() * glm::vec3(cannon.pushDistance, 0.0f, 0.0f) + cannonTransform.getPosition();
				cannon.cannonTrackPosition++;

				//hacky way to limit the camera but it works for now
				player.yRotationClamp = -50.0f;
				if (playerTransform.getRotationEulerY() > player.yRotationClamp)
					playerTransform.setRotationEulerY(player.yRotationClamp);
			}

			std::cout << "PUSHING!\n";

			player.adjustingPositionStateData.startPos = playerTransform.getPosition();
			player.adjustingPositionStateData.destinationPos.y = playerTransform.getPositionY(); //don't change the player's y position

			player.pushingStateData.startPos = player.adjustingPositionStateData.destinationPos;
			player.pushingStateData.destinationPos.y = playerTransform.getPositionY(); //don't change the player's y position

			cannon.pushStateData.startPos = cannonTransform.getPosition();

			//once all the new positions are set for the states, send out state change events
			PlayerStateChangeEvent playerStateChange;
			playerStateChange.playerEntity = a_playerEntity;
			playerStateChange.newState = PlayerState::pushing;
			postEvent(playerStateChange);

			CannonStateChangeEvent cannonStateChange;
			cannonStateChange.cannonEntity = player.interactableEntity;
			cannonStateChange.newState = CannonState::beingPushed;
			postEvent(cannonStateChange);

			return;
		}
	}
}

	//TODO: make it so when a player is in a quicktime cleaning event, they are automatically kicked out when no longer hitting the garbage pile with a raycast
void PlayerInteractionValidationSystem::dropPlayerCarriedItems(entt::entity a_playerEntity, bool dropSpecificItemType, CarryableItemType itemTypeToDrop)
{
	auto& player          = registry->get<Player>(a_playerEntity);
	auto& playerTransform = registry->get<component::Transform>(a_playerEntity);

	auto carriedItemsView = registry->view<CarryableItem, component::Parent, component::Transform>();
	for (entt::entity carriedItemEntity : carriedItemsView)
	{
		auto& carriedItem          = registry->get<CarryableItem>(carriedItemEntity);
		auto& carriedItemParent    = registry->get<component::Parent>(carriedItemEntity);
		auto& carriedItemTransform = registry->get<component::Transform>(carriedItemEntity);

		if (dropSpecificItemType && carriedItem.type != itemTypeToDrop)
			continue;
		//dont let player drop items while cleaning (will prevent weird edge cases that aren't worth dealing with)
		if (player.state == PlayerState::cleaning)
			continue;

		if (carriedItemParent.parent == a_playerEntity)
		{
			carriedItemParent.parent = entt::null;
			carriedItem.isBeingCarried = false;

			glm::vec3 newPosition = playerTransform.getPosition();
			glm::vec3 newRotation = playerTransform.getRotationEuler();

			switch (carriedItem.type)
			{
				case CarryableItemType::cannonball:
				{
					std::cout << "DROPPED CANNONBALL!\n";
					player.primaryCarriedItem = entt::null;

					newPosition += playerTransform.getForward() * 0.8f;
					newPosition.y = 0.25f;

					break;
				}
				case CarryableItemType::mop:
				{
					std::cout << "DROPPED MOP!\n";
					player.primaryCarriedItem = entt::null;

					newPosition += playerTransform.getForward() * 0.7f;
					newPosition += playerTransform.getRight()   * 0.7f;
					newPosition.y = 0.1f;

					newRotation += glm::vec3(0.0f, -90.0f, 0.0f);

					break;
				}
				case CarryableItemType::cleaningSolution:
				{
					std::cout << "DROPPED CLEANING SOLUTION!\n";
					player.secondaryCarriedItem = entt::null;

					newPosition += playerTransform.getForward() * 1.0f;
					newPosition += playerTransform.getRight()   * -0.3f;
					newPosition.y = 0.22f;

					break;
				}
				case CarryableItemType::gloop:
				{
					std::cout << "DROPPED GLOOP!\n";
					player.primaryCarriedItem = entt::null;

					newPosition += playerTransform.getForward() * 0.8f;
					newPosition.y = 0.27f;

					break;
				}
			}

			carriedItemTransform.setPosition(newPosition);
			carriedItemTransform.setRotationEuler(newRotation);
		}
	}
}