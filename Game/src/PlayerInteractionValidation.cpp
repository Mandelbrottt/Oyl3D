#include "PlayerInteractionValidation.h"

void PlayerInteractionValidationSystem::onEnter()
{
	listenForEventCategory((EventCategory)CategoryPlayer);
	listenForEventCategory((EventCategory)CategoryQuicktimeCleaningEvent);
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
			if (   !registry->valid(player.interactableEntity)
				|| !registry->has<GarbagePile>(player.interactableEntity))
			{
				PlayerStateChangeEvent playerStateChange;
				playerStateChange.playerEntity = playerEntity;
				playerStateChange.newState     = PlayerState::idle;
				postEvent(playerStateChange);

				//deactivate CleaningQuicktimeEvent UI for player
				CancelQuicktimeCleaningEventEvent cancelQuicktimeCleaningEvent;
				cancelQuicktimeCleaningEvent.playerNum = player.playerNum;
				postEvent(cancelQuicktimeCleaningEvent);
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
			auto& player = registry->get<Player>(evt.playerEntity);

			if (   registry->valid(player.primaryCarriedItem) 
				&& registry->get<CarryableItem>(player.primaryCarriedItem).type == CarryableItemType::throwableBottle
				&& evt.itemClassifiatonToUse == PlayerItemClassifiation::primary
				&& player.state != PlayerState::throwingBottle)
			{
				PlayerStateChangeEvent playerStateChange;
				playerStateChange.playerEntity = evt.playerEntity;
				playerStateChange.newState     = PlayerState::throwingBottle;
				postEvent(playerStateChange);
			}
			else
				performInteractionForPlayer(evt.playerEntity, evt.itemClassifiatonToUse);
	        
			break;
	    }
		case (EventType)TypeCancelButtonPressed:
		{
			auto evt = event_cast<CancelButtonPressedEvent>(event);

			auto& player = registry->get<Player>(evt.playerEntity);

			if (player.state == PlayerState::inCleaningQuicktimeEvent)
			{
				CancelQuicktimeCleaningEventEvent cancelQTE;
				cancelQTE.playerNum = player.playerNum;
				postEvent(cancelQTE);

				PlayerStateChangeEvent playerStateChange;
				playerStateChange.playerEntity = evt.playerEntity;
				playerStateChange.newState     = PlayerState::idle;
				postEvent(playerStateChange);
			}
			else
				dropPlayerCarriedItems(evt.playerEntity);
			
			break;
		}
		case (EventType)TypePlayerDropItemRequest:
		{
			auto evt = event_cast<PlayerDropItemRequestEvent>(event);

			auto& player = registry->get<Player>(evt.playerEntity);

			switch (evt.itemClassificationToDrop)
			{
				case PlayerItemClassifiation::any:
				{
					dropPlayerCarriedItems(evt.playerEntity);
					break;
				}
				case PlayerItemClassifiation::primary:
				{
					if (registry->valid(player.primaryCarriedItem))
						dropPlayerCarriedItems(evt.playerEntity, true, registry->get<CarryableItem>(player.primaryCarriedItem).type);
					break;
				}
				case PlayerItemClassifiation::secondary:
				{
					if (registry->valid(player.secondaryCarriedItem))
						dropPlayerCarriedItems(evt.playerEntity, true, registry->get<CarryableItem>(player.secondaryCarriedItem).type);
					break;
				}
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

	if (player.state == PlayerState::pushing || player.state == PlayerState::cleaning || player.state == PlayerState::stunned)
	{
		//player cannot switch out of those states states
		player.interactableEntity = entt::null;

		PlayerInteractResultEvent playerInteractResult;
		playerInteractResult.interactionType = PlayerInteractionResult::nothing;
		playerInteractResult.playerNum       = player.playerNum;
		postEvent(playerInteractResult);

		return;
	}

	//get the camera associated with the player
	entt::entity playersCameraEntity = entt::null;

	auto cameraView = registry->view<component::Camera, component::Transform>();
	for (entt::entity cameraEntity : cameraView)
	{
		if (player.playerNum == registry->get<component::Camera>(cameraEntity).player)
		{
			playersCameraEntity = cameraEntity;
			//set the camera parent to the player in case it hasn't already been done
			auto& cameraParent  = registry->get_or_assign<component::Parent>(cameraEntity);
			cameraParent.parent = a_playerEntity;
			
			break;
		}
	}
	if (!registry->valid(playersCameraEntity))
	{
		OYL_LOG("WARNING: PLAYER {} DOES NOT HAVE AN ASSOCIATED CAMERA", (uint)player.playerNum + 1);
		return;
	}

	auto& playerCameraTransform = registry->get<component::Transform>(playersCameraEntity);

	auto ray = RayTest::Closest(playerCameraTransform.getPositionGlobal(), playerCameraTransform.getForwardGlobal(), 2.0f);
	if (ray->hasHit && registry->valid(ray->hitObject.entity))
	{
		entt::entity raycastHitEntity = ray->hitObject.entity;

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
			playerInteractResult.playerNum       = player.playerNum;
			postEvent(playerInteractResult);
		}
	}
	else //no ray hits
	{
		//no valid interactions
		player.interactableEntity = entt::null;

		PlayerInteractResultEvent playerInteractResult;
		playerInteractResult.interactionType = PlayerInteractionResult::nothing;
		playerInteractResult.playerNum       = player.playerNum;
		postEvent(playerInteractResult);
	}
}

#pragma region VALIDATION FUNCTIONS
////////////////////////////////////////////////////////////////////
/////////////////////// VALIDATION FUNCTIONS ///////////////////////
////////////////////////////////////////////////////////////////////
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
				PlayerInteractResultEvent playerInteractResult;
				playerInteractResult.playerNum = player.playerNum;

				if (!registry->valid(player.primaryCarriedItem) && !registry->valid(player.secondaryCarriedItem))
				{
					player.interactableEntity = a_carryableItemEntity;

					playerInteractResult.interactionType = PlayerInteractionResult::pickUpCannonball;
				}
				else //player is carrying items
				{
					player.interactableEntity = entt::null;

					playerInteractResult.interactionType = PlayerInteractionResult::needToDropItems;
				}

				postEvent(playerInteractResult);
				return;
			}
			case CarryableItemType::mop:
			{
				PlayerInteractResultEvent playerInteractResult;
				playerInteractResult.playerNum = player.playerNum;

				if (!registry->valid(player.primaryCarriedItem))
				{
					player.interactableEntity = a_carryableItemEntity;

					playerInteractResult.interactionType = PlayerInteractionResult::pickUpMop;
				}
				else //player is carrying a primary item
				{
					player.interactableEntity = entt::null;

					playerInteractResult.interactionType = PlayerInteractionResult::needToDropItems;
				}

				postEvent(playerInteractResult);
				return;
			}
			case CarryableItemType::cleaningSolution:
			{
				PlayerInteractResultEvent playerInteractResult;
				playerInteractResult.playerNum = player.playerNum;

				if (!registry->valid(player.secondaryCarriedItem))
				{
					player.interactableEntity = a_carryableItemEntity;

					playerInteractResult.interactionType = PlayerInteractionResult::pickUpCleaningSolution;
				}
				else //player is carrying a secondary item
				{
					player.interactableEntity = entt::null;

					playerInteractResult.interactionType = PlayerInteractionResult::needToDropItems;
				}

				postEvent(playerInteractResult);
				return;
			}
			case CarryableItemType::gloop:
			{
				PlayerInteractResultEvent playerInteractResult;
				playerInteractResult.playerNum = player.playerNum;

				if (!registry->valid(player.secondaryCarriedItem))
				{
					player.interactableEntity = a_carryableItemEntity;

					playerInteractResult.interactionType = PlayerInteractionResult::pickUpGloop;
				}
				else //player is carrying a secondary item
				{
					player.interactableEntity = entt::null;

					playerInteractResult.interactionType = PlayerInteractionResult::needToDropItems;
				}

				postEvent(playerInteractResult);
				return;
			}
			case CarryableItemType::throwableBottle:
			{
				PlayerInteractResultEvent playerInteractResult;
				playerInteractResult.playerNum = player.playerNum;

				if (!registry->valid(player.primaryCarriedItem))
				{
					if (registry->get<ThrowableBottle>(a_carryableItemEntity).isBeingThrown) //cannot pick up a bottle while it's being thrown
						break;

					player.interactableEntity = a_carryableItemEntity;

					playerInteractResult.interactionType = PlayerInteractionResult::pickUpThrowableBottle;
				}
				else //player is carrying a primary item
				{
					player.interactableEntity = entt::null;

					playerInteractResult.interactionType = PlayerInteractionResult::needToDropItems;
				}

				postEvent(playerInteractResult);
				return;
			}
		}
	}

	//no early return means there is no valid result
	player.interactableEntity = entt::null;

	PlayerInteractResultEvent playerInteractResult;
	playerInteractResult.interactionType = PlayerInteractionResult::invalid;
	playerInteractResult.playerNum       = player.playerNum;
	postEvent(playerInteractResult);
}

