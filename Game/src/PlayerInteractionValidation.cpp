#include "PlayerInteractionValidation.h"

void PlayerInteractionValidationSystem::onEnter()
{
	this->listenForEventCategory((OylEnum)CategoryPlayer);
	this->listenForEventCategory((OylEnum)CategoryCannon);
}

void PlayerInteractionValidationSystem::onExit()
{
    
}

void PlayerInteractionValidationSystem::onUpdate(Timestep dt)
{
	auto playerView = registry->view<Player, component::Transform>();
	for (entt::entity playerEntity : playerView)
		checkForAnyValidPlayerInteractions(playerEntity);
}

bool PlayerInteractionValidationSystem::onEvent(Ref<Event> event)
{
	switch (event->type)
	{
	    case TypePlayerInteractionRequest:
	    {
			auto evt = (PlayerInteractionRequestEvent)* event;
	        
			auto playerView = registry->view<Player, component::Transform>();
			for (entt::entity playerEntity : playerView)
			{
			    if (evt.playerEntity == playerEntity)
					validateInteraction(playerEntity);
			}
	        
			break;
	    }

		case TypePlayerDropItem:
		{
			auto evt = (PlayerDropItemEvent)* event;
			dropPlayerCarriedItems(evt.playerEntity);
			
			break;
		}
	}

	return false;
}

