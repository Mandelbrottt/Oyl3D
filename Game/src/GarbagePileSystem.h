#pragma once
#include "CustomEvents.h"
#include "CustomComponents.h"

using namespace oyl;

class GarbagePileSystem : public System
{
	OYL_CTOR(GarbagePileSystem, System)

	const float PASSIVE_GARBAGE_BUILDUP_TIME = 15.0f;
	float passiveGarbageBuildupCountdown = PASSIVE_GARBAGE_BUILDUP_TIME;
	int numBuildUpsAccumulated = 0;

	int totalGarbageLevel = 0;

	void onEnter() override;
	void onExit() override;
	void onUpdate() override;
	bool onEvent(const Event& event) override;

	void increaseGarbageLevel(entt::entity a_garbagePileEntity);
};