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
	inCleaningQuicktimeEvent,
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
	pickUpCleaningSolution,
	pickUpGloop,
	useGloop,
	cleanGarbagePile,
};

enum class ReticleType
{
	normal,
	invalid
};

struct MoveableUsingLerp
{
	glm::vec3 startPos;
	glm::vec3 destinationPos;

	float speed;
	bool  isMovingForward = true;
	float interpolationParam = 0.0f;
};

struct Player
{
	int playerNum = -1; //should always correspond with appropriate PlayerCamera player number

	Team team;

	entt::entity primaryCarriedItem   = entt::null;
	entt::entity secondaryCarriedItem = entt::null;
    
	PlayerState state = PlayerState::idle;

	float speedForce = 7.0f;
	glm::vec3 moveDirection = glm::vec3(0.0f);

	float adjustingPositionSpeed = 3.333f;
	float pushingSpeed = 0.2f;

	MoveableUsingLerp adjustingPositionStateData; //TODO: integrate adjusting position into pushing state instead of having its own state
	MoveableUsingLerp pushingStateData;

	float CLEANING_TIME_DURATION = 1.5f;
	float cleaningTimeCountdown = CLEANING_TIME_DURATION;

	float yRotationClamp = 0.0f;

	entt::entity interactableEntity = entt::null;
};

struct Cannon
{
	Team team;

	CannonState state = CannonState::doingNothing;
	bool isLoaded = false;

	float FUSE_DURATION = 20.0f;
	float fuseCountdown = FUSE_DURATION;

	int cannonTrackPosition = 0;
	float pushDistance = 10.0f;
	float beingPushedSpeed = 0.2f;
	MoveableUsingLerp pushStateData;

	float WAIT_BEFORE_BEING_PUSHED_DURATION = 0.3f;
	float waitBeforeBeingPushedCountdown = WAIT_BEFORE_BEING_PUSHED_DURATION;
};

struct CarryableItem
{
	Team team;

	CarryableItemType type = CarryableItemType::invalid; //must manually be set when spawning items
	bool isBeingCarried = false;

	bool isActive = false;
};

struct Cannonball
{
	bool isBeingFired = false;

	glm::vec3 v1;
	glm::vec3 v2;
	glm::vec3 v3;
	glm::vec3 v4;

	float interpolationParam = 0.0f;
};

struct Gloop
{
	int numUses = 2;
};

struct CannonballCrate
{
	Team team;
};

struct GarbagePile
{
	Team team;

	bool isGlooped = false;

	int MAX_GARBAGE_LEVEL = 5;
	int garbageLevel = 1;

	float GARBAGE_TICKS_PER_LEVEL = 3.0f;
	float garbageTicks = 3.0f;
};

struct PlayerInteractionType
{
	PlayerInteractionResult type;
};

struct Reticle
{
	ReticleType type;
};

struct EndScreen
{
	bool isLoseScreen;
};

struct GarbageTick
{
	float ON_SCREEN_DURATION = 2.5f;
	float onScreenCountdown = ON_SCREEN_DURATION;
};

struct CleaningQuicktimeEventIndicator
{
	bool isActive = false;

	float LOWER_BOUND_FOR_SUCCESS = 0.4f;
	float UPPER_BOUND_FOR_SUCCESS = 0.6f;

	MoveableUsingLerp lerpInformation;

	entt::entity cleaningQuicktimeEventBackground;
};