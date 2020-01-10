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
	void onUpdate() override;
	bool onEvent(const Event& event) override;
    
	void updateFuse(float dt, Cannon* a_cannon, component::Transform* a_cannonTransform);

	void fireCannon(Cannon* a_cannon, component::Transform* a_cannonTransform);

    //state changing functions
	void changeToDoingNothing(Cannon* a_cannon);
	void changeToBeingPushed(Cannon* a_cannon);
	void changeToFiringSoon(Cannon* a_cannon);
};