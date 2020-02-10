#include "PlayerSystem.h"

void PlayerSystem::onEnter()
{
	this->listenForEventCategory((EventCategory)CategoryPlayer);
	this->listenForEventType(EventType::PhysicsCollisionStay);
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
				if (player.moveDirection != glm::vec3(0.0f))
					changeToWalking(&player);
		        
				break;
		    }
		    
		    case PlayerState::walking:
			{
				performBasicMovement(playerEntity, player.speedForce, Time::deltaTime());

				if (player.moveDirection == glm::vec3(0.0f))
					changeToIdle(&player);

				break;
			}

			case PlayerState::jumping:
			{
		        
				break;
			}

			case PlayerState::falling:
			{

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
				performBasicMovement(playerEntity, player.speedForce, Time::deltaTime());

				break;
			}

			case PlayerState::cleaning:
			{
				performBasicMovement(playerEntity, player.speedForce * 0.73f, Time::deltaTime());

				player.cleaningTimeCountdown -= Time::deltaTime();
				if (player.cleaningTimeCountdown < 0.0f)
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
		case (EventType)TypePlayerMove:
		{
			auto evt = event_cast<PlayerMoveEvent>(event);

			auto& player = registry->get<Player>(evt.playerEntity);

			player.moveDirection += evt.direction;
			changeToWalking(&player);

			break;
		}

		case (EventType)TypePlayerJump:
		{
			auto evt = event_cast<PlayerJumpEvent>(event);

			auto& player   = registry->get<Player>(evt.playerEntity);
			auto& playerRB = registry->get<component::RigidBody>(evt.playerEntity);

			if (!player.isJumping)
			{
				playerRB.addImpulse(glm::vec3(0.0f, 1.0f, 0.0f) * player.jumpForce);
				player.isJumping         = true;
				player.jumpCooldownTimer = player.JUMP_COOLDOWN_DURATION;
			}

			break;
		}

		case (EventType) TypePlayerStateChange:
		{
			auto evt     = event_cast<PlayerStateChangeEvent>(event);
			auto& player = registry->get<Player>(evt.playerEntity);

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
			}

			break;
		}

		case EventType::PhysicsCollisionStay:
		{
			auto evt = event_cast<PhysicsCollisionStayEvent>(event);

			entt::entity playerEntity = entt::null;

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
				player.isJumping         = false;
				player.jumpCooldownTimer = player.JUMP_COOLDOWN_DURATION;
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

void PlayerSystem::performBasicMovement(entt::entity a_playerEntity, const float a_speedForce, const float a_dt)
{
	auto& player   = registry->get<Player>(a_playerEntity);
	auto& playerRB = registry->get<component::RigidBody>(a_playerEntity);

	glm::vec3 deltaVelocity = (player.moveDirection * a_speedForce) - playerRB.getVelocity();
	playerRB.addImpulse(playerRB.getMass() * deltaVelocity * static_cast<float>(a_dt));
}

void PlayerSystem::checkAndResolveSlopeCollision(entt::entity a_playerEntity)
{
	auto& playerTransform = registry->get<component::Transform>(a_playerEntity);
	auto& palyerRB        = registry->get<component::RigidBody>(a_playerEntity);

	auto ray = RayTest::Closest(playerTransform.getPositionGlobal(), glm::vec3(0.0f, -1.0f, 0.0f), 0.5001f);
	if (ray->hasHit && registry->valid(ray->hitObject.entity))
	{
		entt::entity raycastHitEntity = ray->hitObject.entity;
	}
}