void PlayerInteractionValidationSystem::checkForAnyValidPlayerInteractions(entt::entity a_playerEntity)
{
	auto& player          = registry->get<Player>(a_playerEntity);
	auto& playerTransform = registry->get<component::Transform>(a_playerEntity);

	if (player.state == PlayerState::pushing || player.state == PlayerState::cleaning)
	{
		//player cannot switch out of pushing or cleaning states
		player.interactableEntity = entt::null;

		PlayerInteractResultEvent playerInteractResult;
		playerInteractResult.interactionType = PlayerInteractionResult::nothing;
		postEvent(Event::create(playerInteractResult));

		return;
	}

	auto carryableItemView = registry->view<CarryableItem, component::Transform>();
	for (auto& carryableItemEntity : carryableItemView)
	{
		auto& carryableItem          = registry->get<CarryableItem>(carryableItemEntity);
		auto& carryableItemTransform = registry->get<component::Transform>(carryableItemEntity);

		glm::vec3 itemNewPosition = glm::vec3(0.0f);
		glm::vec3 itemNewRotation = glm::vec3(0.0f);
			

		switch (carryableItem.type)
		{
			case CarryableItemType::cannonball:
			{
				if (   !carryableItem.isBeingCarried
					&& !registry->valid(player.primaryCarriedItem)
					&& !registry->valid(player.secondaryCarriedItem)
					//compare x values
					&& playerTransform.getPositionX() < carryableItemTransform.getPositionX() + 1.f
					&& playerTransform.getPositionX() > carryableItemTransform.getPositionX() - 1.f
					//compare z values
					&& playerTransform.getPositionZ() < carryableItemTransform.getPositionZ() + 1.f
					&& playerTransform.getPositionZ() > carryableItemTransform.getPositionZ() - 1.f)
				{
					player.interactableEntity = carryableItemEntity;

					PlayerInteractResultEvent playerInteractResult;
					playerInteractResult.interactionType = PlayerInteractionResult::pickUpCannonball;
					postEvent(Event::create(playerInteractResult));

					return;
				}
				break;
			}
			case CarryableItemType::mop:
			{
				if (   !carryableItem.isBeingCarried
					&& !registry->valid(player.primaryCarriedItem)
					//compare x values
					&& playerTransform.getPositionX() < carryableItemTransform.getPositionX() + 1.6f
					&& playerTransform.getPositionX() > carryableItemTransform.getPositionX() - 1.6f
					//compare z values
					&& playerTransform.getPositionZ() < carryableItemTransform.getPositionZ() + 1.6f
					&& playerTransform.getPositionZ() > carryableItemTransform.getPositionZ() - 1.6f)
				{
					player.interactableEntity = carryableItemEntity;

					PlayerInteractResultEvent playerInteractResult;
					playerInteractResult.interactionType = PlayerInteractionResult::pickUpMop;
					postEvent(Event::create(playerInteractResult));

					return;
				}

				break;
			}
			case CarryableItemType::cleaningSolution:
			{
				if (   !carryableItem.isBeingCarried
					&& !registry->valid(player.secondaryCarriedItem)
					&& (!registry->valid(player.primaryCarriedItem)
						|| registry->get<CarryableItem>(player.primaryCarriedItem).type == CarryableItemType::mop)
					//compare x values
					&& playerTransform.getPositionX() < carryableItemTransform.getPositionX() + 1.1f
					&& playerTransform.getPositionX() > carryableItemTransform.getPositionX() - 1.1f
					//compare z values
					&& playerTransform.getPositionZ() < carryableItemTransform.getPositionZ() + 1.1f
					&& playerTransform.getPositionZ() > carryableItemTransform.getPositionZ() - 1.1f)
				{
					player.interactableEntity = carryableItemEntity;

					PlayerInteractResultEvent playerInteractResult;
					playerInteractResult.interactionType = PlayerInteractionResult::pickUpCleaningSolution;
					postEvent(Event::create(playerInteractResult));

					return;
				}

				break;
			}
			case CarryableItemType::gloop:
			{
				if (   !carryableItem.isBeingCarried
					&& !registry->valid(player.primaryCarriedItem)
					&& !registry->valid(player.secondaryCarriedItem)
					//compare x values
					&& playerTransform.getPositionX() < carryableItemTransform.getPositionX() + 1.2f
					&& playerTransform.getPositionX() > carryableItemTransform.getPositionX() - 1.2f
					//compare z values
					&& playerTransform.getPositionZ() < carryableItemTransform.getPositionZ() + 1.2f
					&& playerTransform.getPositionZ() > carryableItemTransform.getPositionZ() - 1.2f)
				{
					player.interactableEntity = carryableItemEntity;

					PlayerInteractResultEvent playerInteractResult;
					playerInteractResult.interactionType = PlayerInteractionResult::pickUpGloop;
					postEvent(Event::create(playerInteractResult));

					return;
				}
				break;
			}
		}
	}

	auto garbagePileView = registry->view<GarbagePile, component::Transform>();
	for (auto& garbagePileEntity : garbagePileView)
	{
		auto& garbagePile          = registry->get<GarbagePile>(garbagePileEntity);
		auto& garbagePileTransform = registry->get<component::Transform>(garbagePileEntity);

		if (garbagePile.garbageLevel > 0
			//compare x values
			&& playerTransform.getPositionX() < garbagePileTransform.getPositionX() + 2.6f
			&& playerTransform.getPositionX() > garbagePileTransform.getPositionX() - 2.6f
			//compare z values
			&& playerTransform.getPositionZ() < garbagePileTransform.getPositionZ() + 2.6f
			&& playerTransform.getPositionZ() > garbagePileTransform.getPositionZ() - 2.6f)
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
						player.interactableEntity = garbagePileEntity;

						PlayerInteractResultEvent playerInteractResult;
						playerInteractResult.interactionType = PlayerInteractionResult::cleanGarbagePile;
						postEvent(Event::create(playerInteractResult));

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
						player.interactableEntity = garbagePileEntity;

						PlayerInteractResultEvent playerInteractResult;
						playerInteractResult.interactionType = PlayerInteractionResult::cleanGarbagePile;
						postEvent(Event::create(playerInteractResult));

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
					player.interactableEntity = garbagePileEntity;

					PlayerInteractResultEvent playerInteractResult;
					playerInteractResult.interactionType = PlayerInteractionResult::useGloop;
					postEvent(Event::create(playerInteractResult));

					return;
				}
			}
		}
	}

	auto cannonballCrateView = registry->view<CannonballCrate, component::Transform>();
	for (auto& cannonballCrateEntity : cannonballCrateView)
	{
		auto& cannonballCrate          = registry->get<CannonballCrate>(cannonballCrateEntity);
		auto& cannonballCrateTransform = registry->get<component::Transform>(cannonballCrateEntity);

		if (   !registry->valid(player.primaryCarriedItem)
			&& !registry->valid(player.secondaryCarriedItem)
			&& cannonballCrate.team == player.team
			//compare x values
			&& playerTransform.getPositionX() < cannonballCrateTransform.getPositionX() + 2.2f
			&& playerTransform.getPositionX() > cannonballCrateTransform.getPositionX() - 2.2f
			//compare z values
			&& playerTransform.getPositionZ() < cannonballCrateTransform.getPositionZ() + 1.7f
			&& playerTransform.getPositionZ() > cannonballCrateTransform.getPositionZ() - 1.7f)
		{
			player.interactableEntity = cannonballCrateEntity;

			PlayerInteractResultEvent playerInteractResult;
			playerInteractResult.interactionType = PlayerInteractionResult::takeCannonballFromCrate;
			postEvent(Event::create(playerInteractResult));

			return;
		}
	}

	//check all cannons
	auto cannonView = registry->view<Cannon, component::Transform>();
	for (auto& cannonEntity : cannonView)
	{
		auto& cannon = registry->get<Cannon>(cannonEntity);
		auto& cannonTransform = registry->get<component::Transform>(cannonEntity);

		if (player.team == cannon.team)
		{
			//cannon can be loaded with a cannonball
			if (   registry->valid(player.primaryCarriedItem)
				&& registry->get<CarryableItem>(player.primaryCarriedItem).type == CarryableItemType::cannonball
				&& registry->get<CarryableItem>(player.primaryCarriedItem).team == player.team
				&& cannon.isLoaded == false
				//compare x values
				&& playerTransform.getPositionX() < cannonTransform.getPositionX() + 1.7f
				&& playerTransform.getPositionX() > cannonTransform.getPositionX() - 1.7f
				//compare z values
				&& playerTransform.getPositionZ() < cannonTransform.getPositionZ() + 1.7f
				&& playerTransform.getPositionZ() > cannonTransform.getPositionZ() - 1.7f) //check if cannon can be loaded with a cannonball
			{
				//get rid of the cannonball the player has loaded
				auto carriedItemsView = registry->view<CarryableItem, component::Parent, component::Transform>();
				for (entt::entity carriedItemEntity : carriedItemsView)
				{
					auto& carriedItem          = registry->get<CarryableItem>(carriedItemEntity);
					auto& carriedItemParent    = registry->get<component::Parent>(carriedItemEntity);
					auto& carriedItemTransform = registry->get<component::Transform>(carriedItemEntity);

					if (carriedItemParent.parent == a_playerEntity)
					{
						player.interactableEntity = cannonEntity;

						PlayerInteractResultEvent playerInteractResult;
						playerInteractResult.interactionType = PlayerInteractionResult::loadCannon;
						postEvent(Event::create(playerInteractResult));

						return;
					}
				}
			}
			else if (cannon.state == CannonState::firingSoon)
			{
				//compare x values
				if (   playerTransform.getPositionX() < cannonTransform.getPositionX() + 1.7f
					&& playerTransform.getPositionX() > cannonTransform.getPositionX() - 1.7f
					//compare z values
					&& playerTransform.getPositionZ() < cannonTransform.getPositionZ() + 1.7f
					&& playerTransform.getPositionZ() > cannonTransform.getPositionZ() - 1.7f) //check if cannon can be loaded with a cannonball
				{
					player.interactableEntity = entt::null;

					PlayerInteractResultEvent playerInteractResult;
					playerInteractResult.interactionType = PlayerInteractionResult::cannonFiringSoon;
					postEvent(Event::create(playerInteractResult));

					return;
				}
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

				//TODO: get rid of position comparison once raycasting is in
				//check if player is on the right side of the cannon (will be pushing towards the left)
				if (playerForwardDotCannonRight < -0.65f && !isCannonOnLeftSideOfTrack
					//compare x values
					&& playerTransform.getPositionX() < cannonTransform.getPositionX() + 2.5f
					&& playerTransform.getPositionX() > cannonTransform.getPositionX() + 1.1f
					//compare z values
					&& playerTransform.getPositionZ() < cannonTransform.getPositionZ() + 1.0f
					&& playerTransform.getPositionZ() > cannonTransform.getPositionZ() - 1.0f)
				{
					player.interactableEntity = cannonEntity;

					PlayerInteractResultEvent playerInteractResult;
					playerInteractResult.interactionType = PlayerInteractionResult::pushCannon;
					postEvent(Event::create(playerInteractResult));

					return;
				}
				//TODO: get rid of position comparison once raycasting is in
				//check if player is on the left side of the cannon (will be pushing towards the right)
				else if (playerForwardDotCannonRight > 0.65f && !isCannonOnRightSideOfTrack
					//compare x values
					&& playerTransform.getPositionX() > cannonTransform.getPositionX() - 2.5f
					&& playerTransform.getPositionX() < cannonTransform.getPositionX() - 1.1f
					//compare z values
					&& playerTransform.getPositionZ() < cannonTransform.getPositionZ() + 1.0f
					&& playerTransform.getPositionZ() > cannonTransform.getPositionZ() - 1.0f)
				{
					player.interactableEntity = cannonEntity;

					PlayerInteractResultEvent playerInteractResult;
					playerInteractResult.interactionType = PlayerInteractionResult::pushCannon;
					postEvent(Event::create(playerInteractResult));

					return;
				}
			}
		}
	}

	//no valid interactions (we would've early returned if there was)
	player.interactableEntity = entt::null;

	PlayerInteractResultEvent playerInteractResult;
	playerInteractResult.interactionType = PlayerInteractionResult::nothing;
	postEvent(Event::create(playerInteractResult));
}

