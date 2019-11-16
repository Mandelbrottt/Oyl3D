#pragma once
#include "Team.h"
#include "CustomEvents.h"
#include "CustomComponents.h"

using namespace oyl;

/////////// EVENTS ///////////
OYL_EVENT_STRUCT(CannonInteractEvent, TypeCannonInteract, CategoryCannon,
	             {
		             entt::entity cannon;
                     //TODO: add player entity
	             });
//////////////////////////////

/////////// COMPONENTS ///////////
enum class CannonStates
{
	doingNothing,
	waiting,
    beingPushed,
    firingSoon
};

struct Cannon
{
	Teams team;
    
	CannonStates state = CannonStates::doingNothing;
	bool isLoaded = false;
    
	TimedAction fuse;
	MoveableUsingLerp pushStateData;
	TimedAction waitTimer;

	bool isPushingValid           = true; //TODO: change to false initially, GUI will check validation for pushing and set this flag
	bool isLoadingCannonballValid = true; //TODO: change to false initially, GUI will check validation for loading with cannonball and set this flag
};
////////////////////////////////


/////////// SYSTEMS ///////////
class CannonSystem : public System
{
	OYL_CTOR(CannonSystem, System)
    
	void onEnter() override;
	void onExit() override;
	void onUpdate(Timestep dt) override;
	bool onEvent(Ref<Event> event) override;

    //state changing functions
	void changeToDoingNothing(Cannon* a_cannon);
	void changeToWaiting(float a_timeToWait, Cannon* a_cannon);
	void changeToBeingPushed(glm::vec3 a_destinationPos, Cannon* a_cannon, const component::Transform* a_cannonTransform);
	void changeToFiringSoon(Cannon* a_cannon);
    
	void updateFuse(float dt, Cannon* a_cannon);

	void loadCannonball(Cannon* a_cannon);
	void fireCannon(Cannon* a_cannon);
};
///////////////////////////////