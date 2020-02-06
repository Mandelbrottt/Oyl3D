#pragma once
#include "CustomEvents.h"
#include "CustomComponents.h"

using namespace oyl;

class GarbageMeterSystem : public System
{
	OYL_CTOR(GarbageMeterSystem, System)

	void onEnter() override;
	void onExit() override;
	void onUpdate() override;
	bool onEvent(const Event& event) override;

	void updateMeterBarsForGarbagePile(entt::entity a_garbagePileEntity);
};