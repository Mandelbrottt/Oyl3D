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
	{
		auto& player = registry->get<Player>(playerEntity);
		auto& playerTransform = registry->get<component::Transform>(playerEntity);

		checkForAnyValidPlayerInteractions(playerEntity, &player, &playerTransform);
	}
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
			    {
					auto& player          = registry->get<Player>(playerEntity);
					auto& playerTransform = registry->get<component::Transform>(playerEntity);
			        
					validateInteraction(playerEntity, &player, &playerTransform);
			    }
			}
	        
			break;
	    }

		case TypePlayerDropItem:
		{
			auto evt = (PlayerDropItemEvent)* event;

			auto playerView = registry->view<Player, component::Transform>();
			for (entt::entity playerEntity : playerView)
			{
				if (evt.playerEntity == playerEntity)
				{
					auto& player = registry->get<Player>(playerEntity);
					auto& playerTransform = registry->get<component::Transform>(playerEntity);

					auto carriedItemsView = registry->view<CarryableItem, component::Parent, component::Transform>();
					for (entt::entity carriedItemEntity : carriedItemsView)
					{
						auto& carriedItem = registry->get<CarryableItem>(carriedItemEntity);
						auto& carriedItemParent = registry->get<component::Parent>(carriedItemEntity);
						auto& carriedItemTransform = registry->get<component::Transform>(carriedItemEntity);

						if (carriedItemParent.parent == playerEntity)
						{
							std::cout << "DROPPED ITEMS!\n";

							carriedItemParent.parent = entt::null;
							player.carriedItem = CarryingItemState::nothing;
							carriedItem.isBeingCarried = false;
							carriedItem.isActive = false;

							glm::vec3 newPosition = playerTransform.getPosition();
							glm::vec3 newRotation = playerTransform.getRotationEuler();

							if (carriedItem.type == CarryableItemType::mop)
							{
								newPosition += playerTransform.getForward() * 0.7f;
								newPosition += playerTransform.getRight() * 0.7f;
								newPosition.y = 0.1f;

								newRotation += glm::vec3(0.0f, -90.0f, 0.0f);
							}
							else if (carriedItem.type == CarryableItemType::cannonball)
							{
								newPosition += playerTransform.getForward() * 0.8f;
								newPosition.y = 0.25f;
							}

							carriedItemTransform.setPosition(newPosition);
							carriedItemTransform.setRotationEuler(newRotation);
						}
					}
				}
			}

			break;
		}
	}

	return false;
}

