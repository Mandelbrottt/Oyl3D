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
			        
					validateInteraction(&player, playerTransform);
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
						auto& carriedItemParent = registry->get<component::Parent>(carriedItemEntity);
						if (carriedItemParent.parent == playerEntity)
						{
							std::cout << "DROPPED ITEMS!\n";

							carriedItemParent.parent = entt::null;
							player.carriedItem = CarryingItemState::nothing;
							registry->get<CarryableItem>(carriedItemEntity).isBeingCarried = false;

							auto& carriedItemTransform = registry->get<component::Transform>(carriedItemEntity);
							carriedItemTransform.setPosition(playerTransform.getPosition() + (playerTransform.getForward() * 1.5f));
							carriedItemTransform.setPositionY(0.1f);
							carriedItemTransform.setRotationEuler(playerTransform.getRotationEuler() + glm::vec3(-107.0f, 0.0f, 106.0f));
						}
					}
				}
			}

			break;
		}
	}

	return false;
}

void PlayerInteractionValidationSystem::validateInteraction(Player* a_player, const component::Transform& a_playerTransform)
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
			    if (a_player->carriedItem == CarryingItemState::cannonball) //check if cannon can be loaded with a cannonball
			    {

			    }
				else if (cannon.state == CannonState::doingNothing && a_player->carriedItem == CarryingItemState::nothing) //cannon can be pushed TODO: get rid of conditions after raycast validation is in, should just be an else
				{
					float playerForwardDotCannonRight = glm::dot(a_playerTransform.getForward(), cannonTransform.getRight());

					bool isCannonOnLeftSideOfTrack = (cannon.cannonTrackPosition == -1)
						                              ? true : false;
					bool isCannonOnRightSideOfTrack = (cannon.cannonTrackPosition == 1)
						                              ? true : false;

				    //TODO: get rid of position comparison once raycasting is in
				    //check if player is on the right side of the cannon (will be pushing towards the left)
					if (playerForwardDotCannonRight < -0.3f && !isCannonOnLeftSideOfTrack 
						//compare x values
						&& a_playerTransform.getPositionX() < cannonTransform.getPositionX() + 2.5f
						&& a_playerTransform.getPositionX() > cannonTransform.getPositionX() + 1.1f
						//compare z values
						&& a_playerTransform.getPositionZ() < cannonTransform.getPositionZ() + 1.0f
						&& a_playerTransform.getPositionZ() > cannonTransform.getPositionZ() - 1.0f)
					{
						//set adjusting position and pushing state data before setting the new state
						a_player->adjustingPositionStateData.destinationPos   = cannonTransform.getRight() * glm::vec3(1.2f, 0.0f, 0.0f) + cannonTransform.getPosition();
						a_player->adjustingPositionStateData.destinationPos.y = a_playerTransform.getPositionY(); //don't change the player's y position
					    
						a_player->pushingStateData.destinationPos   = -cannonTransform.getRight() * glm::vec3(cannon.pushDistance, 0.0f, 0.0f) + a_player->adjustingPositionStateData.destinationPos;
						a_player->pushingStateData.destinationPos.y = a_playerTransform.getPositionY(); //don't change the player's y position
					    
						cannon.pushStateData.destinationPos = -cannonTransform.getRight() * glm::vec3(cannon.pushDistance, 0.0f, 0.0f) + cannonTransform.getPosition();

						cannon.cannonTrackPosition--;

						isValidInteraction = true; //TODO: remove once raycast is in
					}
					//TODO: get rid of position comparison once raycasting is in
				    //check if player is on the left side of the cannon (will be pushing towards the right
					else if (playerForwardDotCannonRight > 0.3f && !isCannonOnRightSideOfTrack
						//compare x values
						&& a_playerTransform.getPositionX() > cannonTransform.getPositionX() - 2.5f
						&& a_playerTransform.getPositionX() < cannonTransform.getPositionX() - 1.1f
						//compare z values
						&& a_playerTransform.getPositionZ() < cannonTransform.getPositionZ() + 1.0f
						&& a_playerTransform.getPositionZ() > cannonTransform.getPositionZ() - 1.0f)
					{
						//set adjusting position and pushing state data before setting the new state
						a_player->adjustingPositionStateData.destinationPos   = -cannonTransform.getRight() * glm::vec3(1.2f, 0.0f, 0.0f) + cannonTransform.getPosition();
						a_player->adjustingPositionStateData.destinationPos.y = a_playerTransform.getPositionY(); //don't change the player's y position
					    
						a_player->pushingStateData.destinationPos   = cannonTransform.getRight() * glm::vec3(cannon.pushDistance, 0.0f, 0.0f) + a_player->adjustingPositionStateData.destinationPos;
						a_player->pushingStateData.destinationPos.y = a_playerTransform.getPositionY(); //don't change the player's y position
					    
						cannon.pushStateData.destinationPos = cannonTransform.getRight() * glm::vec3(cannon.pushDistance, 0.0f, 0.0f) + cannonTransform.getPosition();

						cannon.cannonTrackPosition++;

						isValidInteraction = true; //TODO: remove once raycast is in
					}

					if (isValidInteraction)
					{
						std::cout << "PUSHING!";

						a_player->adjustingPositionStateData.startPos = a_playerTransform.getPosition();
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
				switch (carryableItem.type)
				{
					case CarryableItemType::mop:
					{
						if (!carryableItem.isBeingCarried
							//compare x values
							&& a_playerTransform.getPositionX() < carryableItemTransform.getPositionX() + 1.8f
							&& a_playerTransform.getPositionX() > carryableItemTransform.getPositionX() - 1.8f
							//compare z values
							&& a_playerTransform.getPositionZ() < carryableItemTransform.getPositionZ() + 1.8f
							&& a_playerTransform.getPositionZ() > carryableItemTransform.getPositionZ() - 1.8f)
						{
							if (a_player->carriedItem == CarryingItemState::nothing)
							{
								a_player->carriedItem = CarryingItemState::mop;
								isValidInteraction = true;
							}
							else if (a_player->carriedItem == CarryingItemState::cleaningSolution)
							{
								a_player->carriedItem = CarryingItemState::mopAndCleaningSolution;
								isValidInteraction = true;
							}
						}

						if (isValidInteraction)
						{
							carryableItem.isBeingCarried = true;
							std::cout << "PICKED UP MOP!\n";

							auto view = registry->view<Player>();
							for (auto& playerEntity : view)
							{
								if (a_player == &registry->get<Player>(playerEntity))
								{
									auto& mopParent = registry->get_or_assign<component::Parent>(carryableItemEntity);
									mopParent.parent = playerEntity;

									carryableItemTransform.setRotationEuler(glm::vec3(-107.0f, -69.5f, 106.0f));
									carryableItemTransform.setPosition(glm::vec3(0.45f, -0.15f, -1.4f));
								}
							}
						}

						break;
					}
				}
			}
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
					&& a_playerTransform.getPositionX() < garbagePileTransform.getPositionX() + 2.3f
					&& a_playerTransform.getPositionX() > garbagePileTransform.getPositionX() - 2.3f
					//compare z values
					&& a_playerTransform.getPositionZ() < garbagePileTransform.getPositionZ() + 2.3f
					&& a_playerTransform.getPositionZ() > garbagePileTransform.getPositionZ() - 2.3f)
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
}

CannonInteractionOutcome PlayerInteractionValidationSystem::validateRaycastHit(Player* a_player, Cannon* a_cannon)
{
	return CannonInteractionOutcome::invalid;
}