#pragma once
#include "CustomEvents.h"
#include "CustomComponents.h"

using namespace oyl;

class GarbagePileSystem : public System
{
	OYL_CTOR(GarbagePileSystem, System)

	float bluePassiveBuildupWait = 13.0f;
	float redPassiveBuildupWait  = 13.0f;

	float bluePassiveBuildupCountdown = bluePassiveBuildupWait;
	float redPassiveBuildupCountdown  = redPassiveBuildupWait;

	std::vector<int> blueActivePileNums{ -1, 0, 1 };
	std::vector<int> redActivePileNums{ -1, 0, 1 };

	void onEnter() override;
	void onExit() override;
	void onUpdate() override;
	bool onEvent(const Event& event) override;

	void increaseGarbageLevel(entt::entity a_garbagePileEntity);
	void decreaseGarbageLevel(entt::entity a_garbagePileEntity);
	void updateGarbagePileVisualSize(entt::entity a_garbagePileEntity);
};