void PlayerInteractionValidationSystem::validateGarbagePileInteraction(entt::entity a_playerEntity, entt::entity a_garbagePileEntity)
{
	auto& player          = registry->get<Player>(a_playerEntity);
	auto& playerTransform = registry->get<component::Transform>(a_playerEntity);

	auto& garbagePile          = registry->get<GarbagePile>(a_garbagePileEntity);
	auto& garbagePileTransform = registry->get<component::Transform>(a_garbagePileEntity);

	if (garbagePile.garbageLevel == garbagePile.MAX_GARBAGE_LEVEL && garbagePile.garbageTicks == garbagePile.GARBAGE_TICKS_PER_LEVEL)
	{
		//garbage pile is maxed out and cannot be cleaned anymore
		player.interactableEntity = entt::null;

		PlayerInteractResultEvent playerInteractResult;
		playerInteractResult.interactionType = PlayerInteractionResult::invalid;
		playerInteractResult.playerNum       = player.playerNum;
		postEvent(playerInteractResult);

		return;
	}
	else if (garbagePile.garbageLevel <= 0)
	{
		//there is no garbage pile there to even interact with (so it shouldn't even count as invalid, just nothing)
		player.interactableEntity = entt::null;

		PlayerInteractResultEvent playerInteractResult;
		playerInteractResult.interactionType = PlayerInteractionResult::nothing;
		playerInteractResult.playerNum       = player.playerNum;
		postEvent(playerInteractResult);

		return;
	}
	else //garbageLevel > 0
	{
		if (garbagePile.team == player.team)
		{
			if (garbagePile.garbageTicks <= garbagePile.GARBAGE_TICKS_PER_LEVEL * 0.75f) //75% or lower requires mop to clean
			{
				if (   registry->valid(player.primaryCarriedItem)
					&& registry->get<CarryableItem>(player.primaryCarriedItem).type == CarryableItemType::mop
					&& registry->get<CarryableItem>(player.primaryCarriedItem).team == player.team)
				{
					player.interactableEntity = a_garbagePileEntity;

					if (player.state == PlayerState::inCleaningQuicktimeEvent)
					{
						//if they're already in the QTE, we don't want to keep showing them the message
						PlayerInteractResultEvent playerInteractResult;
						playerInteractResult.interactionType = PlayerInteractionResult::nothing;
						playerInteractResult.playerNum       = player.playerNum;
						postEvent(playerInteractResult);
					}
					else
					{
						PlayerInteractResultEvent playerInteractResult;
						playerInteractResult.interactionType = PlayerInteractionResult::cleanGarbagePile;
						playerInteractResult.playerNum       = player.playerNum;
						postEvent(playerInteractResult);
					}

					return;
				}
				else //player is not carrying mop
				{
					player.interactableEntity = entt::null;

					PlayerInteractResultEvent playerInteractResult;
					playerInteractResult.interactionType = PlayerInteractionResult::needMop;
					playerInteractResult.playerNum       = player.playerNum;
					postEvent(playerInteractResult);

					return;
				}
			}
			else //garbage ticks at cleaning solution required threshold
			{
				//garbage ticks at max means the player needs cleaning solution to be able to clean (cleaning quicktime event is not displayed in this case)
				if (   registry->valid(player.secondaryCarriedItem)
					&& registry->get<CarryableItem>(player.secondaryCarriedItem).type == CarryableItemType::cleaningSolution
					&& registry->get<CarryableItem>(player.secondaryCarriedItem).team == player.team)
				{
					player.interactableEntity = a_garbagePileEntity;

					PlayerInteractResultEvent playerInteractResult;
					playerInteractResult.interactionType = PlayerInteractionResult::cleanGarbagePile;
					playerInteractResult.playerNum       = player.playerNum;
					postEvent(playerInteractResult);

					return;
				}
				else //player is not carrying cleaning solution
				{
					player.interactableEntity = entt::null;

					PlayerInteractResultEvent playerInteractResult;
					playerInteractResult.interactionType = PlayerInteractionResult::needCleaningSolution;
					playerInteractResult.playerNum       = player.playerNum;
					postEvent(playerInteractResult);

					return;
				}
			}
		}
		else //garbagePile.team != player.team
		{
			if (   !garbagePile.isGlooped
				&& registry->valid(player.secondaryCarriedItem)
				&& registry->get<CarryableItem>(player.secondaryCarriedItem).type == CarryableItemType::gloop
				&& registry->get<CarryableItem>(player.secondaryCarriedItem).team == player.team)
			{
				player.interactableEntity = a_garbagePileEntity;

				PlayerInteractResultEvent playerInteractResult;
				playerInteractResult.interactionType = PlayerInteractionResult::useGloop;
				playerInteractResult.playerNum       = player.playerNum;
				postEvent(playerInteractResult);

				return;
			}
		}
	}

	//no early return means there is no valid result
	player.interactableEntity = entt::null;

	PlayerInteractResultEvent playerInteractResult;
	playerInteractResult.interactionType = PlayerInteractionResult::invalid;
	playerInteractResult.playerNum       = player.playerNum;
	postEvent(playerInteractResult);
}

