#pragma once
#include "Oyl3D.h"

enum class Team
{
	blue,
	red
};

enum class PlayerState
{
	idle,
	walking,
	jumping,
	falling,
	pushing,
	cleaning
};

enum class CannonState
{
	doingNothing,
	beingPushed,
	firingSoon
};

enum class CarryableItemType
{
    nothing,
    cannonball,
    mop,
    cleaningSolution,
    gloop
};

struct TimedAction
{
	float elapsed = 0.0f;
	float timeToWait;
};

struct MoveableUsingLerp
{
	glm::vec3 startPos;
	glm::vec3 destinationPos;

	float speed;
	float interpolationParam = 0.0f;
};

struct Player
{
	Team team;

	CarryableItemType carriedItem = CarryableItemType::nothing;
    
	PlayerState state = PlayerState::idle;

	float speedForce = 40.0f;
	glm::vec3 moveDirection = glm::vec3(0.0f);

	float adjustingPositionSpeed = 3.333f;
	float pushingSpeed = 0.2f;

	MoveableUsingLerp adjustingPositionStateData; //TODO: integrate adjusting position into pushing state instead of having its own state
	MoveableUsingLerp pushingStateData;

	entt::entity interactableEntity = entt::null;
};

struct Cannon
{
	Team team;

	CannonState state = CannonState::doingNothing;
	bool isLoaded = false;

	TimedAction fuse;

	glm::vec3 initialPosition;
	float pushDistance = 5.0f;
	float beingPushedSpeed = 0.2f;
	MoveableUsingLerp pushStateData;

	float waitTimeBeforeBeingPushed = 0.3f;
	TimedAction waitTimer; //TODO: integrate wait timer into pushing state instead of having its own state
};

struct CarryableItem
{
	Team team;

	CarryableItemType type = CarryableItemType::nothing; //must manually be set when spawning items
	bool isBeingCarried = false;
};