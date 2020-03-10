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

		playerRB.addImpulse(glm::vec3(0.0f, -35.0f, 0.0f) * Time::deltaTime());

		if (player.transitionIntoQTE && player.state != PlayerState::cleaning)
			player.transitionIntoQTE = false;

		switch (player.state)
		{
		    case PlayerState::idle:
		    {
				performBasicMovement(playerEntity, player.speedForce);

				if (player.moveDirection != glm::vec3(0.0f))
					changeToWalking(playerEntity);
		        
				break;
		    }
		    
		    case PlayerState::walking:
			{
				performBasicMovement(playerEntity, player.speedForce);

				if (player.moveDirection == glm::vec3(0.0f))
					changeToIdle(playerEntity);

				break;
			}

			case PlayerState::jumping:
			{
				performBasicMovement(playerEntity, player.speedForce);

				if (playerRB.getVelocity().y < 0.0f)
					changeToFalling(playerEntity);
				else if (!player.isJumping)
					changeToIdle(playerEntity);

				break;
			}

			case PlayerState::falling:
			{
				performBasicMovement(playerEntity, player.speedForce);

				if (playerRB.getVelocity().y >= 0.0f)
					changeToIdle(playerEntity);

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
						changeToIdle(playerEntity);
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
					changeToIdle(playerEntity);

				break;
			}

			case PlayerState::throwingBottle:
			{
				performBasicMovement(playerEntity, player.speedForce * 0.5f);

				player.delayBeforeThrowingCountdown -= Time::deltaTime();
				//if (player.delayBeforeThrowingCountdown < 0.0f)
				{
					changeToIdle(playerEntity);

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
				performBasicMovement(playerEntity, player.speedForce);

				player.stunnedTimeCountdown -= Time::deltaTime();
				if (player.stunnedTimeCountdown < 0.0f)
					changeToIdle(playerEntity);

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
					changeToJumping(evt.playerEntity);
			}

			break;
		}

		case (EventType) TypePlayerStateChangeRequest:
		{
			auto evt = event_cast<PlayerStateChangeRequestEvent>(event);
			auto& player = registry->get<Player>(evt.playerEntity);

			if (player.state == PlayerState::pushing || player.state == PlayerState::stunned || player.state == PlayerState::throwingBottle)
				break;

			switch (evt.newState)
			{
				if (player.isCameraLocked)
					player.isCameraLocked = false;

				case PlayerState::idle:
			    {
					changeToIdle(evt.playerEntity);
				    break;
			    }
			    case PlayerState::pushing:
			    {
					changeToPushing(evt.playerEntity);
				    break;
			    }
				case PlayerState::inCleaningQuicktimeEvent:
				{
					changeToInCleaningQuicktimeEvent(evt.playerEntity);
					break;
				}
				case PlayerState::cleaning:
				{
					changeToCleaning(evt.playerEntity);
					break;
				}
				case PlayerState::throwingBottle:
				{
					changeToThrowingBottle(evt.playerEntity);
					break;
				}
				case PlayerState::stunned:
				{
					changeToStunned(evt.playerEntity);
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

				if (player.state == PlayerState::falling)
					changeToIdle(playerEntity);
			}
		}
	}
	return false;
}

void PlayerSystem::changeToIdle(entt::entity a_playerEntity)
{
	auto& player = registry->get<Player>(a_playerEntity);

	player.state = PlayerState::idle;

	PlayerStateChangedEvent stateChanged;
	stateChanged.playerEntity = a_playerEntity;
	stateChanged.newState     = PlayerState::idle;
	postEvent(stateChanged);
}

void PlayerSystem::changeToWalking(entt::entity a_playerEntity)
{
	auto& player = registry->get<Player>(a_playerEntity);
	
	if (player.state == PlayerState::idle)
	{
		player.state = PlayerState::walking;

		PlayerStateChangedEvent stateChanged;
		stateChanged.playerEntity = a_playerEntity;
		stateChanged.newState     = PlayerState::walking;
		postEvent(stateChanged);
	}
}

void PlayerSystem::changeToJumping(entt::entity a_playerEntity)
{
	auto& player = registry->get<Player>(a_playerEntity);

	player.state = PlayerState::jumping;

	PlayerStateChangedEvent stateChanged;
	stateChanged.playerEntity = a_playerEntity;
	stateChanged.newState     = PlayerState::jumping;
	postEvent(stateChanged);
}

