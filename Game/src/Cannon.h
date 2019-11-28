#pragma once

#include <Oyl3D.h>

#include "CustomEvents.h"
#include "CustomComponents.h"

using namespace oyl;

class CannonSystem : public System
{
private:
	OYL_CTOR(CannonSystem, System)
    
	void onEnter() override;
	void onExit() override;
	void onUpdate(Timestep dt) override;
	bool onEvent(Ref<Event> event) override;
    
	void updateFuse(float dt, Cannon* a_cannon);

	void fireCannon(Cannon* a_cannon);

    //state changing functions
	void changeToDoingNothing(Cannon* a_cannon);
	void changeToBeingPushed(Cannon* a_cannon);
	void changeToFiringSoon(Cannon* a_cannon);
};