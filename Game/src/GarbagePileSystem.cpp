#include "GarbagePileSystem.h"

void GarbagePileSystem::onEnter()
{
	listenForEventCategory((EventCategory)CategoryGarbagePile);
	listenForEventCategory((EventCategory)CategoryCannon);

	bluePassiveBuildupWait = 16.0f;
	redPassiveBuildupWait  = 16.0f;

	bluePassiveBuildupCountdown = bluePassiveBuildupWait;
	redPassiveBuildupCountdown  = redPassiveBuildupWait;
}

void GarbagePileSystem::onExit()
{

}

void GarbagePileSystem::onUpdate()
{
	bool addBlueGarbageLevel = false;
	bool addRedGarbageLevel  = false;

	bluePassiveBuildupCountdown -= Time::deltaTime();
	redPassiveBuildupCountdown  -= Time::deltaTime();

	int blueRandomGarbagePileNum = -5; //initialize to invalid
	int redRandomGarbagePileNum  = -5; //initialize to invalid

	if (bluePassiveBuildupCountdown < 0.0f)
	{
		addBlueGarbageLevel = true;
		bluePassiveBuildupWait -= 0.25f;
		bluePassiveBuildupCountdown = bluePassiveBuildupWait;

		//ensure we don't divide by zero
		if (blueActivePileNums.size() > 0)
			blueRandomGarbagePileNum = blueActivePileNums[rand() % blueActivePileNums.size()];
	}

	if (redPassiveBuildupCountdown < 0.0f)
	{
		addRedGarbageLevel = true;
		redPassiveBuildupWait -= 0.25f;
		redPassiveBuildupCountdown = redPassiveBuildupWait;

		//ensure we don't divide by zero
		if (redActivePileNums.size() > 0)
			redRandomGarbagePileNum = redActivePileNums[rand() % redActivePileNums.size()];
	}

	auto garbagePileView = registry->view<GarbagePile, component::Renderable, component::Transform>();
	for (auto& garbagePileEntity : garbagePileView)
	{
		auto& garbagePile           = registry->get<GarbagePile>(garbagePileEntity);
		auto& garbagePileRenderable = registry->get<component::Renderable>(garbagePileEntity);
		auto& garbagePileTransform  = registry->get<component::Transform>(garbagePileEntity);

		if (addBlueGarbageLevel && garbagePile.team == Team::blue && garbagePile.relativePositionOnShip == blueRandomGarbagePileNum)
			increaseGarbageLevel(garbagePileEntity);
		else if (addRedGarbageLevel && garbagePile.team == Team::red && garbagePile.relativePositionOnShip == redRandomGarbagePileNum)
			increaseGarbageLevel(garbagePileEntity);

		if (garbagePile.delayBeforeAddingGarbageCountdown > 0.0f)
		{
			garbagePile.delayBeforeAddingGarbageCountdown -= Time::deltaTime();

			if (garbagePile.delayBeforeAddingGarbageCountdown < 0.0f)
				increaseGarbageLevel(garbagePileEntity);
		}
	}
}

bool GarbagePileSystem::onEvent(const Event& event)
{
	switch (event.type)
	{
	case (EventType)TypeRequestToCleanGarbage:
	{
		auto evt = event_cast<RequestToCleanGarbageEvent>(event);

		auto& garbagePile = registry->get<GarbagePile>(evt.garbagePileEntity);

		decreaseGarbageLevel(evt.garbagePileEntity);

		GarbageCleanedEvent garbageCleaned;
		garbageCleaned.garbagePileEntity = evt.garbagePileEntity;
		postEvent(garbageCleaned);

		break;
	}

	case (EventType)TypeGarbageGlooped:
	{
		auto evt = event_cast<GarbageGloopedEvent>(event);

		auto& garbagePile     = registry->get<GarbagePile>(evt.garbagePileEntity);
		garbagePile.isGlooped = true;

		updateGarbagePileVisualSize(evt.garbagePileEntity);

		break;
	}

	case (EventType)TypeCannonFired:
	{
		auto evt = event_cast<CannonFiredEvent>(event);
		auto& cannon = registry->get<Cannon>(evt.cannonEntity);

		int garbagePileFiredAt = cannon.trackPosition * -1; //flip the sign since we're on the other ship (cannon at -1 points at opposite ship's garbage pile at 1)

		auto view = registry->view<GarbagePile, component::Transform>();
		for (auto& garbagePileEntity : view)
		{
			auto& garbagePile = registry->get<GarbagePile>(garbagePileEntity);

			if (garbagePile.relativePositionOnShip == garbagePileFiredAt && garbagePile.team != cannon.team)
			{
				garbagePile.delayBeforeAddingGarbageCountdown = garbagePile.DELAY_BEFORE_ADDING_GARBAGE_DURATION;
				break;
			}
		}
		break;
	}

	case (EventType)TypeSetMaxGarbageLevel:
	{
		auto view = registry->view<GarbagePile, component::Transform>();
		for (auto& garbagePileEntity : view)
		{
			auto& garbagePile = registry->get<GarbagePile>(garbagePileEntity);

			if (garbagePile.garbageLevel > 3)
			{
				garbagePile.garbageLevel = 3;
				updateGarbagePileVisualSize(garbagePileEntity);
			}
		}

		bluePassiveBuildupWait = 16.0f;
		redPassiveBuildupWait = 16.0f;

		break;
	}
	}

	return false;
}