void PlayerInteractionValidationSystem::validateCannonballCrateInteraction(entt::entity a_playerEntity, entt::entity a_cannonballCrateEntity)
{
	auto& player          = registry->get<Player>(a_playerEntity);
	auto& playerTransform = registry->get<component::Transform>(a_playerEntity);

	auto& cannonballCrate          = registry->get<CannonballCrate>(a_cannonballCrateEntity);
	auto& cannonballCrateTransform = registry->get<component::Transform>(a_cannonballCrateEntity);

	if (   !registry->valid(player.primaryCarriedItem)
		&& !registry->valid(player.secondaryCarriedItem))
	{
		if (cannonballCrate.team == player.team)
		{
			player.interactableEntity = a_cannonballCrateEntity;

			PlayerInteractResultEvent playerInteractResult;
			playerInteractResult.interactionType = PlayerInteractionResult::takeCannonballFromCrate;
			playerInteractResult.playerNum       = player.playerNum;
			postEvent(playerInteractResult);

			return;
		}
	}
	else //player is carrying items
	{
		player.interactableEntity = entt::null;

		PlayerInteractResultEvent playerInteractResult;
		playerInteractResult.interactionType = PlayerInteractionResult::needToDropItems;
		playerInteractResult.playerNum       = player.playerNum;
		postEvent(playerInteractResult);

		return;
	}

	//no early return means there is no valid result
	player.interactableEntity = entt::null;

	PlayerInteractResultEvent playerInteractResult;
	playerInteractResult.interactionType = PlayerInteractionResult::invalid;
	playerInteractResult.playerNum       = player.playerNum;
	postEvent(playerInteractResult);
}

