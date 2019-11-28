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
    invalid,
    cannonball,
    mop,
    cleaningSolution,
    gloop
};

enum class CarryingItemState
{
    nothing,
    cannonball,
    mop,
    cleaningSolution,
    mopAndCleaningSolution,
    gloop
};

enum class PlayerInteractionResult
{
	nothing,
	invalid,
	cannonFiringSoon,
	loadCannon,
	pushCannon,
	takeCannonballFromCrate,
	pickUpCannonball,
	pickUpMop,
	cleanGarbagePile,
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

	CarryingItemState carriedItem = CarryingItemState::nothing;
    
	PlayerState state = PlayerState::idle;

	float speedForce = 7.0f;
	glm::vec3 moveDirection = glm::vec3(0.0f);

	float adjustingPositionSpeed = 3.333f;
	float pushingSpeed = 0.2f;

	MoveableUsingLerp adjustingPositionStateData; //TODO: integrate adjusting position into pushing state instead of having its own state
	MoveableUsingLerp pushingStateData;

	float yRotationClamp = 0.0f;

	entt::entity interactableEntity = entt::null;
};

struct Cannon
{
	Team team;

	CannonState state = CannonState::doingNothing;
	bool isLoaded = false;

	TimedAction fuse;

	int cannonTrackPosition = 0;
	float pushDistance = 5.0f;
	float beingPushedSpeed = 0.2f;
	MoveableUsingLerp pushStateData;

	float waitTimeBeforeBeingPushed = 0.3f;
	TimedAction waitTimer;
};

struct CarryableItem
{
	Team team;

	CarryableItemType type = CarryableItemType::invalid; //must manually be set when spawning items
	bool isBeingCarried = false;

	bool isActive = true;
};

struct GarbagePile
{
	Team team;

	int MAX_GARBAGE_LEVEL = 5;
	int garbageLevel = 1;

	int MAX_GARBAGE_LEVEL_TICKS = 3; // each level will have 3 sub-levels (or ticks) before it goes down by 1
	int garbageLevelTicks = 3;
};

struct CannonballCrate
{
	Team team;
};

struct PlayerInteractionType
{
	PlayerInteractionResult type;
};