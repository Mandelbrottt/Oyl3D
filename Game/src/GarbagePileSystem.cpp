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

	auto view = registry->view<GarbagePile, component::Renderable, component::Transform>();
	for (auto& garbagePileEntity : view)
	{
		auto& garbagePile           = registry->get<GarbagePile>(garbagePileEntity);
		auto& garbagePileRenderable = registry->get<component::Renderable>(garbagePileEntity);
		auto& garbagePileTransform  = registry->get<component::Transform>(garbagePileEntity);

		if (garbagePile.delayBeforeAddingGarbageCountdown > 0.0f)
		{
			garbagePile.delayBeforeAddingGarbageCountdown -= Time::deltaTime();

			if (garbagePile.delayBeforeAddingGarbageCountdown < 0.0f)
				increaseGarbageLevel(garbagePileEntity);
		}

		if (addGarbageLevel)
			increaseGarbageLevel(garbagePileEntity);

		garbagePileTransform.setScaleX(0.03f * garbagePile.garbageLevel + 0.4f);
		garbagePileTransform.setScaleZ(0.03f * garbagePile.garbageLevel + 0.4f);

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

			auto& garbagePile           = registry->get<GarbagePile>(evt.garbagePileEntity);
			auto& garbagePileRenderable = registry->get<component::Renderable>(evt.garbagePileEntity);

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
				{
					garbagePile.garbageTicks      = 0;
					garbagePileRenderable.enabled = false;

					if (registry->has<component::RigidBody>(evt.garbagePileEntity))
						registry->remove<component::RigidBody>(evt.garbagePileEntity);
				}
				
				garbagePile.isGlooped = false;
			}

			GarbageCleanedEvent garbageCleaned;
			garbageCleaned.numGarbageTicksToDisplay = garbagePile.garbageTicks;
			garbageCleaned.displayGlooped           = garbagePile.isGlooped;
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
	auto& garbagePile           = registry->get<GarbagePile>(a_garbagePileEntity);
	auto& garbagePileRenderable = registry->get<component::Renderable>(a_garbagePileEntity);

	if (garbagePile.garbageLevel == 0)
	{
		garbagePile.garbageLevel++;
		garbagePile.garbageTicks = garbagePile.GARBAGE_TICKS_PER_LEVEL;

		garbagePileRenderable.enabled = true;
		auto& garbagePileRB = registry->get_or_assign<component::RigidBody>(a_garbagePileEntity);
		garbagePileRB.setMass(0.0f);
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