void PlayerInteractionValidationSystem::validateCannonInteraction(entt::entity a_playerEntity, entt::entity a_cannonEntity)
{
	auto& player          = registry->get<Player>(a_playerEntity);
	auto& playerTransform = registry->get<component::Transform>(a_playerEntity);

	auto& cannon          = registry->get<Cannon>(a_cannonEntity);
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
			playerInteractResult.playerNum       = player.playerNum;
			postEvent(playerInteractResult);

			return;
		}
		else if (cannon.state == CannonState::firingSoon)
		{
			player.interactableEntity = entt::null;

			PlayerInteractResultEvent playerInteractResult;
			playerInteractResult.interactionType = PlayerInteractionResult::cannonFiringSoon;
			playerInteractResult.playerNum       = player.playerNum;
			postEvent(playerInteractResult);

			return;
		}
		else if (cannon.state == CannonState::doingNothing)
		{
			if (registry->valid(player.primaryCarriedItem) || registry->valid(player.secondaryCarriedItem))
			{
				//the player cannot push the cannon while carrying item(s)
				player.interactableEntity = entt::null;

				PlayerInteractResultEvent playerInteractResult;
				playerInteractResult.interactionType = PlayerInteractionResult::needToDropItems;
				playerInteractResult.playerNum       = player.playerNum;
				postEvent(playerInteractResult);

				return;
			}

			float playerForwardDotCannonRight = glm::dot(playerTransform.getForward(), cannonTransform.getRight());

			bool isCannonOnLeftSideOfTrack  = (cannon.cannonTrackPosition == -1)
				? true : false;
			bool isCannonOnRightSideOfTrack = (cannon.cannonTrackPosition == 1)
				? true : false;

			//make sure the player is beside the cannon (don't let them push if they're directionly in front or behind it)
			if (   playerTransform.getPositionX() < cannonTransform.getPositionX() - 1.1f
				|| playerTransform.getPositionX() > cannonTransform.getPositionX() + 1.1f)
			{
				//check if player is on the right side of the cannon (will be pushing towards the left)
				if (playerForwardDotCannonRight < -0.1f && !isCannonOnLeftSideOfTrack)
				{
					player.interactableEntity = a_cannonEntity;

					PlayerInteractResultEvent playerInteractResult;
					playerInteractResult.interactionType = PlayerInteractionResult::pushCannon;
					playerInteractResult.playerNum       = player.playerNum;
					postEvent(playerInteractResult);

					return;
				}
				//check if player is on the left side of the cannon (will be pushing towards the right)
				else if (playerForwardDotCannonRight > 0.1f && !isCannonOnRightSideOfTrack)
				{
					player.interactableEntity = a_cannonEntity;

					PlayerInteractResultEvent playerInteractResult;
					playerInteractResult.interactionType = PlayerInteractionResult::pushCannon;
					playerInteractResult.playerNum       = player.playerNum;
					postEvent(playerInteractResult);

					return;
				}
			}
		}
	}

	//no early return means there is no valid result
	player.interactableEntity = entt::null;

	PlayerInteractResultEvent playerInteractResult;
	playerInteractResult.interactionType = PlayerInteractionResult::invalid;
	playerInteractResult.playerNum       = player.playerNum;
	postEvent(playerInteractResult);
}
#pragma endregion

