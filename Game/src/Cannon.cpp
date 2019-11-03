#include "Cannon.h"

glm::vec3 lerp(const glm::vec3& v1, const glm::vec3& v2, float u)
{
	return (1.0f - u) * v1 + v2 * u;
}

void CannonSystem::onEnter()
{
	this->addToCategoryMask(CategoryCannon);
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
		updateFuse(dt, &cannon);

	    if (cannon.state == CannonStates::doingNothing)
	    {
	        
	    }
		else if (cannon.state == CannonStates::firingSoon)
		{
		    
		}
		else if (cannon.state == CannonStates::beingPushed)
		{		    
			cannon.pushStateData.interpolationParam = std::min(cannon.pushStateData.interpolationParam + cannon.pushStateData.speed * dt, 
				                                               1.0f);

			cannonTransform.position = lerp(cannon.pushStateData.startPos, cannon.pushStateData.desinationPos, cannon.pushStateData.interpolationParam);
			
		    if (cannon.pushStateData.interpolationParam >= 1.0f)
				changeToDoingNothing(&cannon);
		}
		else if (cannon.state == CannonStates::waiting)
		{
			cannon.waitTimer.timer += dt;
			if (cannon.waitTimer.timer > cannon.waitTimer.timeToWait)
			{
			    //TODO: destination not hard coded. check which side of the cannon the player is on and incorporate the rotation of the cannon to determine destination
				changeToBeingPushed(cannonTransform.position + glm::vec3(15.0f, 0.0f, 0.0f), &cannon, &cannonTransform);
				cannon.waitTimer.timer = 0.0f;
			}
		}
	}
}

bool CannonSystem::onEvent(Ref<Event> event)
{
	auto view = registry->view<Cannon, component::Transform>();
	for (auto& entity : view)
	{
		auto& cannon = registry->get<Cannon>(entity);
		auto& cannonTransform = registry->get<component::Transform>(entity);
	    
		switch (event->type)
		{
		    case TypeCannonInteract:
		    {
				auto evt = (CannonInteractEvent)* event;
				if (evt.cannon == entity)
				{
					if (cannon.isPushingValid)
						changeToWaiting(0.3f, &cannon);
					else if (cannon.isLoadingCannonballValid)
						loadCannonball(&cannon);
				}
		    }
		}
	}
    
	return false;
}

void CannonSystem::changeToDoingNothing(Cannon* a_cannon)
{
	a_cannon->state = CannonStates::doingNothing;
}

void CannonSystem::changeToWaiting(float a_timeToWait, Cannon* a_cannon)
{
	a_cannon->waitTimer.timeToWait = a_timeToWait;
	a_cannon->state = CannonStates::waiting;
}

void CannonSystem::changeToBeingPushed(glm::vec3 a_destinationPos, Cannon* a_cannon, const component::Transform* a_cannonTransform)
{    
	a_cannon->pushStateData.interpolationParam = 0.0f;
    
	a_cannon->pushStateData.startPos      = a_cannonTransform->position;
	a_cannon->pushStateData.desinationPos = a_destinationPos;
	a_cannon->state = CannonStates::beingPushed;
}

void CannonSystem::changeToFiringSoon(Cannon* a_cannon)
{
	a_cannon->state = CannonStates::firingSoon;
}

void CannonSystem::updateFuse(float dt, Cannon* a_cannon)
{
	a_cannon->fuse.timer += dt;

    //check if cannon is firing soon (if fuse timer >= (time between firing - time it takes to push the cannon)
    if (a_cannon->fuse.timer >= (a_cannon->fuse.timeToWait - (1.0f / a_cannon->pushStateData.speed)))
    {
		changeToFiringSoon(a_cannon);
        
		if (a_cannon->fuse.timer >= a_cannon->fuse.timeToWait)
		{
			loadCannonball(a_cannon); //TODO: get rid of this once cannon loading is implemented
		    
			a_cannon->fuse.timer = 0.0f;
			changeToDoingNothing(a_cannon);
		    
		    if (a_cannon->isLoaded)
			    fireCannon(a_cannon);
		}
    }
}

void CannonSystem::loadCannonball(Cannon* a_cannon)
{
	a_cannon->isLoaded = true;
}

void CannonSystem::fireCannon(Cannon* a_cannon)
{
	std::cout << "CANNON FIRED!\n";
}
