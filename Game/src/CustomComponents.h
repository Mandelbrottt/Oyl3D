#pragma once
#include "Oyl3D.h"

enum class Teams
{
	blue,
	red
};

struct TimedAction
{
	float elapsed = 0.0f;
	float timeToWait;
};

struct MoveableUsingLerp
{
	glm::vec3 startPos;
	glm::vec3 desinationPos;

	float speed;
	float interpolationParam = 0.0f;
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

struct Player
{
	PlayerState state = PlayerState::idle;

	float speedForce = 500.0f;
	glm::vec3 moveDirection = glm::vec3(0.0f);

	float adjustingPositionSpeed = 3.333f;
	float pushingSpeed = 0.2f;

	MoveableUsingLerp adjustingPositionStateData; //TODO: integrate adjusting position into pushing state instead of having its own state
	MoveableUsingLerp pushingStateData;

	entt::entity interactableEntity = entt::null;
};

struct Cannon
{
	Teams team;

	CannonState state = CannonState::doingNothing;
	bool isLoaded = false;

	TimedAction fuse;

	float beingPushedSpeed = 0.2f;
	MoveableUsingLerp pushStateData;

	float waitTimeBeforeBeingPushed = 0.3f;
	TimedAction waitTimer; //TODO: integrate wait timer into pushing state instead of having its own state

	bool isPushingValid = true; //TODO: change to false initially, validation system will check validation for pushing and set this flag
	bool isLoadingCannonballValid = true; //TODO: change to false initially, validation system will check validation for loading with cannonball and set this flag
};