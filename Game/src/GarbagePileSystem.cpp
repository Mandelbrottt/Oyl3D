#include "GarbagePileSystem.h"

void GarbagePileSystem::onEnter()
{
	this->listenForEventCategory((EventCategory)CategoryGarbagePile);
	this->listenForEventCategory((EventCategory)CategoryCannon);
}

void GarbagePileSystem::onExit()
{

}

void GarbagePileSystem::onUpdate()
{
	//TODO: change to add to a random garbage pile rather than all of them for the final version
	bool addGarbageLevel = false;
	passiveGarbageBuildupCountdown -= Time::deltaTime();
	if (passiveGarbageBuildupCountdown < 0.0f)
	{
		addGarbageLevel = true;
		passiveGarbageBuildupCountdown = PASSIVE_GARBAGE_BUILDUP_TIME - numBuildUpsAccumulated * 0.1f; //TODO: come up with a better equation for time scaling
		numBuildUpsAccumulated++;
	}

	int lastFrameTotalGarbageLevel = totalGarbageLevel; //get the last frame's total garbage level so we can compare them later
	totalGarbageLevel = 0; //reset the total garbage level and recalculate every frame

	auto garbagePileView = registry->view<GarbagePile, component::Renderable, component::Transform>();
	for (auto& garbagePileEntity : garbagePileView)
	{
		auto& garbagePile           = registry->get<GarbagePile>(garbagePileEntity);
		auto& garbagePileRenderable = registry->get<component::Renderable>(garbagePileEntity);
		auto& garbagePileTransform  = registry->get<component::Transform>(garbagePileEntity);

		if (addGarbageLevel)
			increaseGarbageLevel(garbagePileEntity);

		if (garbagePile.delayBeforeAddingGarbageCountdown > 0.0f)
		{
			garbagePile.delayBeforeAddingGarbageCountdown -= Time::deltaTime();

			if (garbagePile.delayBeforeAddingGarbageCountdown < 0.0f)
				increaseGarbageLevel(garbagePileEntity);
		}

		if (garbagePile.delayBeforeRemovingGarbageCountdown > 0.0f)
		{
			garbagePile.delayBeforeRemovingGarbageCountdown -= Time::deltaTime();

			if (garbagePile.delayBeforeRemovingGarbageCountdown < 0.0f)
				updateGarbagePileVisualSize(garbagePileEntity);
		}

		totalGarbageLevel += garbagePile.garbageLevel;

		//if (totalGarbageLevel >= 15)
			//totalGarbageLevel = 500;
	}

	if (totalGarbageLevel != lastFrameTotalGarbageLevel)
	{
		TotalGarbageCountEvent totalGarbageUpdate;
		totalGarbageUpdate.totalGarbageCount = totalGarbageLevel;
		postEvent(totalGarbageUpdate);
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
			garbagePile.delayBeforeRemovingGarbageCountdown = garbagePile.DELAY_BEFORE_REMOVING_GARBAGE_DURATION;

			decreaseGarbageLevel(evt.garbagePileEntity);
			//don't update the visual size of the garbage pile till AFTER the delay before removing garbage countdown hits 0.0s

			GarbageCleanedEvent garbageCleaned;
			garbageCleaned.garbagePileEntity = evt.garbagePileEntity;
			postEvent(garbageCleaned);

			break;
		}

		case (EventType)TypeCannonFired:
		{
			auto evt     = event_cast<CannonFiredEvent>(event);
			auto& cannon = registry->get<Cannon>(evt.cannonEntity);

			int garbagePileFiredAt = cannon.cannonTrackPosition * -1; //flip the sign since we're on the other ship (cannon at -1 points at opposite ship's garbage pile at 1)

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
	}

	return false;
}

void GarbagePileSystem::increaseGarbageLevel(entt::entity a_garbagePileEntity)
{
	auto& garbagePile = registry->get<GarbagePile>(a_garbagePileEntity);

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

	updateGarbagePileVisualSize(a_garbagePileEntity); //update size whenever garbage is added

	IncreasedGarbageLevelEvent increasedGarbageLevel;
	increasedGarbageLevel.garbagePileEntity = a_garbagePileEntity;
	postEvent(increasedGarbageLevel);
}

void GarbagePileSystem::decreaseGarbageLevel(entt::entity a_garbagePileEntity)
{
	auto& garbagePile = registry->get<GarbagePile>(a_garbagePileEntity);

	if (garbagePile.garbageTicks < garbagePile.GARBAGE_TICKS_PER_LEVEL)
		garbagePile.garbageTicks -= garbagePile.isGlooped ? 0.5f : 1.0f;
	else //garbage ticks >= MAX TICKS
		garbagePile.garbageTicks -= 1.0f;

	if (garbagePile.garbageTicks <= 0.0f)
	{
		garbagePile.garbageLevel--;
		if (garbagePile.garbageLevel > 0)
			garbagePile.garbageTicks = garbagePile.GARBAGE_TICKS_PER_LEVEL;
		else //garbagePile.garbageLevel == 0
			garbagePile.garbageTicks = 0;

		garbagePile.isGlooped = false;
	}

	//dont update garbage pile size yet because this should be done after the delay before removing garbage countdown reaches 0.0s.
	//if you want to update the garbage pile size right away, do it after calling this function
}

void GarbagePileSystem::updateGarbagePileVisualSize(entt::entity a_garbagePileEntity)
{
	auto& garbagePile           = registry->get<GarbagePile>(a_garbagePileEntity);
	auto& garbagePileRenderable = registry->get<component::Renderable>(a_garbagePileEntity);

	if (garbagePile.garbageLevel == garbagePile.MAX_GARBAGE_LEVEL)
	{
		if (garbagePile.garbageTicks == garbagePile.GARBAGE_TICKS_PER_LEVEL)
			garbagePileRenderable.mesh = Mesh::get("garbageSurrender");
		else
			garbagePileRenderable.mesh = Mesh::get("garbageMassive");
	}
	else if (garbagePile.garbageLevel == garbagePile.MAX_GARBAGE_LEVEL - 1)
		garbagePileRenderable.mesh = Mesh::get("garbageLarge");
	else if (garbagePile.garbageLevel == garbagePile.MAX_GARBAGE_LEVEL - 2)
		garbagePileRenderable.mesh = Mesh::get("garbageMedium");
	else if (garbagePile.garbageLevel == garbagePile.MAX_GARBAGE_LEVEL - 3)
		garbagePileRenderable.mesh = Mesh::get("garbageSmall");
	else 
		garbagePileRenderable.mesh = Mesh::get("garbageTiny");

	if (garbagePile.garbageLevel == 0)
		garbagePileRenderable.enabled = false;
	else
		garbagePileRenderable.enabled = true;
}