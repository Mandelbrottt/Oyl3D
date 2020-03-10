#pragma once
#include "Oyl3D.h"

using oyl::PlayerNumber;

enum class Team
{
	blue,
	red,
	neutral
};

enum class GameEndResult
{
	blueWin,
	redWin,
	tie
};

enum class PlayerState
{
	idle,
	walking,
	jumping,
	falling,
	pushing,
	inCleaningQuicktimeEvent,
	cleaning,
	throwingBottle,
	stunned
};

enum class PlayerItemClassification
{
	primary,
	secondary,
	any
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
	gloop,
	throwableBottle
};

enum class PlayerInteractionResult
{
	nothing,

	//"invalid" results (invalid reticle is displayed)
	invalid,
	cannonFiringSoon,
	needCleaningSolution,
	needMop,
	needGloop,
	needToDropItems,

	//"valid" results (normal reticle is displayed)
	loadCannon,
	pushCannon,
	takeCannonballFromCrate,
	pickUpCannonball,
	pickUpMop,
	pickUpCleaningSolution,
	pickUpGloop,
	pickUpThrowableBottle,
	useGloop,
	cleanGarbagePile
};

enum class ReticleType
{
	normal,
	invalid
};

enum class MenuOption
{
	playGame,
	controls,
	settings,
	credits,
	exit,
	start,
	pause,
	resume,
	playAgain,
	goToMainMenu
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
	PlayerNumber playerNum = PlayerNumber::One; //should always correspond with appropriate PlayerCamera player number
	int controllerNum      = -1; //the gid of the controller that will be used to control the player

	Team team;

	entt::entity primaryCarriedItem   = entt::null;
	entt::entity secondaryCarriedItem = entt::null;
    
	PlayerState state = PlayerState::idle;

	bool transitionIntoQTE = false;

	glm::vec3 moveDirection = glm::vec3(0.0f);

	float speedForce = 5.6f;
	float jumpForce  = 27.0f;
	bool  isJumping  = false;

	float JUMP_COOLDOWN_DURATION = 0.1f;
	float jumpCooldownTimer      = 0.0f;

	float adjustingPositionSpeed = 3.333f;
	float pushingSpeed = 0.3f;

	MoveableUsingLerp adjustingPositionStateData;
	MoveableUsingLerp pushingStateData;

	float CLEANING_TIME_DURATION = 1.2f; //IF YOU CHANGE THIS, MAKE SURE TO ALSO CHANGE THE DEPENDANT VALUES IN GARBAGE PILE AND GARBAGE HP BAR COMPONENETS (check the comments in those components to figure out which ones)
	float cleaningTimeCountdown  = CLEANING_TIME_DURATION;

	float THROWING_DELAY_DURATION      = 0.5f;
	float delayBeforeThrowingCountdown = THROWING_DELAY_DURATION;

	float STUNNED_TIME_DURATION = 2.0f;
	float stunnedTimeCountdown  = CLEANING_TIME_DURATION;

	bool  isCameraLocked = false;
	float yRotationClamp = 0.0f;

	entt::entity interactableEntity = entt::null;
};

struct Cannon
{
	Team team;

	CannonState state = CannonState::doingNothing;
	bool isLoaded     = false;

	float FUSE_DURATION = 2.0f;
	float fuseCountdown = FUSE_DURATION;

	glm::vec3 firingDirection = glm::vec3(1.0f, 1.0f, 1.0f);

	int cannonTrackPosition = 0;
	float pushDistance      = 11.4f;
	float beingPushedSpeed  = 0.3f;
	MoveableUsingLerp pushStateData;

	float WAIT_BEFORE_BEING_PUSHED_DURATION = 0.3f;
	float waitBeforeBeingPushedCountdown    = WAIT_BEFORE_BEING_PUSHED_DURATION;
};

struct CarryableItem
{
	Team team;

	bool isActive = true;

	CarryableItemType type = CarryableItemType::invalid; //must manually be set when spawning items

	bool isBeingCarried = false;
	bool hasBeenCarried = false;
};

struct Respawnable
{
	glm::vec3 spawnPosition = glm::vec3(0.0f);
	glm::vec3 spawnRotation = glm::vec3(0.0f);
};

struct RespawnManager
{
	entt::entity entityPrefab = entt::null;

