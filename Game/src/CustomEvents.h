#pragma once
#include "Oyl3D.h"
#include "CustomComponents.h"

enum CustomEventsCategories
{
	CategoryPlayer = oyl::CategoryCustomStart,
	CategoryCannon,
	CategoryGarbagePile,
	CategoryGloop,
	CategoryQuicktimeCleaningEvent
};
enum CustomEventTypes
{
    TypePlayerMove = oyl::EventTypeCustomStart,
	TypePlayerInteractResult,
    TypePlayerInteractionRequest,
	TypePlayerDropItem,
	TypePlayerStateChange,
    TypeCannonStateChange,
	TypeCannonFired,
	TypeTotalGarbageCount,
	TypeRequestToCleanGarbage,
	TypeGarbageCleaned,
	TypeUseGloop,
	TypeQuicktimeCleaningEventResult
};


OYL_EVENT_STRUCT(PlayerMoveEvent, TypePlayerMove, CategoryPlayer,
	{
		entt::entity playerEntity;
		glm::vec3    direction;
	});

OYL_EVENT_STRUCT(PlayerInteractResultEvent, TypePlayerInteractResult, CategoryPlayer,
	{
		PlayerInteractionResult interactionType;
	});

OYL_EVENT_STRUCT(PlayerInteractionRequestEvent, TypePlayerInteractionRequest, CategoryPlayer,
	{
		entt::entity playerEntity;
	});

OYL_EVENT_STRUCT(PlayerDropItemEvent, TypePlayerDropItem, CategoryPlayer,
	{
		entt::entity playerEntity;
	});

OYL_EVENT_STRUCT(PlayerStateChangeEvent, TypePlayerStateChange, CategoryPlayer,
	{
		entt::entity playerEntity;
        PlayerState  newState;
	});

OYL_EVENT_STRUCT(CannonStateChangeEvent, TypeCannonStateChange, CategoryCannon,
	{
		entt::entity cannonEntity;
		CannonState  newState;
	});

OYL_EVENT_STRUCT(CannonFiredEvent, TypeCannonFired, CategoryCannon,
	{
		glm::vec3 cannonPosition;
	});

OYL_EVENT_STRUCT(TotalGarbageCountEvent, TypeTotalGarbageCount, CategoryGarbagePile,
	{
		int totalGarbageCount;
	});

OYL_EVENT_STRUCT(RequestToCleanGarbageEvent, TypeRequestToCleanGarbage, CategoryGarbagePile,
	{
		entt::entity garbagePileEntity;
	});

OYL_EVENT_STRUCT(GarbageCleanedEvent, TypeGarbageCleaned, CategoryGarbagePile,
	{
		float numGarbageTicksToDisplay;
		bool  displayGlooped;
	});

OYL_EVENT_STRUCT(UseGloopEvent, TypeUseGloop, CategoryGloop,
	{
		entt::entity gloopEntity;
	});

OYL_EVENT_STRUCT(QuicktimeCleaningEventResultEvent, TypeQuicktimeCleaningEventResult, CategoryQuicktimeCleaningEvent,
	{
		entt::entity playerEntity;
		bool         wasSuccessful;
	});