#include "PlayerSystem.h"

void PlayerSystem::onEnter()
{
	this->listenForEventCategory((EventCategory)CategoryPlayer);
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
		case (EventType) TypePlayerStateChange:
		{
			auto evt     = event_cast<PlayerStateChangeEvent>(event);
			auto& player = registry->get<Player>(evt.playerEntity);

			switch (evt.newState)
			{
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
	    
		case (EventType)TypePlayerMove:
		{
			auto evt     = event_cast<PlayerMoveEvent>(event);
			auto& player = registry->get<Player>(evt.playerEntity);

			player.moveDirection += evt.direction;
			changeToWalking(&player);
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
	a_player->adjustingPositionStateData.speed  = a_player->adjustingPositionSpeed;
    
	a_player->pushingStateData.interpolationParam = 0.0f;
	a_player->pushingStateData.speed = a_player->pushingSpeed;
    
	a_player->state = PlayerState::pushing;
}

void PlayerSystem::changeToInCleaningQuicktimeEvent(Player* a_player)
{
	a_player->state = PlayerState::inCleaningQuicktimeEvent;
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