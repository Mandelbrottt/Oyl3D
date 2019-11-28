#pragma once
#include "Oyl3D.h"
#include "CustomComponents.h"

enum CustomEventsCategories
{
	CategoryPlayer = oyl::CategoryCustomStart,
	CategoryCannon,
	CategoryGarbagePile
};
enum CustomEventTypes
{
    TypePlayerMove = oyl::EventTypeCustomStart,
	TypePlayerInteractResult,
    TypePlayerInteractionRequest,
	TypePlayerDropItem,
	TypePlayerStateChange,
    TypeCannonStateChange,
    TypeLoadCannon,
	TypeTotalGarbageCount
};


OYL_EVENT_STRUCT(PlayerMoveEvent, TypePlayerMove, CategoryPlayer,
	{
		Player*   player;
		glm::vec3 direction;
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
		Player*     player;
        PlayerState newState;
	});

OYL_EVENT_STRUCT(CannonStateChangeEvent, TypeCannonStateChange, CategoryCannon,
	{
		Cannon*      cannon;
		CannonState  newState;
	});

OYL_EVENT_STRUCT(LoadCannonEvent, TypeLoadCannon, CategoryCannon,
	{
		Cannon* cannon;
	});

OYL_EVENT_STRUCT(TotalGarbageCountEvent, TypeTotalGarbageCount, CategoryGarbagePile,
	{
		int totalGarbageCount;
	});