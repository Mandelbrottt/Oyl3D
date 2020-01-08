#include "Cannon.h"

void CannonSystem::onEnter()
{
	this->listenForEventCategory((OylEnum)CategoryCannon);
}

void CannonSystem::onExit()
{
}

void CannonSystem::onUpdate(Timestep dt)
{
	auto view = registry->view<Cannon, component::Transform>();
	for (auto& entity : view)
	{
		auto& cannon = registry->get<Cannon>(entity);
	    auto& cannonTransform = registry->get<component::Transform>(entity);
	    
	    //update fuse for every cannon
		updateFuse(dt, &cannon, &cannonTransform);

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
				cannon.waitBeforeBeingPushedCountdown -= dt;
				if (cannon.waitBeforeBeingPushedCountdown < 0.0f)
				{
					cannon.pushStateData.interpolationParam = std::min(
						cannon.pushStateData.interpolationParam + cannon.pushStateData.speed * dt,
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

bool CannonSystem::onEvent(Ref<Event> event)
{
	switch (event->type)
	{
	    case TypeCannonStateChange:
	    {
			auto evt = (CannonStateChangeEvent)* event;
	        switch (evt.newState)
	        {
			    case CannonState::beingPushed:
			    {
					changeToBeingPushed(evt.cannon);
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

void CannonSystem::updateFuse(float dt, Cannon* a_cannon, component::Transform* a_cannonTransform)
{
	a_cannon->fuseCountdown -= dt;

    //check if cannon is firing soon (if fuse timer <= time it takes to push the cannon)
    if (a_cannon->fuseCountdown <= (1.0f / a_cannon->beingPushedSpeed))
    {
        if (a_cannon->state == CannonState::doingNothing)
		    changeToFiringSoon(a_cannon);
        
		if (a_cannon->fuseCountdown < 0.0f)
		{
			a_cannon->fuseCountdown = a_cannon->FUSE_DURATION;
			changeToDoingNothing(a_cannon);
		    
			if (a_cannon->isLoaded)
				fireCannon(a_cannon, a_cannonTransform);
			else
				std::cout << "CANNON MISFIRE! (UNLOADED CANNON FIRED)\n";
		}
    }
}

void CannonSystem::fireCannon(Cannon* a_cannon, component::Transform* a_cannonTransform)
{
	std::cout << "CANNON FIRED!\n";
	a_cannon->isLoaded = false;
	changeToDoingNothing(a_cannon);

	CannonFiredEvent cannonFired;
	cannonFired.cannonPosition = a_cannonTransform->getPosition();
	postEvent(Event::create(cannonFired));
}