#pragma region STATE CHANGING FUNCTIONS
////////////////////////////////////////////////////////////////////
///////////////////// STATE CHANGING FUNCTIONS /////////////////////
////////////////////////////////////////////////////////////////////

//the player's interactable entity would have been validated and set based on the result of the raycast performed every frame
void PlayerInteractionValidationSystem::performInteractionForPlayer(entt::entity a_playerEntity, PlayerItemClassifiation itemClassification)
{
	auto& player = registry->get<Player>(a_playerEntity);

	//TODO: get rid of this for final version, it's only for testing. Mashing e will clean, only here so that you can still clean piles on KBM for testing
	if (player.state == PlayerState::inCleaningQuicktimeEvent && player.playerNum == PlayerNumber::One)
	{
		RequestToCleanGarbageEvent requestToCleanGarbage;
		requestToCleanGarbage.garbagePileEntity = player.interactableEntity;
		postEvent(requestToCleanGarbage);

		return;
	}

	//check if there's a valid entity the player can interact with first
	if (player.interactableEntity == entt::null)
		return;

	if (registry->has<CarryableItem>(player.interactableEntity))
		performCarryableItemInteraction(a_playerEntity, player.interactableEntity, itemClassification);

	else if (registry->has<GarbagePile>(player.interactableEntity))
		performGarbagePileInteraction(a_playerEntity, player.interactableEntity, itemClassification);

	else if (registry->has<CannonballCrate>(player.interactableEntity))
		performCannonballCrateInteraction(a_playerEntity, player.interactableEntity, itemClassification);

	else if (registry->has<Cannon>(player.interactableEntity))
		performCannonInteraction(a_playerEntity, player.interactableEntity, itemClassification);
}

