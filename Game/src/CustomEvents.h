#pragma once
#include "Oyl3D.h"
#include "CustomComponents.h"

enum CustomEventsCategories : int
{
	CategoryPlayer = (int) oyl::EventCategory::CustomStart,
	CategoryCannon,
	CategoryGarbagePile,
	CategoryCannonball,
	CategoryGloop,
	CategoryQuicktimeCleaningEvent
};
enum CustomEventTypes : int
{
    TypePlayerMove = (int) oyl::EventType::CustomStart,
	TypePlayerInteractResult,
    TypePlayerInteractionRequest,
	TypePlayerDropItem,
	TypePlayerStateChange,
    TypeCannonStateChange,
	TypeCannonFired,
	TypeTotalGarbageCount,
	TypeRequestToCleanGarbage,
	TypeGarbageCleaned,
	TypeSpawnCannonballForPlayer,
	TypeUseGloop,
	TypeQuicktimeCleaningEventResult,
	TypeCancelQuicktimeCleaningEvent
};


OYL_EVENT_STRUCT(PlayerMoveEvent, (oyl::EventType) TypePlayerMove, (oyl::EventCategory) CategoryPlayer,
	{
		entt::entity playerEntity;
		glm::vec3    direction;
	});

OYL_EVENT_STRUCT(PlayerInteractResultEvent, (oyl::EventType) TypePlayerInteractResult, (oyl::EventCategory) CategoryPlayer,
	{
		PlayerInteractionResult interactionType;
		int playerNum;
	});

OYL_EVENT_STRUCT(PlayerInteractionRequestEvent, (oyl::EventType) TypePlayerInteractionRequest, (oyl::EventCategory) CategoryPlayer,
	{
		entt::entity playerEntity;
	});

OYL_EVENT_STRUCT(PlayerDropItemEvent, (oyl::EventType) TypePlayerDropItem, (oyl::EventCategory) CategoryPlayer,
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
		glm::vec3 cannonPosition;
	});

OYL_EVENT_STRUCT(TotalGarbageCountEvent, (oyl::EventType) TypeTotalGarbageCount, (oyl::EventCategory) CategoryGarbagePile,
	{
		int totalGarbageCount;
	});

OYL_EVENT_STRUCT(RequestToCleanGarbageEvent, (oyl::EventType) TypeRequestToCleanGarbage, (oyl::EventCategory) CategoryGarbagePile,
	{
		entt::entity garbagePileEntity;
	});

OYL_EVENT_STRUCT(GarbageCleanedEvent, (oyl::EventType) TypeGarbageCleaned, (oyl::EventCategory) CategoryGarbagePile,
	{
		float numGarbageTicksToDisplay;
		bool  displayGlooped;
	});

OYL_EVENT_STRUCT(SpawnCannonballForPlayerEvent, (oyl::EventType) TypeSpawnCannonballForPlayer, (oyl::EventCategory) CategoryCannonball,
	{
		entt::entity playerEntity;
	});

OYL_EVENT_STRUCT(UseGloopEvent, (oyl::EventType) TypeUseGloop, (oyl::EventCategory) CategoryGloop,
	{
		entt::entity gloopEntity;
	});

OYL_EVENT_STRUCT(QuicktimeCleaningEventResultEvent, (oyl::EventType) TypeQuicktimeCleaningEventResult, (oyl::EventCategory) CategoryQuicktimeCleaningEvent,
	{
		entt::entity playerEntity;
		bool         wasSuccessful;
	});

OYL_EVENT_STRUCT(CancelQuicktimeCleaningEventEvent, (oyl::EventType) TypeCancelQuicktimeCleaningEvent, (oyl::EventCategory) CategoryQuicktimeCleaningEvent,
	{
		int playerNum;
	});