void PlayerInteractionValidationSystem::checkForAnyValidPlayerInteractions(entt::entity a_playerEntity, Player* a_player, component::Transform* a_playerTransform)
{
	bool isValidInteraction = false; //TODO: remove once raycast is in

	//check all cannons
	auto view = registry->view<Cannon, component::Transform>();
	for (auto& entity : view)
	{
		auto& cannon = registry->get<Cannon>(entity);
		auto& cannonTransform = registry->get<component::Transform>(entity);

		a_player->interactableEntity = entity; //TODO: remove this once proper validation (raycast) is in

		if (a_player->interactableEntity == entity)
		{
			//cannon can be loaded with a cannonball
			if (a_player->carriedItem == CarryingItemState::cannonball
				&& cannon.isLoaded == false
				//compare x values
				&& a_playerTransform->getPositionX() < cannonTransform.getPositionX() + 1.7f
				&& a_playerTransform->getPositionX() > cannonTransform.getPositionX() - 1.7f
				//compare z values
				&& a_playerTransform->getPositionZ() < cannonTransform.getPositionZ() + 1.7f
				&& a_playerTransform->getPositionZ() > cannonTransform.getPositionZ() - 1.7f) //check if cannon can be loaded with a cannonball
			{
				isValidInteraction = true;
				cannon.isLoaded = true;

				//get rid of the cannonball the player has loaded
				auto carriedItemsView = registry->view<CarryableItem, component::Parent, component::Transform>();
				for (entt::entity carriedItemEntity : carriedItemsView)
				{
					auto& carriedItem = registry->get<CarryableItem>(carriedItemEntity);
					auto& carriedItemParent = registry->get<component::Parent>(carriedItemEntity);
					auto& carriedItemTransform = registry->get<component::Transform>(carriedItemEntity);

					if (carriedItemParent.parent == a_playerEntity)
					{
						isValidInteraction = true;

						PlayerInteractResultEvent playerInteractResult;
						playerInteractResult.interactionType = PlayerInteractionResult::loadCannon;
						postEvent(Event::create(playerInteractResult));
					}
				}
			}
			else if (cannon.state == CannonState::doingNothing && a_player->carriedItem == CarryingItemState::nothing) //cannon can be pushed TODO: get rid of conditions after raycast validation is in, should just be an else
			{
				float playerForwardDotCannonRight = glm::dot(a_playerTransform->getForward(), cannonTransform.getRight());

				bool isCannonOnLeftSideOfTrack = (cannon.cannonTrackPosition == -1)
					? true : false;
				bool isCannonOnRightSideOfTrack = (cannon.cannonTrackPosition == 1)
					? true : false;

				//TODO: get rid of position comparison once raycasting is in
				//check if player is on the right side of the cannon (will be pushing towards the left)
				if (playerForwardDotCannonRight < -0.65f && !isCannonOnLeftSideOfTrack
					//compare x values
					&& a_playerTransform->getPositionX() < cannonTransform.getPositionX() + 2.5f
					&& a_playerTransform->getPositionX() > cannonTransform.getPositionX() + 1.1f
					//compare z values
					&& a_playerTransform->getPositionZ() < cannonTransform.getPositionZ() + 1.0f
					&& a_playerTransform->getPositionZ() > cannonTransform.getPositionZ() - 1.0f)
				{
					isValidInteraction = true;

					PlayerInteractResultEvent playerInteractResult;
					playerInteractResult.interactionType = PlayerInteractionResult::pushCannon;
					postEvent(Event::create(playerInteractResult));
				}
				//TODO: get rid of position comparison once raycasting is in
				//check if player is on the left side of the cannon (will be pushing towards the right)
				else if (playerForwardDotCannonRight > 0.65f && !isCannonOnRightSideOfTrack
					//compare x values
					&& a_playerTransform->getPositionX() > cannonTransform.getPositionX() - 2.5f
					&& a_playerTransform->getPositionX() < cannonTransform.getPositionX() - 1.1f
					//compare z values
					&& a_playerTransform->getPositionZ() < cannonTransform.getPositionZ() + 1.0f
					&& a_playerTransform->getPositionZ() > cannonTransform.getPositionZ() - 1.0f)
				{
					isValidInteraction = true;

					PlayerInteractResultEvent playerInteractResult;
					playerInteractResult.interactionType = PlayerInteractionResult::pushCannon;
					postEvent(Event::create(playerInteractResult));
				}
			}
		}
	}

	if (!isValidInteraction && a_player->state != PlayerState::pushing && a_player->state != PlayerState::cleaning)
	{
		auto view = registry->view<CarryableItem, component::Transform>();
		for (auto& carryableItemEntity : view)
		{
			auto& carryableItem = registry->get<CarryableItem>(carryableItemEntity);
			auto& carryableItemTransform = registry->get<component::Transform>(carryableItemEntity);

			glm::vec3 itemNewPosition = glm::vec3(0.0f);
			glm::vec3 itemNewRotation = glm::vec3(0.0f);

			switch (carryableItem.type)
			{
				case CarryableItemType::mop:
				{
					if (!carryableItem.isBeingCarried
						//compare x values
						&& a_playerTransform->getPositionX() < carryableItemTransform.getPositionX() + 1.8f
						&& a_playerTransform->getPositionX() > carryableItemTransform.getPositionX() - 1.8f
						//compare z values
						&& a_playerTransform->getPositionZ() < carryableItemTransform.getPositionZ() + 1.8f
						&& a_playerTransform->getPositionZ() > carryableItemTransform.getPositionZ() - 1.8f)
					{
						if (a_player->carriedItem == CarryingItemState::nothing || a_player->carriedItem == CarryingItemState::cleaningSolution)
						{
							isValidInteraction = true;

							PlayerInteractResultEvent playerInteractResult;
							playerInteractResult.interactionType = PlayerInteractionResult::pickUpMop;
							postEvent(Event::create(playerInteractResult));
						}
					}

					break;
				}
				case CarryableItemType::cannonball:
				{
					if (!carryableItem.isBeingCarried
						//compare x values
						&& a_playerTransform->getPositionX() < carryableItemTransform.getPositionX() + 1.1f
						&& a_playerTransform->getPositionX() > carryableItemTransform.getPositionX() - 1.1f
						//compare z values
						&& a_playerTransform->getPositionZ() < carryableItemTransform.getPositionZ() + 1.1f
						&& a_playerTransform->getPositionZ() > carryableItemTransform.getPositionZ() - 1.1f)
					{
						if (a_player->carriedItem == CarryingItemState::nothing)
						{
							isValidInteraction = true;

							PlayerInteractResultEvent playerInteractResult;
							playerInteractResult.interactionType = PlayerInteractionResult::pickUpCannonball;
							postEvent(Event::create(playerInteractResult));
						}
					}
					break;
				}
			}

			if (isValidInteraction)
				break; //don't keep looping if we get a valid interaction
		}
	}

	if (!isValidInteraction)
	{
		auto view = registry->view<GarbagePile, component::Transform>();
		for (auto& garbagePileEntity : view)
		{
			auto& garbagePile = registry->get<GarbagePile>(garbagePileEntity);
			auto& garbagePileTransform = registry->get<component::Transform>(garbagePileEntity);

			if (a_player->carriedItem == CarryingItemState::mop
				//compare x values
				&& a_playerTransform->getPositionX() < garbagePileTransform.getPositionX() + 2.6f
				&& a_playerTransform->getPositionX() > garbagePileTransform.getPositionX() - 2.6f
				//compare z values
				&& a_playerTransform->getPositionZ() < garbagePileTransform.getPositionZ() + 2.6f
				&& a_playerTransform->getPositionZ() > garbagePileTransform.getPositionZ() - 2.6f)
			{
				//make sure there is some garbage before we try to clean
				if (garbagePile.garbageLevel > 0)
				{
					isValidInteraction = true;

					PlayerInteractResultEvent playerInteractResult;
					playerInteractResult.interactionType = PlayerInteractionResult::cleanGarbagePile;
					postEvent(Event::create(playerInteractResult));
				}
			}

			if (isValidInteraction)
				break; //don't keep looping if we get a valid interaction
		}
	}

	if (!isValidInteraction && a_player->state != PlayerState::pushing && a_player->state != PlayerState::cleaning)
	{
		auto crateView = registry->view<CannonballCrate, component::Transform>();
		for (auto& cannonballCrateEntity : crateView)
		{
			auto& cannonballCrate = registry->get<CannonballCrate>(cannonballCrateEntity);
			auto& cannonballCrateTransform = registry->get<component::Transform>(cannonballCrateEntity);

			if (a_player->carriedItem == CarryingItemState::nothing
				//compare x values
				&& a_playerTransform->getPositionX() < cannonballCrateTransform.getPositionX() + 2.2f
				&& a_playerTransform->getPositionX() > cannonballCrateTransform.getPositionX() - 2.2f
				//compare z values
				&& a_playerTransform->getPositionZ() < cannonballCrateTransform.getPositionZ() + 1.7f
				&& a_playerTransform->getPositionZ() > cannonballCrateTransform.getPositionZ() - 1.7f)
			{
				auto carryableItemView = registry->view<CarryableItem, component::Transform>();
				for (auto& carryableItemEntity : carryableItemView)
				{
					if (isValidInteraction)
						break;

					auto& carryableItem = registry->get<CarryableItem>(carryableItemEntity);
					auto& carryableItemTransform = registry->get<component::Transform>(carryableItemEntity);

					if (carryableItem.type == CarryableItemType::cannonball && carryableItem.isActive && !carryableItem.isBeingCarried)
					{
						isValidInteraction = true;

						PlayerInteractResultEvent playerInteractResult;
						playerInteractResult.interactionType = PlayerInteractionResult::takeCannonballFromCrate;
						postEvent(Event::create(playerInteractResult));
					}
				}
			}
		}
	}

	//no valid interactions
	if (!isValidInteraction)
	{
		PlayerInteractResultEvent playerInteractResult;
		playerInteractResult.interactionType = PlayerInteractionResult::nothing;
		postEvent(Event::create(playerInteractResult));
	}
}