void PlayerInteractionValidationSystem::performCarryableItemInteraction(entt::entity a_playerEntity, entt::entity a_carryableItemEntity, PlayerItemClassifiation itemClassification)
{
	auto& player = registry->get<Player>(a_playerEntity);

	auto& carryableItem          = registry->get<CarryableItem>(a_carryableItemEntity);
	auto& carryableItemTransform = registry->get<component::Transform>(a_carryableItemEntity);
	
	glm::vec3 itemNewPosition = glm::vec3(0.0f);
	glm::vec3 itemNewRotation = glm::vec3(0.0f);

	switch (carryableItem.type)
	{
		case CarryableItemType::cannonball:
		{
			player.primaryCarriedItem   = a_carryableItemEntity;
			player.secondaryCarriedItem = a_carryableItemEntity;

			OYL_LOG("PICKED UP CANNONBALL!");

			itemNewPosition = glm::vec3(0.0f, 0.4f, -0.7f);
			itemNewRotation = glm::vec3(0.0f, 0.0f, 0.0f);

			break;
		}
		case CarryableItemType::mop:
		{
			if (itemClassification == PlayerItemClassifiation::secondary)
				return;

			player.primaryCarriedItem = a_carryableItemEntity;

			OYL_LOG("PICKED UP MOP!");

			itemNewPosition = glm::vec3(0.45f, 0.55f, -0.35f);
			itemNewRotation = glm::vec3(-100.0f, -54.7f, 99.0f);

			break;
		}
		case CarryableItemType::cleaningSolution:
		{
			if (itemClassification == PlayerItemClassifiation::primary)
				return;

			player.secondaryCarriedItem = a_carryableItemEntity;

			OYL_LOG("PICKED UP CLEANING SOLUTION!");

			itemNewPosition = glm::vec3(-0.2f, 0.45f, -0.55f);
			itemNewRotation = glm::vec3(0.0f, 0.0f, 0.0f);

			break;
		}
		case CarryableItemType::gloop:
		{
			if (itemClassification == PlayerItemClassifiation::primary)
				return;

			player.secondaryCarriedItem = a_carryableItemEntity;

			OYL_LOG("PICKED UP GLOOP!");

			itemNewPosition = glm::vec3(-0.2f, 0.58f, -0.55f);
			itemNewRotation = glm::vec3(0.0f, 0.0f, 0.0f);

			break;
		}
		case CarryableItemType::throwableBottle:
		{
			if (itemClassification == PlayerItemClassifiation::secondary)
				return;

			player.primaryCarriedItem = a_carryableItemEntity;

			OYL_LOG("PICKED UP THROWABLE BOTTLE!");

			itemNewPosition = glm::vec3(0.3f, 0.55f, -0.55f);
			itemNewRotation = glm::vec3(0.0f, 0.0f, 0.0f);

			break;
		}
		default:
		{
			OYL_LOG("INVALID ITEM TYPE: TRIED TO PICK UP AN ITEM BUT THE ITEM TYPE WAS NOT A VALID OPTION!");
			break;
		}
	}

	auto& carryableItemParent = registry->get_or_assign<component::Parent>(a_carryableItemEntity);

	//remove rigidbody when item is carried
	if (registry->has<component::RigidBody>(a_carryableItemEntity))
		registry->remove<component::RigidBody>(a_carryableItemEntity);

	carryableItem.isBeingCarried = true;
	carryableItem.hasBeenCarried = true;
	carryableItemParent.parent   = a_playerEntity;

	carryableItemTransform.setRotationEuler(itemNewRotation);
	carryableItemTransform.setPosition(itemNewPosition);
}

void PlayerInteractionValidationSystem::performGarbagePileInteraction(entt::entity a_playerEntity, entt::entity a_garbagePileEntity, PlayerItemClassifiation itemClassification)
{
	auto& player = registry->get<Player>(a_playerEntity);

	auto& garbagePile = registry->get<GarbagePile>(a_garbagePileEntity);

	if (garbagePile.team == player.team)
	{
		//75% or lower requires mop to clean
		if (garbagePile.garbageTicks <= garbagePile.GARBAGE_TICKS_PER_LEVEL * 0.75f) 
		{
			if (itemClassification == PlayerItemClassifiation::primary || itemClassification == PlayerItemClassifiation::any)
			{
				//only a mop is needed for cleaning, drop the secondary item if the player is carrying one (mop animation uses both hands)
				if (registry->valid(player.secondaryCarriedItem))
					dropPlayerCarriedItems(a_playerEntity, true, registry->get<CarryableItem>(player.secondaryCarriedItem).type);

				PlayerStateChangeEvent playerStateChange;
				playerStateChange.playerEntity = a_playerEntity;
				playerStateChange.newState = PlayerState::inCleaningQuicktimeEvent;
				postEvent(playerStateChange);

				ActivateQuicktimeCleaningEventEvent activateCleaningQTE;
				activateCleaningQTE.playerNum = player.playerNum;
				activateCleaningQTE.garbagePileEntity = player.interactableEntity;
				postEvent(activateCleaningQTE);
			}
		}
		else //cleaning solution is required
		{
			if (itemClassification == PlayerItemClassifiation::secondary || itemClassification == PlayerItemClassifiation::any)
			{
				auto& carryableItem = registry->get<CarryableItem>(player.secondaryCarriedItem);
				auto& carryableItemTransform = registry->get<component::Transform>(player.secondaryCarriedItem);

				registry->destroy(player.secondaryCarriedItem);
				player.secondaryCarriedItem = entt::null;

				RequestToCleanGarbageEvent requestToCleanGarbage;
				requestToCleanGarbage.garbagePileEntity = a_garbagePileEntity;
				postEvent(requestToCleanGarbage);

				PlayerStateChangeEvent playerStateChange;
				playerStateChange.playerEntity = a_playerEntity;
				playerStateChange.newState = PlayerState::cleaning;
				postEvent(playerStateChange);
			}
		}
	}
	else //garbagePile.team != player.team
	{
		if (itemClassification == PlayerItemClassifiation::secondary || itemClassification == PlayerItemClassifiation::any)
		{
			OYL_LOG("USED GLOOP");

			garbagePile.isGlooped = true;

			UseGloopEvent useGloop;
			useGloop.gloopEntity = player.secondaryCarriedItem;
			postEvent(useGloop);

			PlayerStateChangeEvent playerStateChange;
			playerStateChange.playerEntity = a_playerEntity;
			playerStateChange.newState = PlayerState::cleaning; //not exactly cleaning but close enough. We want the same delay and movement slow effect anyway so might as well reuse it
			postEvent(playerStateChange);
		}
	}
}

