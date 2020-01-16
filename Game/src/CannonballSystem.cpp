#include "CannonballSystem.h"

void CannonballSystem::onEnter()
{
	this->listenForEventCategory((EventCategory)CategoryCannon);
	this->listenForEventCategory((EventCategory)CategoryCannonball);
}

void CannonballSystem::onExit()
{

}

void CannonballSystem::onUpdate()
{
	auto view = registry->view<Cannonball, CarryableItem, component::Transform>();
	for (auto& cannonballEntity : view)
	{
		auto& cannonball          = registry->get<Cannonball>(cannonballEntity);
		auto& cannonballCarryable = registry->get<CarryableItem>(cannonballEntity);
		auto& cannonballTransform = registry->get<component::Transform>(cannonballEntity);

		if (cannonball.isBeingFired)
		{
			cannonball.interpolationParam = std::min(
				cannonball.interpolationParam + 0.7f * Time::deltaTime(),
				1.0f);
			
			cannonballTransform.setPosition(glm::catmullRom(
				cannonball.v1, 
				cannonball.v2, 
				cannonball.v3, 
				cannonball.v4, 
				cannonball.interpolationParam));

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

bool CannonballSystem::onEvent(const Event& event)
{
	switch (event.type)
	{
		case (EventType) TypeSpawnCannonballForPlayer:
		{
			auto evt = event_cast<SpawnCannonballForPlayerEvent>(event);

			bool isThereAnInactiveCannonball = false;
			entt::entity cannonballEntityToCopyFrom;

			auto cannonballView = registry->view<Cannonball, CarryableItem, component::Transform>();
			for (auto& cannonballEntity : cannonballView)
			{
				auto& cannonballCarryable = registry->get<CarryableItem>(cannonballEntity);

				if (cannonballCarryable.team == evt.team)
				{
					cannonballEntityToCopyFrom = cannonballEntity;

					if (!cannonballCarryable.isActive)
					{
						isThereAnInactiveCannonball = true;
						setCannonballToCarriedForPlayer(evt.playerEntity, cannonballEntity);

						break;
					}
				}
			}

			if (!isThereAnInactiveCannonball)
			{
				std::cout << registry->get<component::EntityInfo>(cannonballEntityToCopyFrom).name << "\n";

				auto newCannonballEntity = registry->create();
				registry->stomp(newCannonballEntity, cannonballEntityToCopyFrom, *registry);

				std::string teamNamePrefix = evt.team == Team::blue ? "Blue" : "Red";

				auto& cannonballEntityInfo = registry->get_or_assign<component::EntityInfo>(newCannonballEntity);
				cannonballEntityInfo.name  = teamNamePrefix + "Cannonball" + std::to_string(cannonballView.size());

				setCannonballToCarriedForPlayer(evt.playerEntity, newCannonballEntity);
			}

			break;
		}

		case (EventType) TypeCannonFired:
		{
			auto evt = event_cast<CannonFiredEvent>(event);
			
			auto cannonballView = registry->view<Cannonball, CarryableItem, component::Transform>();
			for (auto& cannonballEntity : cannonballView)
			{
				auto& cannonball          = registry->get<Cannonball>(cannonballEntity);
				auto& cannonballCarryable = registry->get<CarryableItem>(cannonballEntity);
				auto& cannonballTransform = registry->get<component::Transform>(cannonballEntity);

				if (!cannonball.isBeingFired && !cannonballCarryable.isActive)
				{
					cannonball.isBeingFired      = true;
					cannonballCarryable.isActive = true;

					cannonball.v1 = evt.cannonPosition + glm::vec3(0.0f, -1.0f, -.0f);
					cannonball.v2 = cannonball.v1 + glm::vec3(0.0f, 2.5f, -1.0f);
					cannonball.v3 = cannonball.v2 + glm::vec3(0.0f, 3.0f, -18.0f);
					cannonball.v4 = cannonball.v3 + glm::vec3(0.0f, -5.0f, -15.0f);

					break;
				}
			}

			break;
		}
	}

	return false;
}

void CannonballSystem::setCannonballToCarriedForPlayer(entt::entity a_playerEntity, entt::entity a_cannonballEntity)
{
	auto& player = registry->get<Player>(a_playerEntity);

	auto& cannonball          = registry->get<Cannonball>(a_cannonballEntity);
	auto& cannonballCarryable = registry->get<CarryableItem>(a_cannonballEntity);
	auto& cannonballTransform = registry->get<component::Transform>(a_cannonballEntity);

	//remove rigidbody when item is carried
	if (registry->has<component::RigidBody>(a_cannonballEntity))
		registry->remove<component::RigidBody>(a_cannonballEntity);

	player.primaryCarriedItem = a_cannonballEntity;

	cannonballCarryable.isBeingCarried = true;
	cannonballCarryable.isActive       = true;

	std::cout << "OBTAINED CANNONBALL!\n";

	auto& cannonballParent  = registry->get_or_assign<component::Parent>(a_cannonballEntity);
	cannonballParent.parent = a_playerEntity;

	cannonballTransform.setRotationEuler(glm::vec3(0.0f));
	cannonballTransform.setPosition(glm::vec3(0.0f, 0.3f, -0.8f));
}