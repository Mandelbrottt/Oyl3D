#include "CannonballSystem.h"

void CannonballSystem::onEnter()
{
	this->listenForEventCategory((EventCategory)CategoryCannon);
	this->listenForEventCategory((EventCategory)CategoryCannonball);
	
	blueMiddleSplines.push_back({ -3.03f, -2.68f, 2.33f });
	blueMiddleSplines.push_back({ -2.88f,  0.0f,  0.0f });
	blueMiddleSplines.push_back({ -2.39f,  4.03f, 9.94f });
	blueMiddleSplines.push_back({ -1.47f,  5.7f,  13.03f });
	blueMiddleSplines.push_back({ -0.5f,   5.37f, 16.08f });
	blueMiddleSplines.push_back({  0.23f,  4.54f, 19.19f });
	blueMiddleSplines.push_back({  0.69f,  2.8f,  21.92f });
	blueMiddleSplines.push_back({  1.03f,  0.23f, 23.91f });
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
			if (cannonball.currentSplineIndex)

			cannonball.interpolationParam = std::min(
				cannonball.interpolationParam + cannonball.interpolationSpeed * Time::deltaTime(),
				1.0f);
			
			glm::vec3 v1, v2, v3, v4;

			if (cannonball.currentSplineIndex == 0)
				v1 = cannonball.splineFollowedWhenFired[0];
			else
				v1 = cannonball.splineFollowedWhenFired[cannonball.currentSplineIndex - 1];

			v2 = cannonball.splineFollowedWhenFired[cannonball.currentSplineIndex];

			if (cannonball.currentSplineIndex == cannonball.splineFollowedWhenFired.size() - 1)
			{
				v3 = cannonball.splineFollowedWhenFired[cannonball.currentSplineIndex];
				v4 = cannonball.splineFollowedWhenFired[cannonball.currentSplineIndex];
			}
			else
			{
				v3 = cannonball.splineFollowedWhenFired[cannonball.currentSplineIndex + 1];

				if (cannonball.currentSplineIndex == cannonball.splineFollowedWhenFired.size() - 2)
					v4 = cannonball.splineFollowedWhenFired[cannonball.currentSplineIndex + 1];
				else
					v4 = cannonball.splineFollowedWhenFired[cannonball.currentSplineIndex + 2];
			}

			cannonballTransform.setPosition(glm::catmullRom(v1, v2, v3, v4, cannonball.interpolationParam));

			if (cannonball.interpolationParam >= 1.0f)
			{
				if (cannonball.currentSplineIndex >= cannonball.splineFollowedWhenFired.size() - 1)
					registry->destroy(cannonballEntity);
				else
				{
					cannonball.currentSplineIndex++;
					cannonball.interpolationParam = 0.0f;
				}
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
				auto& cannonball          = registry->get<Cannonball>(cannonballEntity);
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

			auto& cannon          = registry->get<Cannon>(evt.cannonEntity);
			auto& cannonTransform = registry->get<component::Transform>(evt.cannonEntity);
			
			auto cannonballView = registry->view<Cannonball, CarryableItem, component::Transform>();
			for (auto& cannonballEntity : cannonballView)
			{
				auto& cannonball          = registry->get<Cannonball>(cannonballEntity);
				auto& cannonballCarryable = registry->get<CarryableItem>(cannonballEntity);
				auto& cannonballTransform = registry->get<component::Transform>(cannonballEntity);

				if (cannonball.isWaitingToBeFired && cannonballCarryable.team == cannon.team)
				{
					cannonball.isBeingFired       = true;
					cannonball.isWaitingToBeFired = false;

					cannonball.currentSplineIndex = 0;

					if (cannon.cannonTrackPosition == 0)
					{
						if (cannon.team == Team::blue)
							cannonball.splineFollowedWhenFired = blueMiddleSplines;
						else
							cannonball.splineFollowedWhenFired = redMiddleSplines;
					}

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

	cannonball.isWaitingToBeFired = false;

	cannonballCarryable.isBeingCarried = true;
	cannonballCarryable.isActive       = true;

	std::cout << "OBTAINED CANNONBALL!\n";

	auto& cannonballParent  = registry->get_or_assign<component::Parent>(a_cannonballEntity);
	cannonballParent.parent = a_playerEntity;

	cannonballTransform.setRotationEuler(glm::vec3(0.0f));
	cannonballTransform.setPosition(glm::vec3(0.0f, 0.3f, -0.8f));
}