void PlayerInteractionValidationSystem::performCannonballCrateInteraction(entt::entity a_playerEntity, entt::entity a_cannonballCrateEntity, PlayerItemClassifiation itemClassification)
{
	auto& player = registry->get<Player>(a_playerEntity);

	auto& cannonballCrate = registry->get<CannonballCrate>(a_cannonballCrateEntity);

	SpawnCannonballForPlayerEvent spawnCannonball;
	spawnCannonball.playerEntity = a_playerEntity;
	spawnCannonball.team         = cannonballCrate.team;
	postEvent(spawnCannonball);

	return;
}

void PlayerInteractionValidationSystem::performCannonInteraction(entt::entity a_playerEntity, entt::entity a_cannonEntity, PlayerItemClassifiation itemClassification)
{
	auto& player          = registry->get<Player>(a_playerEntity);
	auto& playerTransform = registry->get<component::Transform>(a_playerEntity);

	auto& cannon          = registry->get<Cannon>(a_cannonEntity);
	auto& cannonTransform = registry->get<component::Transform>(a_cannonEntity);

	//cannon can be loaded with a cannonball
	if (   registry->valid(player.primaryCarriedItem)
		&& registry->has<Cannonball>(player.primaryCarriedItem)
		&& cannon.isLoaded == false)
	{
		cannon.isLoaded = true;

		auto& cannonball           = registry->get<Cannonball>(player.primaryCarriedItem);
		auto& carriedItem          = registry->get<CarryableItem>(player.primaryCarriedItem);
		auto& carriedItemParent    = registry->get<component::Parent>(player.primaryCarriedItem);
		auto& carriedItemTransform = registry->get<component::Transform>(player.primaryCarriedItem);

		if (carriedItemParent.parent == a_playerEntity)
		{
			OYL_LOG("LOADED CANNON!");

			carriedItemParent.parent    = entt::null;
			player.primaryCarriedItem   = entt::null;
			player.secondaryCarriedItem = entt::null;
			
			cannonball.isWaitingToBeFired = true;

			carriedItem.isBeingCarried = false;
			carriedItemTransform.setPosition(glm::vec3(-99999.0f));

			return;
		}
	}
	else
	{
		float playerForwardDotCannonRight = glm::dot(playerTransform.getForward(), cannonTransform.getRight());

		bool isCannonOnLeftSideOfTrack  = (cannon.cannonTrackPosition == -1)
			? true : false;
		bool isCannonOnRightSideOfTrack = (cannon.cannonTrackPosition == 1)
			? true : false;

		OYL_LOG("PLAYER FORWARD DOT PRODUCT W/ CANNON RIGHT: {}", playerForwardDotCannonRight);

		//check if player is on the right side of the cannon (will be pushing towards the left)
		if (playerForwardDotCannonRight < -0.1f)
		{
			//set direction-specific variables in the if statement and non-direction specific variables outside
			player.adjustingPositionStateData.destinationPos = cannonTransform.getRight() * glm::vec3(2.4f, 0.0f, 0.0f) + cannonTransform.getPosition();

			player.pushingStateData.destinationPos = -cannonTransform.getRight() * glm::vec3(cannon.pushDistance, 0.0f, 0.0f) + player.adjustingPositionStateData.destinationPos;

			cannon.pushStateData.destinationPos = -cannonTransform.getRight() * glm::vec3(cannon.pushDistance, 0.0f, 0.0f) + cannonTransform.getPosition();

			cannon.cannonTrackPosition--;
		}
		//player is on the left side of the cannon (will be pushing towards the right)
		else
		{
			//set direction-specific variables in the if statement and non-direction specific variables outside
			player.adjustingPositionStateData.destinationPos = -cannonTransform.getRight() * glm::vec3(2.4f, 0.0f, 0.0f) + cannonTransform.getPosition();

			player.pushingStateData.destinationPos = cannonTransform.getRight() * glm::vec3(cannon.pushDistance, 0.0f, 0.0f) + player.adjustingPositionStateData.destinationPos;

			cannon.pushStateData.destinationPos = cannonTransform.getRight() * glm::vec3(cannon.pushDistance, 0.0f, 0.0f) + cannonTransform.getPosition();

			cannon.cannonTrackPosition++;
		}

		OYL_LOG("PUSHING!");

		player.adjustingPositionStateData.startPos         = playerTransform.getPosition();
		player.adjustingPositionStateData.destinationPos.y = playerTransform.getPositionY(); //don't change the player's y position
		
		player.pushingStateData.startPos         = player.adjustingPositionStateData.destinationPos;
		player.pushingStateData.destinationPos.y = playerTransform.getPositionY(); //don't change the player's y position

		cannon.pushStateData.startPos = cannonTransform.getPosition();

		//hacky way to limit the camera but it works
		if (playerTransform.getPositionX() > cannonTransform.getPositionX())
		{
			player.yRotationClamp = 30.0f;
			if (playerTransform.getRotationEulerY() < player.yRotationClamp)
				playerTransform.setRotationEulerY(player.yRotationClamp);
		}
		else
		{
			player.yRotationClamp = -30.0f;
			if (playerTransform.getRotationEulerY() > player.yRotationClamp)
				playerTransform.setRotationEulerY(player.yRotationClamp);
		}


		//once all the new positions are set for the states, send out state change events
		PlayerStateChangeEvent playerStateChange;
		playerStateChange.playerEntity = a_playerEntity;
		playerStateChange.newState     = PlayerState::pushing;
		postEvent(playerStateChange);

		CannonStateChangeEvent cannonStateChange;
		cannonStateChange.cannonEntity = a_cannonEntity;
		cannonStateChange.newState     = CannonState::beingPushed;
		postEvent(cannonStateChange);

		return;
	}
}
#pragma endregion