void PlayerInteractionValidationSystem::validateInteraction(entt::entity a_playerEntity, Player* a_player, component::Transform* a_playerTransform)
{
	/////////////TODO: remove this once proper validation (raycast) is in
	auto view = registry->view<Cannon, component::Transform>();
	for (auto& entity : view)
	{
		a_player->interactableEntity = entity;
	}
    /////////////
    
    //check if there's a valid entity the player can interact with first
	if (a_player->interactableEntity == entt::null)
		return;

	bool isValidInteraction = false; //TODO: remove once raycast is in

    //check type of interactable entity
	if (registry->has<Cannon>(a_player->interactableEntity))
	{
		auto view = registry->view<Cannon, component::Transform>();
		for (auto& entity : view)
		{
			auto& cannon          = registry->get<Cannon>(entity);
			auto& cannonTransform = registry->get<component::Transform>(entity);

			a_player->interactableEntity = entity; //TODO: remove this once proper validation (raycast) is in

			if (a_player->interactableEntity == entity)
			{
				//cannon can be loaded with a cannonball
			    if (a_player->carriedItem == CarryingItemState::cannonball
					&& cannon.isLoaded == false
					//compare x values
					&& a_playerTransform->getPositionX() < cannonTransform.getPositionX() + 1.7f
					&& a_playerTransform->getPositionX() > cannonTransform.getPositionX() - 1.7f
					//compare z values
					&& a_playerTransform->getPositionZ() < cannonTransform.getPositionZ() + 1.7f
					&& a_playerTransform->getPositionZ() > cannonTransform.getPositionZ() - 1.7f) //check if cannon can be loaded with a cannonball
			    {
					isValidInteraction = true;
					cannon.isLoaded = true;

					//get rid of the cannonball the player has loaded
					auto carriedItemsView = registry->view<CarryableItem, component::Parent, component::Transform>();
					for (entt::entity carriedItemEntity : carriedItemsView)
					{
						auto& carriedItem = registry->get<CarryableItem>(carriedItemEntity);
						auto& carriedItemParent = registry->get<component::Parent>(carriedItemEntity);
						auto& carriedItemTransform = registry->get<component::Transform>(carriedItemEntity);

						if (carriedItemParent.parent == a_playerEntity)
						{
							std::cout << "LOADED CANNON!\n";

							carriedItemParent.parent = entt::null;
							a_player->carriedItem = CarryingItemState::nothing;
							carriedItem.isBeingCarried = false;
							carriedItemTransform.setPosition(glm::vec3(1000.0f, 1000.0f, 1000.0f));
							carriedItem.isActive = false;
						}
					}
			    }
				else if (cannon.state == CannonState::doingNothing && a_player->carriedItem == CarryingItemState::nothing) //cannon can be pushed TODO: get rid of conditions after raycast validation is in, should just be an else
				{
					float playerForwardDotCannonRight = glm::dot(a_playerTransform->getForward(), cannonTransform.getRight());

					bool isCannonOnLeftSideOfTrack = (cannon.cannonTrackPosition == -1)
						                              ? true : false;
					bool isCannonOnRightSideOfTrack = (cannon.cannonTrackPosition == 1)
						                              ? true : false;

					std::cout << "dot: " << playerForwardDotCannonRight << "\n";

				    //TODO: get rid of position comparison once raycasting is in
				    //check if player is on the right side of the cannon (will be pushing towards the left)
					if (playerForwardDotCannonRight < -0.65f && !isCannonOnLeftSideOfTrack 
						//compare x values
						&& a_playerTransform->getPositionX() < cannonTransform.getPositionX() + 2.5f
						&& a_playerTransform->getPositionX() > cannonTransform.getPositionX() + 1.1f
						//compare z values
						&& a_playerTransform->getPositionZ() < cannonTransform.getPositionZ() + 1.0f
						&& a_playerTransform->getPositionZ() > cannonTransform.getPositionZ() - 1.0f)
					{
						//set adjusting position and pushing state data before setting the new state
						a_player->adjustingPositionStateData.destinationPos   = cannonTransform.getRight() * glm::vec3(1.2f, 0.0f, 0.0f) + cannonTransform.getPosition();
						a_player->adjustingPositionStateData.destinationPos.y = a_playerTransform->getPositionY(); //don't change the player's y position
					    
						a_player->pushingStateData.destinationPos   = -cannonTransform.getRight() * glm::vec3(cannon.pushDistance, 0.0f, 0.0f) + a_player->adjustingPositionStateData.destinationPos;
						a_player->pushingStateData.destinationPos.y = a_playerTransform->getPositionY(); //don't change the player's y position
					    
						cannon.pushStateData.destinationPos = -cannonTransform.getRight() * glm::vec3(cannon.pushDistance, 0.0f, 0.0f) + cannonTransform.getPosition();

						cannon.cannonTrackPosition--;

						//hacky way to limit the camera but it works for now
						a_player->yRotationClamp = 50.0f;
						if (a_playerTransform->getRotationEulerY() < a_player->yRotationClamp)
							a_playerTransform->setRotationEulerY(a_player->yRotationClamp);

						isValidInteraction = true; //TODO: remove once raycast is in
					}
					//TODO: get rid of position comparison once raycasting is in
				    //check if player is on the left side of the cannon (will be pushing towards the right)
					else if (playerForwardDotCannonRight > 0.65f && !isCannonOnRightSideOfTrack
						//compare x values
						&& a_playerTransform->getPositionX() > cannonTransform.getPositionX() - 2.5f
						&& a_playerTransform->getPositionX() < cannonTransform.getPositionX() - 1.1f
						//compare z values
						&& a_playerTransform->getPositionZ() < cannonTransform.getPositionZ() + 1.0f
						&& a_playerTransform->getPositionZ() > cannonTransform.getPositionZ() - 1.0f)
					{
						//set adjusting position and pushing state data before setting the new state
						a_player->adjustingPositionStateData.destinationPos   = -cannonTransform.getRight() * glm::vec3(1.2f, 0.0f, 0.0f) + cannonTransform.getPosition();
						a_player->adjustingPositionStateData.destinationPos.y = a_playerTransform->getPositionY(); //don't change the player's y position
					    
						a_player->pushingStateData.destinationPos   = cannonTransform.getRight() * glm::vec3(cannon.pushDistance, 0.0f, 0.0f) + a_player->adjustingPositionStateData.destinationPos;
						a_player->pushingStateData.destinationPos.y = a_playerTransform->getPositionY(); //don't change the player's y position
					    
						cannon.pushStateData.destinationPos = cannonTransform.getRight() * glm::vec3(cannon.pushDistance, 0.0f, 0.0f) + cannonTransform.getPosition();

						cannon.cannonTrackPosition++;

						//hacky way to limit the camera but it works for now
						a_player->yRotationClamp = -50.0f;
						if (a_playerTransform->getRotationEulerY() > a_player->yRotationClamp)
							a_playerTransform->setRotationEulerY(a_player->yRotationClamp);

						isValidInteraction = true; //TODO: remove once raycast is in
					}

					if (isValidInteraction)
					{
						std::cout << "PUSHING!\n";

						a_player->adjustingPositionStateData.startPos = a_playerTransform->getPosition();
						a_player->pushingStateData.startPos = a_player->adjustingPositionStateData.destinationPos;

						cannon.pushStateData.startPos = cannonTransform.getPosition();

						//once all the new positions are set for the states, send out state change events
						PlayerStateChangeEvent playerStateChange;
						playerStateChange.player = a_player;
						playerStateChange.newState = PlayerState::pushing;
						postEvent(Event::create(playerStateChange));

						CannonStateChangeEvent cannonStateChange;
						cannonStateChange.cannon = &cannon;
						cannonStateChange.newState = CannonState::beingPushed;
						postEvent(Event::create(cannonStateChange));
					}
				}
			}
		}
	}

	if (!isValidInteraction && a_player->state != PlayerState::pushing && a_player->state != PlayerState::cleaning)
	{
		auto view = registry->view<CarryableItem, component::Transform>();
		for (auto& carryableItemEntity : view)
		{
			auto& carryableItem = registry->get<CarryableItem>(carryableItemEntity);
			auto& carryableItemTransform = registry->get<component::Transform>(carryableItemEntity);

			a_player->interactableEntity = carryableItemEntity; //TODO: remove this once proper validation (raycast) is in

			if (a_player->interactableEntity == carryableItemEntity)
			{
				glm::vec3 itemNewPosition = glm::vec3(0.0f);
				glm::vec3 itemNewRotation = glm::vec3(0.0f);

				switch (carryableItem.type)
				{
					case CarryableItemType::mop:
					{
						if (!carryableItem.isBeingCarried
							//compare x values
							&& a_playerTransform->getPositionX() < carryableItemTransform.getPositionX() + 1.8f
							&& a_playerTransform->getPositionX() > carryableItemTransform.getPositionX() - 1.8f
							//compare z values
							&& a_playerTransform->getPositionZ() < carryableItemTransform.getPositionZ() + 1.8f
							&& a_playerTransform->getPositionZ() > carryableItemTransform.getPositionZ() - 1.8f)
						{
							if (a_player->carriedItem == CarryingItemState::nothing)
							{
								a_player->carriedItem = CarryingItemState::mop;
								isValidInteraction = true;
								std::cout << "PICKED UP MOP!\n";

								itemNewPosition = glm::vec3(0.45f, -0.15f, -1.4f);
								itemNewRotation = glm::vec3(-107.0f, -69.5f, 106.0f);
							}
							else if (a_player->carriedItem == CarryingItemState::cleaningSolution)
							{
								a_player->carriedItem = CarryingItemState::mopAndCleaningSolution;
								isValidInteraction = true;
								std::cout << "PICKED UP MOP!\n";

								itemNewPosition = glm::vec3(0.45f, -0.15f, -1.4f);
								itemNewRotation = glm::vec3(-107.0f, -69.5f, 106.0f);
							}
						}

						break;
					}
					case CarryableItemType::cannonball:
					{
						if (!carryableItem.isBeingCarried
							//compare x values
							&& a_playerTransform->getPositionX() < carryableItemTransform.getPositionX() + 1.1f
							&& a_playerTransform->getPositionX() > carryableItemTransform.getPositionX() - 1.1f
							//compare z values
							&& a_playerTransform->getPositionZ() < carryableItemTransform.getPositionZ() + 1.1f
							&& a_playerTransform->getPositionZ() > carryableItemTransform.getPositionZ() - 1.1f)
						{
							if (a_player->carriedItem == CarryingItemState::nothing)
							{
								a_player->carriedItem = CarryingItemState::cannonball;
								isValidInteraction = true;
								std::cout << "PICKED UP CANNONBALL!\n";

								itemNewPosition = glm::vec3(0.0f, 0.35f, -0.75f);
								itemNewRotation = glm::vec3(0.0f, 0.0f, 0.0f);
							}
						}
						break;
					}

				}

				if (isValidInteraction)
				{
					carryableItem.isBeingCarried = true;
					auto& mopParent = registry->get_or_assign<component::Parent>(carryableItemEntity);
					mopParent.parent = a_playerEntity;

					carryableItemTransform.setRotationEuler(itemNewRotation);
					carryableItemTransform.setPosition(itemNewPosition);
				}
			}

			if (isValidInteraction)
				break; //don't keep looping if we get a valid interaction
		}
	}

	if (!isValidInteraction)
	{
		auto view = registry->view<GarbagePile, component::Transform>();
		for (auto& garbagePileEntity : view)
		{
			auto& garbagePile = registry->get<GarbagePile>(garbagePileEntity);
			auto& garbagePileTransform = registry->get<component::Transform>(garbagePileEntity);

			a_player->interactableEntity = garbagePileEntity; //TODO: remove this once proper validation (raycast) is in

			if (a_player->interactableEntity == garbagePileEntity)
			{
				if (a_player->carriedItem == CarryingItemState::mop
					//compare x values
					&& a_playerTransform->getPositionX() < garbagePileTransform.getPositionX() + 2.6f
					&& a_playerTransform->getPositionX() > garbagePileTransform.getPositionX() - 2.6f
					//compare z values
					&& a_playerTransform->getPositionZ() < garbagePileTransform.getPositionZ() + 2.6f
					&& a_playerTransform->getPositionZ() > garbagePileTransform.getPositionZ() - 2.6f)
				{
					//make sure there is some garbage before we try to clean
					if (garbagePile.garbageLevel > 0)
					{
						isValidInteraction = true;

						std::cout << "CLEANING!\n";

						garbagePile.garbageLevelTicks--;
						if (garbagePile.garbageLevelTicks <= 0)
						{
							garbagePile.garbageLevel--;
							garbagePile.garbageLevelTicks = garbagePile.MAX_GARBAGE_LEVEL_TICKS;
						}
					}
				}
			}
		}
	}

	if (!isValidInteraction && a_player->state != PlayerState::pushing && a_player->state != PlayerState::cleaning)
	{
		auto crateView = registry->view<CannonballCrate, component::Transform>();
		for (auto& cannonballCrateEntity : crateView)
		{
			auto& cannonballCrate = registry->get<CannonballCrate>(cannonballCrateEntity);
			auto& cannonballCrateTransform = registry->get<component::Transform>(cannonballCrateEntity);

			a_player->interactableEntity = cannonballCrateEntity; //TODO: remove this once proper validation (raycast) is in

			if (a_player->interactableEntity == cannonballCrateEntity)
			{
				if (a_player->carriedItem == CarryingItemState::nothing
					//compare x values
					&& a_playerTransform->getPositionX() < cannonballCrateTransform.getPositionX() + 2.2f
					&& a_playerTransform->getPositionX() > cannonballCrateTransform.getPositionX() - 2.2f
					//compare z values
					&& a_playerTransform->getPositionZ() < cannonballCrateTransform.getPositionZ() + 1.7f
					&& a_playerTransform->getPositionZ() > cannonballCrateTransform.getPositionZ() - 1.7f)
				{
					auto carryableItemView = registry->view<CarryableItem, component::Transform>();
					for (auto& carryableItemEntity : carryableItemView)
					{
						if (isValidInteraction)
							break;

						auto& carryableItem = registry->get<CarryableItem>(carryableItemEntity);
						auto& carryableItemTransform = registry->get<component::Transform>(carryableItemEntity);

						if (carryableItem.type == CarryableItemType::cannonball && carryableItem.isActive && !carryableItem.isBeingCarried)
						{
							isValidInteraction = true;
							a_player->carriedItem = CarryingItemState::cannonball;
							carryableItem.isBeingCarried = true;
							
							std::cout << "OBTAINED CANNONBALL!\n";

							auto& cannonballParent = registry->get_or_assign<component::Parent>(carryableItemEntity);
							cannonballParent.parent = a_playerEntity;

							carryableItemTransform.setRotationEuler(glm::vec3(0.0f));
							carryableItemTransform.setPosition(glm::vec3(0.0f, 0.35f, -0.75f));
						}
					}
				}
			}
		}
	}
}

CannonInteractionOutcome PlayerInteractionValidationSystem::validateRaycastHit(Player* a_player, Cannon* a_cannon)
{
	return CannonInteractionOutcome::invalid;
}