void PlayerSystem::changeToFalling(entt::entity a_playerEntity)
{
	auto& player = registry->get<Player>(a_playerEntity);

	player.state = PlayerState::falling;

	PlayerStateChangedEvent stateChanged;
	stateChanged.playerEntity = a_playerEntity;
	stateChanged.newState     = PlayerState::falling;
	postEvent(stateChanged);
}

//destination and start positions for adjusting and pushing should be set before calling this function
void PlayerSystem::changeToPushing(entt::entity a_playerEntity)
{    
	auto& player = registry->get<Player>(a_playerEntity);

	player.adjustingPositionStateData.interpolationParam = 0.0f;
	player.adjustingPositionStateData.speed              = player.adjustingPositionSpeed;
    
	player.pushingStateData.interpolationParam = 0.0f;
	player.pushingStateData.speed              = player.pushingSpeed;
    
	player.state = PlayerState::pushing;

	PlayerStateChangedEvent stateChanged;
	stateChanged.playerEntity = a_playerEntity;
	stateChanged.newState     = PlayerState::pushing;
	postEvent(stateChanged);
}

void PlayerSystem::changeToInCleaningQuicktimeEvent(entt::entity a_playerEntity)
{
	auto& player = registry->get<Player>(a_playerEntity);

	player.state          = PlayerState::inCleaningQuicktimeEvent;
	player.isCameraLocked = true;

	ActivateQuicktimeCleaningEventEvent activateQTE;
	activateQTE.playerNum = player.playerNum;
	postEvent(activateQTE);

	PlayerStateChangedEvent stateChanged;
	stateChanged.playerEntity = a_playerEntity;
	stateChanged.newState     = PlayerState::inCleaningQuicktimeEvent;
	postEvent(stateChanged);
}

void PlayerSystem::changeToCleaning(entt::entity a_playerEntity)
{
	auto& player = registry->get<Player>(a_playerEntity);

	player.cleaningTimeCountdown = player.CLEANING_TIME_DURATION;

	player.state = PlayerState::cleaning;

	PlayerStateChangedEvent stateChanged;
	stateChanged.playerEntity = a_playerEntity;
	stateChanged.newState     = PlayerState::cleaning;
	postEvent(stateChanged);
}

void PlayerSystem::changeToThrowingBottle(entt::entity a_playerEntity)
{
	auto& player = registry->get<Player>(a_playerEntity);

	player.delayBeforeThrowingCountdown = player.THROWING_DELAY_DURATION;

	player.state = PlayerState::throwingBottle;

	PlayerStateChangedEvent stateChanged;
	stateChanged.playerEntity = a_playerEntity;
	stateChanged.newState     = PlayerState::throwingBottle;
	postEvent(stateChanged);
}

void PlayerSystem::changeToStunned(entt::entity a_playerEntity)
{
	auto& player = registry->get<Player>(a_playerEntity);

	if (player.state == PlayerState::inCleaningQuicktimeEvent)
	{
		//deactivate CleaningQuicktimeEvent UI for player
		CancelQuicktimeCleaningEventEvent cancelQuicktimeCleaningEvent;
		cancelQuicktimeCleaningEvent.playerNum = player.playerNum;
		postEvent(cancelQuicktimeCleaningEvent);
	}

	player.stunnedTimeCountdown = player.STUNNED_TIME_DURATION;

	player.state = PlayerState::stunned;

	PlayerStateChangedEvent stateChanged;
	stateChanged.playerEntity = a_playerEntity;
	stateChanged.newState     = PlayerState::stunned;
	postEvent(stateChanged);
}



void PlayerSystem::performBasicMovement(entt::entity a_playerEntity, const float a_speedForce)
{
	auto& player   = registry->get<Player>(a_playerEntity);
	auto& playerRB = registry->get<component::RigidBody>(a_playerEntity);

	glm::vec3 deltaVelocity = (player.moveDirection * a_speedForce) - playerRB.getVelocity();
	deltaVelocity.y = 0;
	playerRB.addImpulse(deltaVelocity * 0.85f);
}