#pragma region HELPER FUNCTIONS
////////////////////////////////////////////////////////////////////
//////////////////////// HELPER FUNCTIONS //////////////////////////
////////////////////////////////////////////////////////////////////
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
		//dont let player drop items while cleaning and check parent
		if (player.state == PlayerState::cleaning || carriedItemParent.parent != a_playerEntity)
			continue;

		auto& carriedItemRB = registry->get_or_assign<component::RigidBody>(carriedItemEntity); //add the rigidbody back for the item when it's dropped

		carriedItemParent.parent   = entt::null;
		carriedItem.isBeingCarried = false;

		glm::vec3 newPosition = playerTransform.getPosition();
		glm::vec3 newRotation = playerTransform.getRotationEuler();

		switch (carriedItem.type)
		{
			case CarryableItemType::cannonball:
			{
				OYL_LOG("DROPPED CANNONBALL!");
				player.primaryCarriedItem   = entt::null;
				player.secondaryCarriedItem = entt::null;

				newPosition += playerTransform.getForward() * 0.75f;

				break;
			}
			case CarryableItemType::mop:
			{
				OYL_LOG("DROPPED MOP!");
				player.primaryCarriedItem = entt::null;

				newPosition += playerTransform.getForward() * 0.15f;
				newPosition += playerTransform.getRight() * 0.3f;
				newPosition.y = playerTransform.getPositionY();

				newRotation.y += playerTransform.getForward().z > 0.0f ? 90.0f : -90.0f;

				break;
			}
			case CarryableItemType::cleaningSolution:
			{
				OYL_LOG("DROPPED CLEANING SOLUTION!");
				player.secondaryCarriedItem = entt::null;

				newPosition += playerTransform.getForward() * 0.63f;
				newPosition += playerTransform.getRight() * -0.2f;
				newPosition += playerTransform.getUp() * 0.4f;

				break;
			}
			case CarryableItemType::gloop:
			{
				OYL_LOG("DROPPED GLOOP!");
				player.secondaryCarriedItem = entt::null;

				newPosition += playerTransform.getForward() * 0.7f;
				newPosition += playerTransform.getRight() * -0.2f;
				newPosition += playerTransform.getUp() * 0.4f;

				break;
			}
			case CarryableItemType::throwableBottle:
			{
				OYL_LOG("DROPPED THROWABLE BOTTLE!");
				player.primaryCarriedItem = entt::null;

				newPosition += playerTransform.getForward() * 0.7f;
				newPosition += playerTransform.getRight() * 0.25f;
				newPosition += playerTransform.getUp() * 0.45f;

				break;
			}
		}

		carriedItemTransform.setPosition(newPosition);
		carriedItemTransform.setRotationEuler(newRotation);
	}
}
#pragma endregion