#include "GarbagePileSystem.h"

void GarbagePileSystem::onEnter()
{
	this->listenForEventCategory((OylEnum)CategoryGarbagePile);
}

void GarbagePileSystem::onExit()
{

}

void GarbagePileSystem::onUpdate(Timestep dt)
{
	//TODO: change to add to a random garbage pile rather than all of them for the final version
	bool addGarbageLevel = false;
	passiveGarbageBuildupCountdown -= dt;
	if (passiveGarbageBuildupCountdown < 0.0f)
	{
		addGarbageLevel = true;
		passiveGarbageBuildupCountdown = PASSIVE_GARBAGE_BUILDUP_TIME - numBuildUpsAccumulated * 0.5f; //TODO: come up with a better calculate for time scaling
		numBuildUpsAccumulated++;
	}

	int lastFrameTotalGarbageLevel = totalGarbageLevel; //get the last frame's total garbage level so we can compare them later
	totalGarbageLevel = 0; //reset the total garbage level and recalculate every frame

	auto view = registry->view<GarbagePile, component::Renderable, component::Transform>();
	for (auto& garbagePileEntity : view)
	{
		auto& garbagePile = registry->get<GarbagePile>(garbagePileEntity);
		auto& garbagePileRenderable = registry->get<component::Renderable>(garbagePileEntity);
		auto& garbagePileTransform  = registry->get<component::Transform>(garbagePileEntity);

		if (addGarbageLevel)
			increaseGarbageLevel(garbagePileEntity);

		garbagePileTransform.setScale(glm::vec3(
			0.3f * garbagePile.garbageLevel + 1.5f,
			0.7f,
			0.3f * garbagePile.garbageLevel + 1.5f));

		if (garbagePile.garbageLevel <= 0) //check if garbage is fully depleted
			garbagePileRenderable.enabled = false;
		else //garbage isn't fully depleted
			garbagePileRenderable.enabled = true;

		totalGarbageLevel += garbagePile.garbageLevel;

		//if (totalGarbageLevel >= 15)
			//totalGarbageLevel = 500;
	}

	if (totalGarbageLevel != lastFrameTotalGarbageLevel)
	{
		TotalGarbageCountEvent totalGarbageUpdate;
		totalGarbageUpdate.totalGarbageCount = totalGarbageLevel;
		postEvent(Event::create(totalGarbageUpdate));
	}
}

bool GarbagePileSystem::onEvent(Ref<Event> event)
{
	switch (event->type)
	{
		case TypeRequestToCleanGarbage:
		{
			auto evt = (RequestToCleanGarbageEvent)* event;
			auto& garbagePile = registry->get<GarbagePile>(evt.garbagePileEntity);

			if (garbagePile.garbageTicks < garbagePile.GARBAGE_TICKS_PER_LEVEL)
				garbagePile.garbageTicks -= garbagePile.isGlooped ? 0.5f : 1.0f;
			else //garbage ticks >= MAX TICKS
				garbagePile.garbageTicks -= 1.0f;

			if (garbagePile.garbageTicks <= 0.0f)
			{
				garbagePile.garbageLevel--;
				garbagePile.garbageTicks = garbagePile.garbageLevel > 0 ? garbagePile.GARBAGE_TICKS_PER_LEVEL : 0.0f; //set ticks to 0 if the garbage pile is depleted since there's nothing to clean
				garbagePile.isGlooped = false;
			}

			GarbageCleanedEvent garbageCleaned;
			garbageCleaned.numGarbageTicksToDisplay = garbagePile.garbageTicks;
			garbageCleaned.displayGlooped           = garbagePile.isGlooped;
			postEvent(Event::create(garbageCleaned));

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
	}
	else if (garbagePile.garbageLevel < garbagePile.MAX_GARBAGE_LEVEL)
	{
		garbagePile.garbageLevel++;
	}
	else //garbage level == MAX
	{
		garbagePile.garbageTicks = garbagePile.GARBAGE_TICKS_PER_LEVEL;
	}
}