#pragma once
#include "Oyl3D.h"

enum CustomEventsCategories
{
	CategoryCannon = oyl::CategoryCustomStart,
	CategoryPlayer
};
enum CustomEventTypes
{
	TypeCannonInteract = oyl::EventTypeCustomStart,
    TypePlayerInteract,
    TypePlayerMove
};