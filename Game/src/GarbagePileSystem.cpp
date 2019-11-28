#include "GarbagePileSystem.h"

void GarbagePileSystem::onEnter()
{

}

void GarbagePileSystem::onExit()
{

}

void GarbagePileSystem::onUpdate(Timestep dt)
{
	bool addGarbageLevel = false;
	passiveGarbageBuildupCountdown -= dt;
	if (passiveGarbageBuildupCountdown < 0.0f)
	{
		addGarbageLevel = true;
		passiveGarbageBuildupCountdown = PASSIVE_GARBAGE_BUILDUP_TIME;
	}

	int lastFrameTotalGarbageLevel = totalGarbageLevel; //get the last frame's total garbage level so we can compare them later
	totalGarbageLevel = 0; //reset the total garbage level and recalculate every frame

	auto view = registry->view<GarbagePile, component::Renderable>();
	for (auto& garbagePileEntity : view)
	{
		auto& garbagePile = registry->get<GarbagePile>(garbagePileEntity);
		auto& garbagePileRenderable = registry->get<component::Renderable>(garbagePileEntity);

		if (addGarbageLevel && garbagePile.garbageLevel <= garbagePile.MAX_GARBAGE_LEVEL)
			garbagePile.garbageLevel++;

		if (garbagePile.garbageLevel <= 0) //check if garbage is fully depleted
		{
			garbageLevelHitZeroDirty = true;
			garbagePileRenderable.enabled = false;
		}
		else //garbage isn't fully depleted
		{
			if (garbageLevelHitZeroDirty)
			{
				garbagePileRenderable.enabled = true;
				garbageLevelHitZeroDirty = false;
			}
		}

		totalGarbageLevel += garbagePile.garbageLevel;

		/*if (totalGarbageLevel > 1)
			totalGarbageLevel = 500;*/
	}

	/*if (totalGarbageLevel != lastFrameTotalGarbageLevel)
	{
		TotalGarbageCountEvent totalGarbageUpdate;
		totalGarbageUpdate.totalGarbageCount = totalGarbageLevel;
		postEvent(Event::create(totalGarbageUpdate));
	}*/
}

bool GarbagePileSystem::onEvent(Ref<Event> event)
{
	return false;
}