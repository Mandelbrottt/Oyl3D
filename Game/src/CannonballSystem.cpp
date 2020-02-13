#include "CannonballSystem.h"

void CannonballSystem::onEnter()
{
	this->listenForEventCategory((EventCategory)CategoryCannon);
	this->listenForEventCategory((EventCategory)CategoryCannonball);

	initFiringSplines();
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
			cannonballTransform.rotate(glm::vec3(390.0f, 0.0f, 150.0f) * Time::deltaTime());

			if (cannonball.interpolationParam >= 1.0f)
			{
				if (cannonball.currentSplineIndex >= cannonball.splineFollowedWhenFired.size() - 3)
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

					std::cout << cannon.cannonTrackPosition << "\n";
					if (cannon.cannonTrackPosition == -1)
					{
						if (cannon.team == Team::blue)
							cannonball.splineFollowedWhenFired = blueFrontSplines;
						else
							cannonball.splineFollowedWhenFired = redFrontSplines;
					}
					else if (cannon.cannonTrackPosition == 0)
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

void CannonballSystem::initFiringSplines()
{
	blueFrontSplines.push_back({ -11.04f, -2.68f, 2.33f });
	blueFrontSplines.push_back({ -11.45f,  3.25f, 8.13f });
	blueFrontSplines.push_back({ -12.06f,  5.74f, 13.03f });
	blueFrontSplines.push_back({ -13.03f,  6.41f, 16.15f });
	blueFrontSplines.push_back({ -14.08f,  5.77f, 19.18f });
	blueFrontSplines.push_back({ -15.36f,  4.13f, 21.25f });
	blueFrontSplines.push_back({ -17.44f,  0.23f, 22.77f });
	blueFrontSplines.push_back({ -17.44f, -2.0f,  22.77f });

	blueMiddleSplines.push_back({ -3.03f, -2.68f, 2.33f });
	blueMiddleSplines.push_back({ -2.58f,  3.25f, 8.13f });
	blueMiddleSplines.push_back({ -2.1f,   5.74f, 12.3f });
	blueMiddleSplines.push_back({ -1.32f,  6.41f, 16.19f });
	blueMiddleSplines.push_back({ -0.54f,  5.77f, 19.46f });
	blueMiddleSplines.push_back({  0.34f,  4.13f, 21.92f });
	blueMiddleSplines.push_back({  1.03f,  0.23f, 23.91f });
	blueMiddleSplines.push_back({  1.03f,  -2.0f, 23.91f });
}