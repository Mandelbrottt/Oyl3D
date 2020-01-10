#pragma once
#include "CustomEvents.h"
#include "CustomComponents.h"

using namespace oyl;

class CleaningQuicktimeEventSystem : public System
{
	OYL_CTOR(CleaningQuicktimeEventSystem, System)

	void onEnter() override;
	void onExit() override;
	void onUpdate(Timestep dt) override;
	bool onEvent(Ref<Event> event) override;
};