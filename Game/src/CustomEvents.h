#pragma once
#include "Oyl3D.h"
#include "CustomComponents.h"

enum CustomEventsCategories : int
{
	CategoryPlayer = (int)oyl::EventCategory::CustomStart,
	CategoryCannon,
	CategoryGarbagePile,
	CategoryCannonball,
	CategoryGloop,
	CategoryQuicktimeCleaningEvent,
	CategoryGameState
};
enum CustomEventTypes : int
{
    TypePlayerMove = (int)oyl::EventType::CustomStart,
	TypePlayerJump,
	TypePlayerInteractResult,
    TypePlayerInteractionRequest,
	TypeCancelButtonPressed,
	TypePlayerStateChange,
    TypeCannonStateChange,
	TypeCannonFired,
	TypeRequestToCleanGarbage,
	TypeGarbageCleaned,
	TypeGarbagePileReappeared,
	TypeIncreasedGarbageLevel,
	TypeSpawnCannonballForPlayer,
	TypeUseGloop,
	TypeActivateQuicktimeCleaningEvent,
	TypeCancelQuicktimeCleaningEvent,
	TypeGameEnd
};

enum GameEndResult
{
	blueWin,
	redWin,
	tie
};

OYL_EVENT_STRUCT(PlayerMoveEvent, (oyl::EventType) TypePlayerMove, (oyl::EventCategory) CategoryPlayer,
	{
		entt::entity playerEntity;
		glm::vec3    direction;
	});

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
	});

OYL_EVENT_STRUCT(CancelButtonPressedEvent, (oyl::EventType) TypeCancelButtonPressed, (oyl::EventCategory) CategoryPlayer,
	{
		entt::entity playerEntity;
	});

OYL_EVENT_STRUCT(PlayerStateChangeEvent, (oyl::EventType) TypePlayerStateChange, (oyl::EventCategory) CategoryPlayer,
	{
		entt::entity playerEntity;
        PlayerState  newState;
	});

OYL_EVENT_STRUCT(CannonStateChangeEvent, (oyl::EventType) TypeCannonStateChange, (oyl::EventCategory) CategoryCannon,
	{
		entt::entity cannonEntity;
		CannonState  newState;
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

OYL_EVENT_STRUCT(GarbagePileReappearedEvent, (oyl::EventType) TypeGarbagePileReappeared, (oyl::EventCategory) CategoryGarbagePile,
	{
		entt::entity garbagePileEntity;
	});

OYL_EVENT_STRUCT(IncreasedGarbageLevelEvent, (oyl::EventType) TypeIncreasedGarbageLevel, (oyl::EventCategory) CategoryGarbagePile,
	{
		entt::entity garbagePileEntity;
	});

OYL_EVENT_STRUCT(SpawnCannonballForPlayerEvent, (oyl::EventType) TypeSpawnCannonballForPlayer, (oyl::EventCategory) CategoryCannonball,
	{
		entt::entity playerEntity;
		Team team;
	});

OYL_EVENT_STRUCT(UseGloopEvent, (oyl::EventType) TypeUseGloop, (oyl::EventCategory) CategoryGloop,
	{
		entt::entity gloopEntity;
	});

OYL_EVENT_STRUCT(ActivateQuicktimeCleaningEventEvent, (oyl::EventType) TypeActivateQuicktimeCleaningEvent, (oyl::EventCategory) CategoryQuicktimeCleaningEvent,
	{
		PlayerNumber playerNum;
		entt::entity garbagePileEntity;
	});

OYL_EVENT_STRUCT(CancelQuicktimeCleaningEventEvent, (oyl::EventType) TypeCancelQuicktimeCleaningEvent, (oyl::EventCategory) CategoryQuicktimeCleaningEvent,
	{
		PlayerNumber playerNum;
	});

OYL_EVENT_STRUCT(GameEndEvent, (oyl::EventType) TypeGameEnd, (oyl::EventCategory) CategoryGameState,
	{
		GameEndResult result;
	});