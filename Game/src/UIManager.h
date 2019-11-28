#pragma once
#include <Oyl3D.h>

#include "CustomEvents.h"
#include "CustomComponents.h"

using namespace oyl;

class UIManagerSystem : public System
{
private:
	OYL_CTOR(UIManagerSystem, System)

	void onEnter() override;
	void onExit() override;
	void onUpdate(Timestep dt) override;
	bool onEvent(Ref<Event> event) override;
};