void GarbagePileSystem::increaseGarbageLevel(entt::entity a_garbagePileEntity)
{
	auto& garbagePile = registry->get<GarbagePile>(a_garbagePileEntity);

	//if garbage pile is already maxed, it wasn't maxed out this iteration
	bool isMaxedOutThisIteration = (garbagePile.garbageLevel != garbagePile.MAX_GARBAGE_LEVEL || garbagePile.garbageTicks != garbagePile.GARBAGE_TICKS_PER_LEVEL);

	if (garbagePile.garbageLevel == 0)
	{
		garbagePile.garbageLevel++;
		garbagePile.garbageTicks = garbagePile.GARBAGE_TICKS_PER_LEVEL;

		GarbagePileReappearedEvent garbageReappeared;
		garbageReappeared.garbagePileEntity = a_garbagePileEntity;
		postEvent(garbageReappeared);
	}
	else if (garbagePile.garbageLevel < garbagePile.MAX_GARBAGE_LEVEL)
		garbagePile.garbageLevel++;
	else //garbage level == MAX
		garbagePile.garbageTicks = garbagePile.GARBAGE_TICKS_PER_LEVEL;

	//check if garbage pile was maxed out after the garbage was just added
	if (garbagePile.garbageLevel == garbagePile.MAX_GARBAGE_LEVEL && garbagePile.garbageTicks == garbagePile.GARBAGE_TICKS_PER_LEVEL && isMaxedOutThisIteration)
	{
		if (garbagePile.team == Team::blue)
		{
			blueActivePileNums.erase(
				std::remove(blueActivePileNums.begin(), blueActivePileNums.end(), garbagePile.relativePositionOnShip),
				blueActivePileNums.end());

			bluePassiveBuildupWait += 5.5f; //add some more time every time a pile maxes out to prevent quick snowballing
		}
		else //team == red
		{
			redActivePileNums.erase(
				std::remove(redActivePileNums.begin(), redActivePileNums.end(), garbagePile.relativePositionOnShip),
				redActivePileNums.end());

			redPassiveBuildupWait += 5.5f; //add some more time every time a pile maxes out to prevent quick snowballing
		}
	}

	updateGarbagePileVisualSize(a_garbagePileEntity); //update size whenever garbage is added

	IncreasedGarbageLevelEvent increasedGarbageLevel;
	increasedGarbageLevel.garbagePileEntity = a_garbagePileEntity;
	postEvent(increasedGarbageLevel);
}

void GarbagePileSystem::decreaseGarbageLevel(entt::entity a_garbagePileEntity)
{
	auto& garbagePile = registry->get<GarbagePile>(a_garbagePileEntity);

	if (garbagePile.garbageTicks <= garbagePile.GARBAGE_TICKS_PER_LEVEL * 0.75f)
		garbagePile.garbageTicks -= garbagePile.isGlooped ? 0.04f : 0.08f;
	else //garbage ticks >= 75% of the max
		garbagePile.garbageTicks -= garbagePile.isGlooped ? 0.5f : 1.0f;

	if (garbagePile.garbageTicks <= 0.0f)
	{
		garbagePile.garbageLevel--;
		if (garbagePile.garbageLevel > 0)
			garbagePile.garbageTicks = garbagePile.GARBAGE_TICKS_PER_LEVEL;
		else //garbagePile.garbageLevel == 0
			garbagePile.garbageTicks = 0;

		garbagePile.isGlooped = false;

		updateGarbagePileVisualSize(a_garbagePileEntity);
	}
}

