#pragma once
#include "Oyl3D.h"
#include "CustomComponents.h"

enum CustomEventsCategories : int
{
	CategoryPlayer = (int)oyl::EventCategory::CustomStart,
	CategoryCannon,
	CategoryGarbagePile,
	CategoryCannonball,
	CategoryCleaningSolution,
	CategoryGloop,
	CategoryThrowableBottle,
	CategoryQuicktimeCleaningEvent,
	CategoryGameState,
	CategoryTutorial
};
enum CustomEventTypes : int
{
    TypePlayerMove = (int)oyl::EventType::CustomStart,
	TypePlayerJump,
	TypePlayerInteractResult,
    TypePlayerInteractionRequest,
	TypePlayerPickedUpItem,
	TypePlayerDropItemRequest,
	TypePlayerDroppedItem,
	TypeCancelButtonPressed,
	TypePlayerStateChangeRequest,
	TypePlayerStateChanged,
    TypeCannonStateChangeRequest,
	TypeCannonLoaded,
	TypeCannonFired,
	TypeRequestToCleanGarbage,
	TypeGarbageCleaned,
	TypeGarbageGlooped,
	TypeGarbagePileReappeared,
	TypeIncreasedGarbageLevel,
	TypeSetMaxGarbageLevel,
	TypeMopUsed,
	TypeSpawnCannonballForPlayer,
	TypeCleaningSolutionUsed,
	TypeUseGloopRequest,
	TypeGloopUsed,
	TypeThrowBottle,
	TypeThrowableBottleHit,
	TypeActivateQuicktimeCleaningEvent,
	TypeCancelQuicktimeCleaningEvent,
	TypeStickMovedDuringQuicktimeCleaningEvent,
	TypeCleanedWithMop,
	TypePerformCleaning,
	TypeGameEnd,
	TypeTutorialSegmentStarted
};

OYL_EVENT_STRUCT(PlayerJumpEvent, (oyl::EventType) TypePlayerJump, (oyl::EventCategory) CategoryPlayer,
	{
		entt::entity playerEntity;
	});

OYL_EVENT_STRUCT(PlayerInteractResultEvent, (oyl::EventType) TypePlayerInteractResult, (oyl::EventCategory) CategoryPlayer,
	{
		PlayerInteractionResult interactionType;
		PlayerNumber playerNum;
	});

OYL_EVENT_STRUCT(PlayerInteractionRequestEvent, (oyl::EventType) TypePlayerInteractionRequest, (oyl::EventCategory) CategoryPlayer,
	{
		entt::entity playerEntity;
		PlayerItemClassification itemClassificatonToUse;
	});

OYL_EVENT_STRUCT(PlayerPickedUpItemEvent, (oyl::EventType) TypePlayerPickedUpItem, (oyl::EventCategory) CategoryPlayer,
	{
		entt::entity playerEntity;
		CarryableItemType itemType;
		Team itemTeam;
	});

OYL_EVENT_STRUCT(PlayerDropItemRequestEvent, (oyl::EventType) TypePlayerDropItemRequest, (oyl::EventCategory) CategoryPlayer,
	{
		entt::entity playerEntity;
		PlayerItemClassification itemClassificationToDrop;
		bool forceDrop; //if true, forces the player's item(s) to drop no matter what state they're in
	});

OYL_EVENT_STRUCT(PlayerDroppedItemEvent, (oyl::EventType) TypePlayerDroppedItem, (oyl::EventCategory) CategoryPlayer,
	{
		entt::entity playerEntity;
		PlayerItemClassification itemClassificationToDrop;
	});

OYL_EVENT_STRUCT(CancelButtonPressedEvent, (oyl::EventType) TypeCancelButtonPressed, (oyl::EventCategory) CategoryPlayer,
	{
		entt::entity playerEntity;
	});

OYL_EVENT_STRUCT(PlayerStateChangeRequestEvent, (oyl::EventType) TypePlayerStateChangeRequest, (oyl::EventCategory) CategoryPlayer,
	{
		entt::entity playerEntity;
        PlayerState  newState;
	});

OYL_EVENT_STRUCT(PlayerStateChangedEvent, (oyl::EventType) TypePlayerStateChanged, (oyl::EventCategory) CategoryPlayer,
	{
		entt::entity playerEntity;
		PlayerState  oldState;
		PlayerState  newState;
	});

OYL_EVENT_STRUCT(CannonStateChangeRequestEvent, (oyl::EventType) TypeCannonStateChangeRequest, (oyl::EventCategory) CategoryCannon,
	{
		entt::entity cannonEntity;
		CannonState  newState;
	});

OYL_EVENT_STRUCT(CannonLoadedEvent, (oyl::EventType) TypeCannonLoaded, (oyl::EventCategory) CategoryCannon,
	{
		entt::entity cannonEntity;
		entt::entity playerEntity;
	});

OYL_EVENT_STRUCT(CannonFiredEvent, (oyl::EventType) TypeCannonFired, (oyl::EventCategory) CategoryCannon,
	{
		entt::entity cannonEntity;
	});