void PlayerInteractionValidationSystem::validateInteraction(entt::entity a_playerEntity)
{
	auto& player          = registry->get<Player>(a_playerEntity);
	auto& playerTransform = registry->get<component::Transform>(a_playerEntity);

    //check if there's a valid entity the player can interact with first
	if (player.interactableEntity == entt::null)
		return;

	if (registry->has<CarryableItem>(player.interactableEntity))
	{
		auto& carryableItem          = registry->get<CarryableItem>(player.interactableEntity);
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
		auto& carryableItemParent    = registry->get_or_assign<component::Parent>(player.interactableEntity);
		carryableItemParent.parent   = a_playerEntity;

		carryableItemTransform.setRotationEuler(itemNewRotation);
		carryableItemTransform.setPosition(itemNewPosition);

		return;
	}

	else if (registry->has<GarbagePile>(player.interactableEntity))
	{
		auto& garbagePile          = registry->get<GarbagePile>(player.interactableEntity);
		auto& garbagePileTransform = registry->get<component::Transform>(player.interactableEntity);

		if (garbagePile.team == player.team)
		{
			std::cout << "CLEANING!\n";

			if (garbagePile.garbageTicks < garbagePile.GARBAGE_TICKS_PER_LEVEL)
			{
				//if only a mop is needed for cleaning, drop the cleaning solution if the player is carrying one (mop animation uses both hands)
				if (registry->valid(player.secondaryCarriedItem) && registry->get<CarryableItem>(player.secondaryCarriedItem).type == CarryableItemType::cleaningSolution)
					dropPlayerCarriedItems(a_playerEntity, true, CarryableItemType::cleaningSolution);
			}
			else //garbage ticks >= max ticks, so cleaning solution is used
			{
				registry->destroy(player.secondaryCarriedItem);
				player.secondaryCarriedItem = entt::null;
			}

			RequestToCleanGarbageEvent requestToCleanGarbage;
			requestToCleanGarbage.garbagePileEntity = player.interactableEntity;
			postEvent(Event::create(requestToCleanGarbage));

			PlayerStateChangeEvent playerStateChange;
			playerStateChange.playerEntity = a_playerEntity;
			playerStateChange.newState = PlayerState::cleaning;
			postEvent(Event::create(playerStateChange));

			return;
		}
		else //garbagePile.team != player.team
		{
			std::cout << "USED GLOOP\n";

			garbagePile.isGlooped = true;

			UseGloopEvent useGloop;
			useGloop.gloopEntity = player.primaryCarriedItem;
			postEvent(Event::create(useGloop));
		}
	}

	else if (registry->has<CannonballCrate>(player.interactableEntity))
	{
		auto carryableItemView = registry->view<CarryableItem, component::Transform>();
		for (auto& carryableItemEntity : carryableItemView)
		{
			auto& carryableItem          = registry->get<CarryableItem>(carryableItemEntity);
			auto& carryableItemTransform = registry->get<component::Transform>(carryableItemEntity);

			if (carryableItem.type == CarryableItemType::cannonball && !carryableItem.isActive && !carryableItem.isBeingCarried)
			{
				player.primaryCarriedItem = carryableItemEntity;

				carryableItem.isBeingCarried = true;
				carryableItem.isActive       = true;
							
				std::cout << "OBTAINED CANNONBALL!\n";

				auto& cannonballParent  = registry->get_or_assign<component::Parent>(carryableItemEntity);
				cannonballParent.parent = a_playerEntity;

				carryableItemTransform.setRotationEuler(glm::vec3(0.0f));
				carryableItemTransform.setPosition(glm::vec3(0.0f, 0.35f, -0.75f));

				return;
			}
		}
	}

	else if (registry->has<Cannon>(player.interactableEntity))
	{
		auto& cannon          = registry->get<Cannon>(player.interactableEntity);
		auto& cannonTransform = registry->get<component::Transform>(player.interactableEntity);

		//cannon can be loaded with a cannonball
		if (   registry->valid(player.primaryCarriedItem)
			&& registry->get<CarryableItem>(player.primaryCarriedItem).type == CarryableItemType::cannonball
			&& cannon.isLoaded == false)
		{
			cannon.isLoaded = true;

			auto& carriedItem          = registry->get<CarryableItem>(player.primaryCarriedItem);
			auto& carriedItemParent    = registry->get<component::Parent>(player.primaryCarriedItem);
			auto& carriedItemTransform = registry->get<component::Transform>(player.primaryCarriedItem);

			if (carriedItemParent.parent == a_playerEntity)
			{
				std::cout << "LOADED CANNON!\n";

				carriedItemParent.parent  = entt::null;
				player.primaryCarriedItem = entt::null;

				carriedItem.isBeingCarried = false;
				carriedItem.isActive       = false;
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
				player.adjustingPositionStateData.destinationPos =  cannonTransform.getRight() * glm::vec3(1.2f, 0.0f, 0.0f) + cannonTransform.getPosition();
				player.pushingStateData.destinationPos           = -cannonTransform.getRight() * glm::vec3(cannon.pushDistance, 0.0f, 0.0f) + player.adjustingPositionStateData.destinationPos;
				
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
				player.pushingStateData.destinationPos           =  cannonTransform.getRight() * glm::vec3(cannon.pushDistance, 0.0f, 0.0f) + player.adjustingPositionStateData.destinationPos;

				cannon.pushStateData.destinationPos = cannonTransform.getRight() * glm::vec3(cannon.pushDistance, 0.0f, 0.0f) + cannonTransform.getPosition();
				cannon.cannonTrackPosition++;

				//hacky way to limit the camera but it works for now
				player.yRotationClamp = -50.0f;
				if (playerTransform.getRotationEulerY() > player.yRotationClamp)
					playerTransform.setRotationEulerY(player.yRotationClamp);
			}

			std::cout << "PUSHING!\n";

			player.adjustingPositionStateData.startPos         = playerTransform.getPosition();
			player.adjustingPositionStateData.destinationPos.y = playerTransform.getPositionY(); //don't change the player's y position

			player.pushingStateData.startPos         = player.adjustingPositionStateData.destinationPos;
			player.pushingStateData.destinationPos.y = playerTransform.getPositionY(); //don't change the player's y position

			cannon.pushStateData.startPos = cannonTransform.getPosition();

			//once all the new positions are set for the states, send out state change events
			PlayerStateChangeEvent playerStateChange;
			playerStateChange.playerEntity = a_playerEntity;
			playerStateChange.newState     = PlayerState::pushing;
			postEvent(Event::create(playerStateChange));

			CannonStateChangeEvent cannonStateChange;
			cannonStateChange.cannonEntity = player.interactableEntity;
			cannonStateChange.newState     = CannonState::beingPushed;
			postEvent(Event::create(cannonStateChange));

			return;
		}
	}
}

CannonInteractionOutcome PlayerInteractionValidationSystem::validateRaycastHit(Player* a_player, Cannon* a_cannon)
{
	return CannonInteractionOutcome::invalid;
}

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