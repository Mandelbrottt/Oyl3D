#include "CannonSystem.h"

void CannonSystem::onEnter()
{
	this->listenForEventCategory((EventCategory)CategoryCannon);
}

void CannonSystem::onExit()
{
}

void CannonSystem::onUpdate()
{
	auto view = registry->view<Cannon, component::Transform>();
	for (auto& cannonEntity : view)
	{
		auto& cannon          = registry->get<Cannon>(cannonEntity);
	    auto& cannonTransform = registry->get<component::Transform>(cannonEntity);
	    
	    //update fuse for every cannon
		updateFuse(Time::deltaTime(), cannonEntity);

		switch (cannon.state)
		{
		    case CannonState::doingNothing:
			{

				break;
			}
		    
			case CannonState::firingSoon:
		    {

				break;
		    }
		    
			case CannonState::beingPushed:
		    {
		        //wait before pushing (so the player can have the position adjusted)
				cannon.waitBeforeBeingPushedCountdown -= Time::deltaTime();
				if (cannon.waitBeforeBeingPushedCountdown < 0.0f)
				{
					cannon.pushStateData.interpolationParam = std::min(
						cannon.pushStateData.interpolationParam + cannon.pushStateData.speed * Time::deltaTime(),
						1.0f);

					cannonTransform.setPosition(glm::mix(cannon.pushStateData.startPos, 
						cannon.pushStateData.destinationPos, 
						cannon.pushStateData.interpolationParam));

					if (cannon.pushStateData.interpolationParam >= 1.0f)
						changeToDoingNothing(&cannon);
				}
		        
				break;
		    }
		}
	}
}

bool CannonSystem::onEvent(const Event& event)
{
	switch (event.type)
	{
		case (EventType)TypeCannonStateChange:
	    {
			auto evt = event_cast<CannonStateChangeEvent>(event);
			auto& cannon = registry->get<Cannon>(evt.cannonEntity);

	        switch (evt.newState)
	        {
			    case CannonState::beingPushed:
			    {
					changeToBeingPushed(&cannon);
				    break;
			    }
	        }

			break;
	    }
	}
    
	return false;
}

void CannonSystem::changeToDoingNothing(Cannon* a_cannon)
{
	a_cannon->state = CannonState::doingNothing;
}

//destination and start positions for adjusting and pushing should be set before calling this function
void CannonSystem::changeToBeingPushed(Cannon* a_cannon)
{    
	a_cannon->waitBeforeBeingPushedCountdown = a_cannon->WAIT_BEFORE_BEING_PUSHED_DURATION;
    
	a_cannon->pushStateData.interpolationParam = 0.0f;
	a_cannon->pushStateData.speed = a_cannon->beingPushedSpeed;
    
	a_cannon->state = CannonState::beingPushed;
}

void CannonSystem::changeToFiringSoon(Cannon* a_cannon)
{
	a_cannon->state = CannonState::firingSoon;

	std::cout << "FIRING SOON!\n";
}

void CannonSystem::updateFuse(float dt, entt::entity a_cannonEntity)
{
	auto& cannon          = registry->get<Cannon>(a_cannonEntity);
	auto& cannonTransform = registry->get<component::Transform>(a_cannonEntity);

	cannon.fuseCountdown -= dt;

    //check if cannon is firing soon (if fuse timer <= time it takes to push the cannon)
    if (cannon.fuseCountdown <= (1.0f / cannon.beingPushedSpeed))
    {
        if (cannon.state == CannonState::doingNothing)
		    changeToFiringSoon(&cannon);
        
		if (cannon.fuseCountdown < 0.0f)
		{
			cannon.fuseCountdown = cannon.FUSE_DURATION;
			changeToDoingNothing(&cannon);
		    
			fireCannon(a_cannonEntity);
		}
    }
}

void CannonSystem::fireCannon(entt::entity a_cannonEntity)
{
	auto& cannon = registry->get<Cannon>(a_cannonEntity);

	if (cannon.isLoaded)
	{
		std::cout << "CANNON FIRED!\n";
		cannon.isLoaded = false;
		changeToDoingNothing(&cannon);

		CannonFiredEvent cannonFired;
		cannonFired.cannonEntity = a_cannonEntity;
		postEvent(cannonFired);
	}
	else
	{
		std::cout << "CANNON MISFIRE! (UNLOADED CANNON FIRED)\n";
	}
}