void GarbagePileSystem::updateGarbagePileVisualSize(entt::entity a_garbagePileEntity)
{
	auto& garbagePile			 = registry->get<GarbagePile>(a_garbagePileEntity);
	auto& garbagePileCollidable  = registry->get<component::Collidable>(a_garbagePileEntity);
	auto& garbagePileRenderable	 = registry->get<component::Renderable>(a_garbagePileEntity);

	//auto& flyAnimator   = registry->get<component::Animatable>(garbagePile.flyEntity);
	//auto& flyRenderable = registry->get<component::Renderable>(garbagePile.flyEntity);

	//reset collider sizes (the ones that need to be set will be set just below, based on garbage pile size)
	glm::vec3 garbagePileCollider0Pos = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 garbagePileCollider1Pos = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 garbagePileCollider2Pos = glm::vec3(0.0f, 0.0f, 0.0f);

	glm::vec3 garbagePileCollider0Size = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 garbagePileCollider1Size = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 garbagePileCollider2Size = glm::vec3(0.0f, 0.0f, 0.0f);

	std::string garbageModelNameToSet;

	//determine which mesh to set based on garbage level
	if (garbagePile.garbageLevel == garbagePile.MAX_GARBAGE_LEVEL)
	{
		if (garbagePile.garbageTicks == garbagePile.GARBAGE_TICKS_PER_LEVEL)
		{
			garbageModelNameToSet = "garbageSurrender";

			garbagePileCollider0Pos = glm::vec3(2.25f, 0.2f, 1.0f);
			garbagePileCollider1Pos = glm::vec3(-3.26f, 1.09f, 1.07f);
			garbagePileCollider2Pos = glm::vec3(0.12f, 1.87f, -1.25f);

			garbagePileCollider0Size = glm::vec3(8.47f, 1.42f, 3.57f);
			garbagePileCollider1Size = glm::vec3(10.95f, 2.5f, 4.71f);
			garbagePileCollider2Size = glm::vec3(3.47f, 3.94f, 4.03f);
		}
		else
		{
			garbageModelNameToSet = "garbageMassive";

			garbagePileCollider0Pos = glm::vec3(0.1f, 0.85f, -0.09f);
			garbagePileCollider1Pos = glm::vec3(0.31f, 2.75f, -0.56f);

			garbagePileCollider0Size = glm::vec3(7.57f, 2.02f, 7.29f);
			garbagePileCollider1Size = glm::vec3(3.05f, 2.3f, 4.03f);
		}
	}
	else if (garbagePile.garbageLevel == garbagePile.MAX_GARBAGE_LEVEL - 1)
	{
		garbageModelNameToSet = "garbageLarge";

		garbagePileCollider0Pos = glm::vec3(0.09f, 0.52f, 0.07f);
		garbagePileCollider1Pos = glm::vec3(0.59f, 1.82f, -0.45f);

		garbagePileCollider0Size = glm::vec3(5.43f, 1.24f, 5.35f);
		garbagePileCollider1Size = glm::vec3(2.15f, 1.42f, 2.63f);
	}
	else if (garbagePile.garbageLevel == garbagePile.MAX_GARBAGE_LEVEL - 2)
	{
		garbageModelNameToSet = "garbageMedium";

		garbagePileCollider0Pos = glm::vec3(0.0f, 0.18f, 0.0f);
		garbagePileCollider1Pos = glm::vec3(0.11f, 0.76f, -0.13f);

		garbagePileCollider0Size = glm::vec3(4.43f, 1.0f, 4.41f);
		garbagePileCollider1Size = glm::vec3(2.91f, 1.0f, 2.97f);
	}
	else if (garbagePile.garbageLevel == garbagePile.MAX_GARBAGE_LEVEL - 3)
	{
		garbageModelNameToSet = "garbageSmall";

		garbagePileCollider0Pos = glm::vec3(0.0f, 0.13f, 0.0f);

		garbagePileCollider0Size = glm::vec3(3.13f, 0.92f, 3.27f);
	}
	else
	{
		garbageModelNameToSet = "garbageTiny";

		garbagePileCollider0Pos = glm::vec3(0.0f, 0.06f, 0.0f);

		garbagePileCollider0Size = glm::vec3(2.35f, 0.52f, 2.37f);
	}

	garbagePileCollidable.getShape(0).box.setCenter(garbagePileCollider0Pos);
	garbagePileCollidable.getShape(1).box.setCenter(garbagePileCollider1Pos);
	garbagePileCollidable.getShape(2).box.setCenter(garbagePileCollider2Pos);

	garbagePileCollidable.getShape(0).box.setSize(garbagePileCollider0Size);
	garbagePileCollidable.getShape(1).box.setSize(garbagePileCollider1Size);
	garbagePileCollidable.getShape(2).box.setSize(garbagePileCollider2Size);

	garbagePileRenderable.model = Model::get(garbageModelNameToSet);

	if (garbagePile.isGlooped)
		garbagePileRenderable.material = Material::get("garbageGlooped");
	else
		garbagePileRenderable.material = Material::get("garbage");

	if (garbagePile.garbageLevel == 0)
	{
		garbagePileRenderable.enabled = false;
		//flyRenderable.enabled         = false;

		garbagePileCollidable.getShape(0).box.setSize(glm::vec3(0.0f, 0.0f, 0.0f));
	}
	else
	{
		garbagePileRenderable.enabled = true;
		//flyRenderable.enabled         = true;
	}
}