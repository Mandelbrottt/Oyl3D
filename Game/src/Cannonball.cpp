#include "Cannonball.h"

void CannonballSystem::onEnter()
{
	this->listenForEventCategory((OylEnum)CategoryCannon);
}

void CannonballSystem::onExit()
{

}

void CannonballSystem::onUpdate(Timestep dt)
{
	auto view = registry->view<Cannonball, CarryableItem, component::Transform>();
	for (auto& cannonballEntity : view)
	{
		auto& cannonball = registry->get<Cannonball>(cannonballEntity);
		auto& cannonballCarryable = registry->get<CarryableItem>(cannonballEntity);
		auto& cannonballTransform = registry->get<component::Transform>(cannonballEntity);

		if (cannonball.isBeingFired)
		{
			cannonball.interpolationParam = std::min(cannonball.interpolationParam + 0.7f * dt,
				1.0f);
			
			cannonballTransform.setPosition(glm::catmullRom(cannonball.v1, cannonball.v2, cannonball.v3, cannonball.v4, cannonball.interpolationParam));

			if (cannonball.interpolationParam >= 1.0f)
			{
				cannonball.interpolationParam = 0.0f;
				cannonball.isBeingFired = false;
				cannonballCarryable.isActive = false;

				cannonballTransform.setPosition(glm::vec3(-1000.0f));
			}
		}
	}
}

bool CannonballSystem::onEvent(Ref<Event> event)
{
	switch (event->type)
	{
		case TypeCannonFired:
		{
			auto evt = (CannonFiredEvent)* event;
			
			auto view = registry->view<Cannonball, CarryableItem, component::Transform>();
			for (auto& cannonballEntity : view)
			{
				auto& cannonball = registry->get<Cannonball>(cannonballEntity);
				auto& cannonballCarryable = registry->get<CarryableItem>(cannonballEntity);
				auto& cannonballTransform = registry->get<component::Transform>(cannonballEntity);

				if (!cannonball.isBeingFired && !cannonballCarryable.isActive)
				{
					cannonball.isBeingFired = true;
					cannonballCarryable.isActive = true;

					cannonball.v1 = evt.cannonPosition + glm::vec3(0.0f, -2.0f, 0.0f);
					cannonball.v2 = cannonball.v1 + glm::vec3(0.0f, 2.5f, 1.0f);
					cannonball.v3 = cannonball.v2 + glm::vec3(0.0f, 3.0f, 18.0f);
					cannonball.v4 = cannonball.v3 + glm::vec3(0.0f, -5.0f, 15.0f);

					break;
				}
			}

			break;
		}
	}

	return false;
}