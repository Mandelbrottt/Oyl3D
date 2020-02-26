#include "PlayerSystem.h"

void PlayerSystem::onEnter()
{
	listenForEventCategory((EventCategory)CategoryPlayer);
	listenForEventType(EventType::PhysicsCollisionStay);
}

void PlayerSystem::onExit()
{
    
}

void PlayerSystem::onUpdate()
{
	auto view = registry->view<Player, component::Transform, component::RigidBody>();
	for (auto& playerEntity : view)
	{
		auto& player          = registry->get<Player>(playerEntity);
		auto& playerTransform = registry->get<component::Transform>(playerEntity);
		auto& playerRB        = registry->get<component::RigidBody>(playerEntity);

		player.jumpCooldownTimer -= Time::deltaTime();

		switch (player.state)
		{
		    case PlayerState::idle:
		    {
				performBasicMovement(playerEntity, player.speedForce);

				if (player.moveDirection != glm::vec3(0.0f))
					changeToWalking(&player);
		        
				break;
		    }
		    
		    case PlayerState::walking:
			{
				performBasicMovement(playerEntity, player.speedForce);

				if (player.moveDirection == glm::vec3(0.0f))
					changeToIdle(&player);

				break;
			}

			case PlayerState::jumping:
			{
				performBasicMovement(playerEntity, player.speedForce);

				if (playerRB.getVelocity().y < 0.0f)
					changeToFalling(&player);
				else if (!player.isJumping)
					changeToIdle(&player);

				break;
			}

			case PlayerState::falling:
			{
				changeToIdle(&player);

				break;
			}

		    case PlayerState::pushing:
		    {
		        //adjust the players position before actually starting to push (this is always the first "phase" of the pushing state)
				if (player.adjustingPositionStateData.interpolationParam < 1.0f)
				{
				    //LERP to the adjusting position destination (the starting point for pushing)
					player.adjustingPositionStateData.interpolationParam = std::min(
						player.adjustingPositionStateData.interpolationParam + player.adjustingPositionStateData.speed * Time::deltaTime(),
						1.0f);

					playerTransform.setPosition(glm::mix(
						player.adjustingPositionStateData.startPos,
						player.adjustingPositionStateData.destinationPos,
						player.adjustingPositionStateData.interpolationParam));
				}
				else //adjustingposition interpolation parameter >= 1.0f AKA player is done adjusting their position
				{
				    //the player is pushing
					player.pushingStateData.interpolationParam = std::min(
						player.pushingStateData.interpolationParam + player.pushingStateData.speed * Time::deltaTime(),
						1.0f);

					playerTransform.setPosition(glm::mix(
						player.pushingStateData.startPos,
						player.pushingStateData.destinationPos,
						player.pushingStateData.interpolationParam));

					if (player.pushingStateData.interpolationParam >= 1.0f)
					{
						changeToIdle(&player);
						player.yRotationClamp = 0.0f; //reset the camera rotation limit
					}
				}

				break;
		    }

			case PlayerState::inCleaningQuicktimeEvent:
			{
				performBasicMovement(playerEntity, player.speedForce);

				break;
			}

			case PlayerState::cleaning:
			{
				performBasicMovement(playerEntity, player.speedForce * 0.3f);

				player.cleaningTimeCountdown -= Time::deltaTime();
				if (player.cleaningTimeCountdown < 0.0f)
					changeToIdle(&player);

				break;
			}

			case PlayerState::throwingBottle:
			{
				performBasicMovement(playerEntity, player.speedForce * 0.5f);

				player.delayBeforeThrowingCountdown -= Time::deltaTime();
				//if (player.delayBeforeThrowingCountdown < 0.0f)
				{
					changeToIdle(&player);

					ThrowBottleEvent throwBottle;
					throwBottle.bottleEntity         = player.primaryCarriedItem;
					throwBottle.playerThrowingEntity = playerEntity;
					postEvent(throwBottle);

					player.primaryCarriedItem = entt::null;
				}

				break;
			}

			case PlayerState::stunned:
			{
				//still apply movement (with no direction) to fake friction for the player
				player.moveDirection = glm::vec3(0.0f, 0.0f, 0.0f);
				performBasicMovement(playerEntity, player.speedForce * 0.7f);

				player.stunnedTimeCountdown -= Time::deltaTime();
				if (player.stunnedTimeCountdown < 0.0f)
					changeToIdle(&player);

				break;
			}
		}
	}
}