	CarryableItemType type = CarryableItemType::invalid; //all respawnable items are also carryable items.. so this works fine
	Team team;

	float respawnTimerDuration  = 10.0f;
	float respawnTimerCountdown = respawnTimerDuration;

	bool isRespawnTimerActive = false;
};

struct Cannonball
{
	bool isWaitingToBeFired = false;
	bool isBeingFired       = false;

	std::vector<glm::vec3> splineFollowedWhenFired;
	int currentSplineIndex = 0;

	float interpolationParam = 0.0f;
	float interpolationSpeed = 3.5f;
};

struct Gloop
{
	int numUses = 2;
};

struct ThrowableBottle
{
	float throwSpeed   = 16.0f;
	bool isBeingThrown = false;

	entt::entity playerThrowingEntity = entt::null;
};

struct ThrowBottlePrompt
{

};

struct CannonballCrate
{
	Team team;
};

struct GarbagePile
{
	Team team;

	entt::entity flyEntity;

	bool isGlooped = false;

	int MAX_GARBAGE_LEVEL = 5; //IF YOU CHANGE THIS, UPDATE INITIAL TARGET VALUE IN GarbageMeterBar
	int garbageLevel      = 1; //IF YOU CHANGE THIS, UPDATE INITIAL TARGET VALUE IN GarbageMeterBar

	float GARBAGE_TICKS_PER_LEVEL = 4.0f;
	float garbageTicks = GARBAGE_TICKS_PER_LEVEL;

	float DELAY_BEFORE_ADDING_GARBAGE_DURATION = 1.75f; //this is used to add garbage to a pile that a cannon has shot at, or any other reason a delay could be wanted before adding garbage
	float delayBeforeAddingGarbageCountdown    = -1.0f; //timer shouldnt start at the beginning of the game

	int relativePositionOnShip = -10; //this will be -1 (left), 0 (middle), or 1 (right) to mirror the cannon track position. It is used to determine which pile the cannons are firing at

};

struct PlayerInteractionType
{
	PlayerInteractionResult type;
};

struct PlayerHUDElement
{
	PlayerNumber playerNum = PlayerNumber::One; //should always correspond with player camera's player number
	glm::vec3 positionWhenActive;
};

struct Reticle
{
	ReticleType type;
};

struct GarbagePileHealthBar
{
	entt::entity outlineEntity; //the outline of the HP bar

	Team team;
	int garbagePileNum;
	PlayerNumber playerNum; //player to render to

	float baseInterpolationSpeed = 0.8f; //this should be relative to the player's time it takes to clean (1 / (time it takes to clean) = the value this variable should be)
	float interpolationSpeed     = baseInterpolationSpeed;
	float interpolationParam     = 0.0f;

	float startValue  = 1.0f; //health bar starts at full
	float targetValue = 1.0f;
	float currentFillAmount = startValue;

	bool shouldHideAfterInterpolating = false;
	bool shouldBeHidden               = false;
};

struct GarbagePileGloopIndicator
{
	Team team;
	int garbagePileNum;
	PlayerNumber playerNum; //player to render to

	bool shouldBeHidden = false;
};

struct CleaningQuicktimeEvent
{
	entt::entity cancelPromptEntity = entt::null; //the prompt that tells the player how to cancel the cleaning QTE

	PlayerNumber playerNum;
	entt::entity garbagePileBeingCleaned = entt::null;

	bool isActive     = false;
	bool isPointingUp = true;
};

struct CameraBreathing
{
	float startPosY;

	float cameraHeightVariance = 0.04f;
	float interpolationParam   = 0.5f; //start halfway up (at the default camera height)
	float speed = 0.45f;

	bool isMovingUp = true;
};

struct GarbageMeterBar
{
	float interpolationSpeed = 0.8f;
	float interpolationParam = 0.0f;

	float startValue  = 0.0f; 
	float targetValue = 1.0f / 5.0f; //garbage piles start at level 1.. needs to be updated if starting or max garbage level changes

	int garbagePileNum = 0;
};

struct GarbageMeterDisplay
{
	entt::entity garbageMeterBars[3];

	Team team;
	PlayerNumber playerNum = PlayerNumber::One; //player to render to
};

struct MenuItem
{
	MenuOption type;
};