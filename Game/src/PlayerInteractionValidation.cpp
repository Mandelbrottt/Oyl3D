#include "PlayerInteractionValidation.h"

void PlayerInteractionValidationSystem::onEnter()
{
	this->addToCategoryMask(CategoryPlayer);
	this->addToCategoryMask(CategoryCannon);
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
			    if (evt.player == playerEntity)
			    {
					auto& player          = registry->get<Player>(playerEntity);
					auto& playerTransform = registry->get<component::Transform>(playerEntity);
			        
					validateInteraction(&player, playerTransform);
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
			    if (a_player->carriedItem == CarryableItemType::cannonball) //check if cannon can be loaded with a cannonball
			    {

			    }
				else if (cannon.state == CannonState::doingNothing) //cannon can be pushed TODO: get rid of condition after raycast validation is in, should just be an else
				{
					float playerForwardDotCannonRight = glm::dot(a_playerTransform.getForward(), cannonTransform.getRight());

					bool isCannonOnLeftSideOfTrack = (-cannonTransform.getRight().x * cannon.pushDistance + cannon.initialPosition.x == cannonTransform.getPosition().x)
						                              ? true : false;
					bool isCannonOnRightSideOfTrack = (cannonTransform.getRight().x * cannon.pushDistance + cannon.initialPosition.x == cannonTransform.getPosition().x)
						                              ? true : false;

				    //TODO: get rid of position comparison once raycasting is in
				    //check if player is on the right side of the cannon (will be pushing towards the left)
					if (playerForwardDotCannonRight < -0.3f && !isCannonOnLeftSideOfTrack) //&& a_playerTransform.getPositionX() > cannonTransform.getPositionX())
					{
						//set adjusting position and pushing state data before setting the new state
						a_player->adjustingPositionStateData.destinationPos   = cannonTransform.getRight()  * glm::vec3(1.0f, 0.0f, 0.0f) + cannonTransform.getPosition();
						a_player->adjustingPositionStateData.destinationPos.y = a_playerTransform.getPositionY(); //don't change the player's y position
					    
						a_player->pushingStateData.destinationPos   = -cannonTransform.getRight() * glm::vec3(cannon.pushDistance, 0.0f, 0.0f) + a_player->adjustingPositionStateData.destinationPos;
						a_player->pushingStateData.destinationPos.y = a_playerTransform.getPositionY(); //don't change the player's y position
					    
						cannon.pushStateData.destinationPos = -cannonTransform.getRight() * glm::vec3(cannon.pushDistance, 0.0f, 0.0f) + cannonTransform.getPosition();
					}
					//TODO: get rid of position comparison once raycasting is in
				    //check if player is on the left side of the cannon (will be pushing towards the right
					else if (playerForwardDotCannonRight > 0.3f && !isCannonOnRightSideOfTrack) //&& a_playerTransform.getPositionX() < cannonTransform.getPositionX())
					{
						//set adjusting position and pushing state data before setting the new state
						a_player->adjustingPositionStateData.destinationPos = -cannonTransform.getRight() * glm::vec3(1.0f, 0.0f, 0.0f) + cannonTransform.getPosition();
						a_player->adjustingPositionStateData.destinationPos.y = a_playerTransform.getPositionY(); //don't change the player's y position
					    
						a_player->pushingStateData.destinationPos   = cannonTransform.getRight()  * glm::vec3(cannon.pushDistance, 0.0f, 0.0f) + a_player->adjustingPositionStateData.destinationPos;
						a_player->pushingStateData.destinationPos.y = a_playerTransform.getPositionY(); //don't change the player's y position
					    
						cannon.pushStateData.destinationPos = cannonTransform.getRight() * glm::vec3(cannon.pushDistance, 0.0f, 0.0f) + cannonTransform.getPosition();
					}
					else
						return; //no valid push
				    
					a_player->adjustingPositionStateData.startPos = a_playerTransform.getPosition();
					a_player->pushingStateData.startPos           = a_player->adjustingPositionStateData.destinationPos;

					cannon.pushStateData.startPos = cannonTransform.getPosition();

				    //once all the new positions are set for the states, send out state change events
					PlayerStateChangeEvent playerStateChange;
					playerStateChange.player   = a_player;
					playerStateChange.newState = PlayerState::pushing;
					postEvent(Event::create(playerStateChange));

					CannonStateChangeEvent cannonStateChange;
					cannonStateChange.cannon   = &cannon;
					cannonStateChange.newState = CannonState::beingPushed;
					postEvent(Event::create(cannonStateChange));
				}
			}
		}
	}
}

CannonInteractionOutcome PlayerInteractionValidationSystem::validateRaycastHit(Player* a_player, Cannon* a_cannon)
{
	return CannonInteractionOutcome::invalid;
}