bool PlayerSystem::onEvent(const Event& event)
{
	switch (event.type)
	{
		case (EventType)TypePlayerJump:
		{
			auto evt = event_cast<PlayerJumpEvent>(event);

			auto& player   = registry->get<Player>(evt.playerEntity);
			auto& playerRB = registry->get<component::RigidBody>(evt.playerEntity);

			if (   !player.isJumping 
				//&& playerRB.getVelocity().y >= -2.0f //add a small buffer 
				&& player.state != PlayerState::stunned 
				&& player.state != PlayerState::pushing 
				&& player.state != PlayerState::cleaning)
			{
				playerRB.addImpulse(glm::vec3(0.0f, 1.0f, 0.0f) * player.jumpForce);

				player.isJumping         = true;
				player.jumpCooldownTimer = player.JUMP_COOLDOWN_DURATION;

				if (player.state != PlayerState::throwingBottle)
					changeToJumping(&player);
			}

			break;
		}

		case (EventType) TypePlayerStateChange:
		{
			auto evt = event_cast<PlayerStateChangeEvent>(event);
			auto& player = registry->get<Player>(evt.playerEntity);

			if (player.state == PlayerState::pushing || player.state == PlayerState::stunned || player.state == PlayerState::throwingBottle)
				break;

			switch (evt.newState)
			{
				if (player.isCameraLocked)
					player.isCameraLocked = false;

				case PlayerState::idle:
			    {
					changeToIdle(&player);
				    break;
			    }
			    case PlayerState::pushing:
			    {
					changeToPushing(&player);
				    break;
			    }
				case PlayerState::inCleaningQuicktimeEvent:
				{
					changeToInCleaningQuicktimeEvent(&player);
					break;
				}
				case PlayerState::cleaning:
				{
					changeToCleaning(&player);
					break;
				}
				case PlayerState::throwingBottle:
				{
					changeToThrowingBottle(&player);
					break;
				}
				case PlayerState::stunned:
				{
					changeToStunned(&player);
					break;
				}
			}

			break;
		}

		case EventType::PhysicsCollisionStay:
		{
			auto evt = event_cast<PhysicsCollisionStayEvent>(event);

			entt::entity playerEntity = entt::null;

			//ensure the entities in the collision are valid
			if (!registry->valid(evt.entity1) || !registry->valid(evt.entity2))
				break;
			//check if there is no player involved in the collision
			if (!registry->has<Player>(evt.entity1) && !registry->has<Player>(evt.entity2))
				break;
			//check if they are both players
			else if (registry->has<Player>(evt.entity1) && registry->has<Player>(evt.entity2))
			{
				auto& player1Transform = registry->get<component::Transform>(evt.entity1);
				auto& player2Transform = registry->get<component::Transform>(evt.entity2);

				//figure out which player is above the other
				if (player1Transform.getPositionY() > player2Transform.getPositionY())
					playerEntity = evt.entity1;
				else
					playerEntity = evt.entity2;
			}
			else if (registry->has<Player>(evt.entity1))
				playerEntity = evt.entity1;
			else //entity 2 is a player
				playerEntity = evt.entity2;

			auto& player           = registry->get<Player>(playerEntity);
			auto& playerTransform  = registry->get<component::Transform>(playerEntity);
			auto& playerCollidable = registry->get<component::Collidable>(playerEntity);
			float halfPlayerHeight = playerCollidable.getShape(0).capsule.getHeight();

			//check if the player is valid to jump
			if (evt.contactPoint.y <= (playerTransform.getPositionY() - halfPlayerHeight + 0.15f) && player.jumpCooldownTimer < 0.0f)
			{
				player.isJumping = false;
			}
		}
	}
	return false;
}

void PlayerSystem::changeToIdle(Player* a_player)
{
	a_player->state = PlayerState::idle;
}

void PlayerSystem::changeToWalking(Player* a_player)
{
    if (a_player->state == PlayerState::idle)
	    a_player->state = PlayerState::walking;
}

void PlayerSystem::changeToJumping(Player* a_player)
{
	a_player->state = PlayerState::jumping;
}

void PlayerSystem::changeToFalling(Player* a_player)
{
	a_player->state = PlayerState::falling;
}

//destination and start positions for adjusting and pushing should be set before calling this function
void PlayerSystem::changeToPushing(Player* a_player)
{    
	a_player->adjustingPositionStateData.interpolationParam = 0.0f;
	a_player->adjustingPositionStateData.speed              = a_player->adjustingPositionSpeed;
    
	a_player->pushingStateData.interpolationParam = 0.0f;
	a_player->pushingStateData.speed              = a_player->pushingSpeed;
    
	a_player->state = PlayerState::pushing;
}

void PlayerSystem::changeToInCleaningQuicktimeEvent(Player* a_player)
{
	a_player->state          = PlayerState::inCleaningQuicktimeEvent;
	a_player->isCameraLocked = true;

	ActivateQuicktimeCleaningEventEvent activateQTE;
	activateQTE.playerNum = a_player->playerNum;
	postEvent(activateQTE);
}

void PlayerSystem::changeToCleaning(Player* a_player)
{
	a_player->cleaningTimeCountdown = a_player->CLEANING_TIME_DURATION;

	a_player->state = PlayerState::cleaning;
}

void PlayerSystem::changeToThrowingBottle(Player* a_player)
{
	a_player->delayBeforeThrowingCountdown = a_player->THROWING_DELAY_DURATION;

	a_player->state = PlayerState::throwingBottle;
}

void PlayerSystem::changeToStunned(Player* a_player)
{
	if (a_player->state == PlayerState::inCleaningQuicktimeEvent)
	{
		//deactivate CleaningQuicktimeEvent UI for player
		CancelQuicktimeCleaningEventEvent cancelQuicktimeCleaningEvent;
		cancelQuicktimeCleaningEvent.playerNum = a_player->playerNum;
		postEvent(cancelQuicktimeCleaningEvent);
	}

	a_player->stunnedTimeCountdown = a_player->STUNNED_TIME_DURATION;

	a_player->state = PlayerState::stunned;
}



void PlayerSystem::performBasicMovement(entt::entity a_playerEntity, const float a_speedForce)
{
	auto& player   = registry->get<Player>(a_playerEntity);
	auto& playerRB = registry->get<component::RigidBody>(a_playerEntity);

	glm::vec3 deltaVelocity = (player.moveDirection * a_speedForce) - playerRB.getVelocity();
	deltaVelocity.y = 0;
	playerRB.addImpulse(deltaVelocity * 0.98f);
}