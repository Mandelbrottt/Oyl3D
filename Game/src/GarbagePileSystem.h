#pragma once
#include "CustomEvents.h"
#include "CustomComponents.h"

using namespace oyl;

class GarbagePileSystem : public System
{
	OYL_CTOR(GarbagePileSystem, System)

	const float PASSIVE_GARBAGE_BUILDUP_TIME = 30.0f;
	float passiveGarbageBuildupCountdown = PASSIVE_GARBAGE_BUILDUP_TIME;
	int numBuildUpsAccumulated = 0;

	int totalGarbageLevel = 0;

	bool garbageLevelHitZeroDirty = false;

	void onEnter() override;
	void onExit() override;
	void onUpdate(Timestep dt) override;
	bool onEvent(Ref<Event> event) override;
};