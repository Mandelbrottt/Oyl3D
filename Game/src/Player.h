#pragma once
#include "CustomEvents.h"
#include "CustomComponents.h"

using namespace oyl;

/////////// EVENTS ///////////
OYL_EVENT_STRUCT(PlayerInteractEvent, TypePlayerInteract, CategoryPlayer,
	{
		entt::entity player;
	});

OYL_EVENT_STRUCT(PlayerMoveEvent, TypePlayerMove, CategoryPlayer,
	{
		entt::entity player;
        glm::vec3 direction;
	});
//////////////////////////////

enum class PlayerStates
{
	idle,
    walking,
    jumping,
    falling,
    adjustingPosition,
    pushing,
    cleaning
};

struct Player
{
	PlayerStates state = PlayerStates::idle;

	float speedForce = 2.0f;
	glm::vec3 moveDirection = glm::vec3(0.0f);
    
	MoveableUsingLerp adjustingPositionStateData;
	MoveableUsingLerp pushingStateData;

	entt::entity e = entt::null;
};

class PlayerSystem : public System
{
	OYL_CTOR(PlayerSystem, System)

	void onEnter() override;
	void onExit() override;
	void onUpdate(Timestep dt) override;
	bool onEvent(Ref<Event> event) override;

	//state changing functions
	void changeToIdle(Player* a_player);
	void changeToWalking(Player* a_player);
	void changeToJumping(Player* a_player);
	void changeToFalling(Player* a_player);
	void changeToAdjustingPosition(Player* a_player, component::Transform a_playerTransform, glm::vec3 a_destinationPos);
	void changeToPushing(Player* a_player, component::Transform a_playerTransform, glm::vec3 a_destinationPos);
	void changeToCleaning(Player* a_player);
};
