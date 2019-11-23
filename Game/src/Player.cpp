#include "Player.h"

void PlayerSystem::onEnter()
{
	addToCategoryMask(CategoryPlayer);
}

void PlayerSystem::onExit()
{
    
}

void PlayerSystem::onUpdate(Timestep dt)
{
	auto view = registry->view<Player, component::Transform, component::RigidBody>();
	for (auto& entity : view)
	{
		auto& player = registry->get<Player>(entity);
		auto& playerTransform = registry->get<component::Transform>(entity);
		auto& playerRB = registry->get<component::RigidBody>(entity);

		switch (player.state)
		{
		    case PlayerState::idle:
		    {
				playerRB.impulse = glm::vec3(0.0f);
				break;
		    }
		    
		    case PlayerState::walking:
			{
				glm::vec3 deltaVelocity = (player.moveDirection * player.speedForce) - playerRB.velocity;
				playerRB.impulse        = playerRB.mass * deltaVelocity * static_cast<float>(dt);

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
				if (player.adjustingPositionStateData.interpolationParam < 1.0f)
				{
					player.adjustingPositionStateData.interpolationParam = std::min(
						player.adjustingPositionStateData.interpolationParam + player.adjustingPositionStateData.speed * dt,
						1.0f);

					playerTransform.setPosition(glm::mix(player.adjustingPositionStateData.startPos,
						player.adjustingPositionStateData.desinationPos,
						player.adjustingPositionStateData.interpolationParam));
				}
				else //adjustingposition interpolation parameter >= 1.0f AKA player is done adjusting their position
				{
				    //the player is pushing
					player.pushingStateData.interpolationParam = std::min(player.pushingStateData.interpolationParam + player.pushingStateData.speed * dt,
						1.0f);

					playerTransform.setPosition(glm::mix(player.pushingStateData.startPos,
						player.pushingStateData.desinationPos,
						player.pushingStateData.interpolationParam));

					if (player.pushingStateData.interpolationParam >= 1.0f)
						changeToIdle(&player);
				}

				break;
		    }

			case PlayerState::cleaning:
			{
		        
				break;
			}
		}

	    //reset player's movement before the next frame
		player.moveDirection = glm::vec3(0.0f);
	}
}

bool PlayerSystem::onEvent(Ref<Event> event)
{
	switch (event->type)
	{
		case TypePlayerStateChange:
		{
			auto evt = (PlayerStateChangeEvent)* event;
			switch (evt.newState)
			{
			    case PlayerState::pushing:
			    {
					changeToPushing(evt.player);
				    break;
			    }
			}

			break;
		}
	    
		case TypePlayerMove:
		{
			auto evt = (PlayerMoveEvent)* event;
		    
			evt.player->moveDirection += evt.direction;
			changeToWalking(evt.player);
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

void PlayerSystem::changeToCleaning(Player* a_player)
{
	a_player->state = PlayerState::cleaning;
}