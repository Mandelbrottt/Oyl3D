#include "CannonballSystem.h"

void CannonballSystem::onEnter()
{
	listenForEventCategory((EventCategory)CategoryCannon);
	listenForEventCategory((EventCategory)CategoryCannonball);

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
			
			if (cannonballCarryable.team == Team::blue)
				cannonballTransform.rotate(glm::vec3(410.0f, 0.0f, 160.0f) * Time::deltaTime());
			else
				cannonballTransform.rotate(glm::vec3(-410.0f, 0.0f, -160.0f) * Time::deltaTime());

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

					if (cannon.cannonTrackPosition == -1)
					{
						if (cannon.team == Team::blue)
							cannonball.splineFollowedWhenFired = blueFrontSpline;
						else
							cannonball.splineFollowedWhenFired = redFrontSpline;
					}
					else if (cannon.cannonTrackPosition == 0)
					{
						if (cannon.team == Team::blue)
							cannonball.splineFollowedWhenFired = blueMiddleSpline;
						else
							cannonball.splineFollowedWhenFired = redMiddleSpline;
					}
					else //cannon track position = 1
					{
						if (cannon.team == Team::blue)
							cannonball.splineFollowedWhenFired = blueBackSpline;
						else
							cannonball.splineFollowedWhenFired = redBackSpline;
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

	player.primaryCarriedItem   = a_cannonballEntity;
	player.secondaryCarriedItem = a_cannonballEntity;

	cannonball.isWaitingToBeFired = false;

	cannonballCarryable.isBeingCarried = true;
	cannonballCarryable.isActive       = true;

	OYL_LOG("OBTAINED CANNONBALL!");

	cannonballTransform.setParent(a_playerEntity);

	cannonballTransform.setRotationEuler(glm::vec3(0.0f));
	cannonballTransform.setPosition(glm::vec3(0.0f, 0.3f, -0.8f));
}

void CannonballSystem::initFiringSplines()
{
	//blue side
	blueFrontSpline.push_back({ -11.04f, -2.68f, 2.33f });
	blueFrontSpline.push_back({ -11.45f,  3.25f, 8.13f });
	blueFrontSpline.push_back({ -12.06f,  5.74f, 13.03f });
	blueFrontSpline.push_back({ -13.03f,  6.41f, 16.15f });
	blueFrontSpline.push_back({ -14.08f,  5.77f, 19.18f });
	blueFrontSpline.push_back({ -15.36f,  4.13f, 21.25f });
	blueFrontSpline.push_back({ -17.44f,  0.23f, 22.77f });
	blueFrontSpline.push_back({ -17.44f, -2.0f,  22.77f });

	blueMiddleSpline.push_back({ -3.03f, -2.68f, 2.33f });
	blueMiddleSpline.push_back({ -2.58f,  3.25f, 8.13f });
	blueMiddleSpline.push_back({ -2.1f,   5.74f, 12.3f });
	blueMiddleSpline.push_back({ -1.32f,  6.41f, 16.19f });
	blueMiddleSpline.push_back({ -0.54f,  5.77f, 19.46f });
	blueMiddleSpline.push_back({  0.34f,  4.13f, 21.92f });
	blueMiddleSpline.push_back({  1.03f,  0.23f, 23.91f });
	blueMiddleSpline.push_back({  1.03f,  -2.0f, 23.91f });

	blueBackSpline.push_back({ 5.0f,  -2.68f, 2.33f });
	blueBackSpline.push_back({ 6.34f,  4.5f,  8.44f });
	blueBackSpline.push_back({ 7.92f,  7.16f, 13.78f });
	blueBackSpline.push_back({ 9.57f,  8.09f, 17.33f });
	blueBackSpline.push_back({ 11.33f, 8.29f, 19.83f });
	blueBackSpline.push_back({ 12.87f, 7.02f, 21.92f });
	blueBackSpline.push_back({ 13.85f, 4.33f, 22.99f });
	blueBackSpline.push_back({ 13.85f, 2.33f, 22.99f });

	//red side
	redFrontSpline.push_back({ -11.04f, -2.68f,  18.06f });
	redFrontSpline.push_back({ -11.45f,  3.25f,  12.26f });
	redFrontSpline.push_back({ -12.06f,  5.95f,  7.36f });
	redFrontSpline.push_back({ -13.03f,  6.41f,  4.24f });
	redFrontSpline.push_back({ -14.08f,  5.77f,  1.18f });
	redFrontSpline.push_back({ -15.36f,  4.13f, -0.88f });
	redFrontSpline.push_back({ -17.44f,  0.23f, -2.59f });
	redFrontSpline.push_back({ -17.44f, -2.0f,  -2.59f });

	redMiddleSpline.push_back({ -3.03f, -2.68f,  18.09f });
	redMiddleSpline.push_back({ -2.58f,  3.25f,  12.29f });
	redMiddleSpline.push_back({ -2.1f,   5.74f,  8.12f });
	redMiddleSpline.push_back({ -1.32f,  6.41f,  4.07f, });
	redMiddleSpline.push_back({ -0.54f,  5.77f,  0.81f, });
	redMiddleSpline.push_back({  0.34f,  4.13f, -1.65f });
	redMiddleSpline.push_back({  0.89f,  0.2f,  -3.79f });
	redMiddleSpline.push_back({  0.89f, -1.8f,  -3.79f });

	redBackSpline.push_back({ 4.91f, -2.68f,  18.09f });
	redBackSpline.push_back({ 6.34f,  4.5f,   11.98f });
	redBackSpline.push_back({ 7.92f,  7.16f,  6.59f });
	redBackSpline.push_back({ 9.57f,  8.09f,  3.06f });
	redBackSpline.push_back({ 11.33f, 8.29f,  0.54f });
	redBackSpline.push_back({ 12.87f, 7.02f, -1.53f });
	redBackSpline.push_back({ 13.85f, 4.23f, -2.67f });
	redBackSpline.push_back({ 13.85f, 2.33f, -2.67f });
}