OYL_EVENT_STRUCT(RequestToCleanGarbageEvent, (oyl::EventType) TypeRequestToCleanGarbage, (oyl::EventCategory) CategoryGarbagePile,
	{
		entt::entity garbagePileEntity;
	});

OYL_EVENT_STRUCT(GarbageCleanedEvent, (oyl::EventType) TypeGarbageCleaned, (oyl::EventCategory) CategoryGarbagePile,
	{
		entt::entity garbagePileEntity;
	}); 

OYL_EVENT_STRUCT(GarbageGloopedEvent, (oyl::EventType) TypeGarbageGlooped, (oyl::EventCategory) CategoryGarbagePile,
	{
		entt::entity garbagePileEntity;
	});

OYL_EVENT_STRUCT(GarbagePileReappearedEvent, (oyl::EventType) TypeGarbagePileReappeared, (oyl::EventCategory) CategoryGarbagePile,
	{
		entt::entity garbagePileEntity;
	});

OYL_EVENT_STRUCT(IncreasedGarbageLevelEvent, (oyl::EventType) TypeIncreasedGarbageLevel, (oyl::EventCategory) CategoryGarbagePile,
	{
		entt::entity garbagePileEntity;
	});

OYL_EVENT_STRUCT(SetMaxGarbageLevelEvent, (oyl::EventType) TypeSetMaxGarbageLevel, (oyl::EventCategory) CategoryGarbagePile,
	{

	});

OYL_EVENT_STRUCT(SpawnCannonballForPlayerEvent, (oyl::EventType) TypeSpawnCannonballForPlayer, (oyl::EventCategory) CategoryCannonball,
	{
		entt::entity playerEntity;
		Team team;
	});

OYL_EVENT_STRUCT(CleaningSolutionUsedEvent, (oyl::EventType) TypeCleaningSolutionUsed, (oyl::EventCategory) CategoryCleaningSolution,
	{
		entt::entity playerEntity;
	});

OYL_EVENT_STRUCT(UseGloopRequestEvent, (oyl::EventType) TypeUseGloopRequest, (oyl::EventCategory) CategoryGloop,
	{
		entt::entity playerEntity;
		entt::entity gloopEntity;
	});

OYL_EVENT_STRUCT(GloopedUsedEvent, (oyl::EventType) TypeGloopUsed, (oyl::EventCategory) CategoryGloop,
	{
		entt::entity playerEntity;
		bool isLastUse;
	});

OYL_EVENT_STRUCT(ThrowBottleEvent, (oyl::EventType) TypeThrowBottle, (oyl::EventCategory) CategoryThrowableBottle,
	{
		entt::entity bottleEntity;
		entt::entity playerThrowingEntity;
	}); 

OYL_EVENT_STRUCT(ThrowableBottleHitEvent, (oyl::EventType) TypeThrowableBottleHit, (oyl::EventCategory) CategoryThrowableBottle,
	{
		entt::entity bottleEntity;
		entt::entity playerHitEntity;
		bool hitPlayer;
	});

OYL_EVENT_STRUCT(ActivateQuicktimeCleaningEventEvent, (oyl::EventType) TypeActivateQuicktimeCleaningEvent, (oyl::EventCategory) CategoryQuicktimeCleaningEvent,
	{
		PlayerNumber playerNum;
		entt::entity garbagePileEntity;
	});

OYL_EVENT_STRUCT(CancelQuicktimeCleaningEventEvent, (oyl::EventType) TypeCancelQuicktimeCleaningEvent, (oyl::EventCategory) CategoryQuicktimeCleaningEvent,
	{
		entt::entity playerEntity;
	});

OYL_EVENT_STRUCT(StickMovedDuringQuicktimeCleaningEventEvent, (oyl::EventType) TypeStickMovedDuringQuicktimeCleaningEvent, (oyl::EventCategory) CategoryQuicktimeCleaningEvent,
	{
		entt::entity playerEntity;
		float stickPosY;
	});

OYL_EVENT_STRUCT(CleanedWithMopEvent, (oyl::EventType) TypeCleanedWithMop, (oyl::EventCategory) CategoryQuicktimeCleaningEvent,
	{
		entt::entity playerEntity;
		bool moppingForward;
	});

OYL_EVENT_STRUCT(PerformCleaningEvent, (oyl::EventType) TypePerformCleaning, (oyl::EventCategory) CategoryQuicktimeCleaningEvent,
	{
		entt::entity playerEntity;
	});

OYL_EVENT_STRUCT(GameEndEvent, (oyl::EventType) TypeGameEnd, (oyl::EventCategory) CategoryGameState,
	{
		GameEndResult result;
	});

OYL_EVENT_STRUCT(TutorialSegmentStartedEvent, (oyl::EventType) TypeTutorialSegmentStarted, (oyl::EventCategory) CategoryTutorial,
	{
		int segmentNum;
	});