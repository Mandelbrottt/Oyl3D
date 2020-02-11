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

	auto garbagePileView = registry->view<GarbagePile, component::Renderable, component::Transform>();
	for (auto& garbagePileEntity : garbagePileView)
	{
		auto& garbagePile = registry->get<GarbagePile>(garbagePileEntity);
		auto& garbagePileRenderable = registry->get<component::Renderable>(garbagePileEntity);
		auto& garbagePileTransform = registry->get<component::Transform>(garbagePileEntity);

		if (addGarbageLevel)
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

	case (EventType)TypeCannonFired:
	{
		auto evt = event_cast<CannonFiredEvent>(event);
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
	}

	updateGarbagePileVisualSize(a_garbagePileEntity);
}

void GarbagePileSystem::updateGarbagePileVisualSize(entt::entity a_garbagePileEntity)
{
	auto& garbagePile			 = registry->get<GarbagePile>(a_garbagePileEntity);
	auto& garbagePileRenderable	 = registry->get<component::Renderable>(a_garbagePileEntity);
	auto& flyAnimator = registry->get<component::Animatable>(garbagePile.flyEntity);
	auto& flyRenderable = registry->get<component::Renderable>(garbagePile.flyEntity);


	//determine which mesh to set based on garbage level
	if (garbagePile.garbageLevel == garbagePile.MAX_GARBAGE_LEVEL)
	{
		if (garbagePile.garbageTicks == garbagePile.GARBAGE_TICKS_PER_LEVEL)
		{
			garbagePileRenderable.mesh = Mesh::get("garbageSurrender");
			flyAnimator.setNextAnimation("GarbageSurrenderAnim");
		}
		else
		{
			garbagePileRenderable.mesh = Mesh::get("garbageMassive");
			flyAnimator.setNextAnimation("GarbageMassiveAnim");
		}
	}
	else if (garbagePile.garbageLevel == garbagePile.MAX_GARBAGE_LEVEL - 1)
	{
		garbagePileRenderable.mesh = Mesh::get("garbageLarge");
		flyAnimator.setNextAnimation("GarbageLargeAnim");
	}
	else if (garbagePile.garbageLevel == garbagePile.MAX_GARBAGE_LEVEL - 2)
	{
		garbagePileRenderable.mesh = Mesh::get("garbageMedium");
		flyAnimator.setNextAnimation("GarbageMediumAnim");
	}
	else if (garbagePile.garbageLevel == garbagePile.MAX_GARBAGE_LEVEL - 3)
	{
		garbagePileRenderable.mesh = Mesh::get("garbageSmall");
		flyAnimator.setNextAnimation("GarbageSmallAnim");
	}
	else
	{
		garbagePileRenderable.mesh = Mesh::get("garbageTiny");
		flyAnimator.setNextAnimation("GarbageTinyAnim");
	}

	if (garbagePile.garbageLevel == 0)
	{
		garbagePileRenderable.enabled = false;
		flyRenderable.enabled = false;
	}
	else
	{
		garbagePileRenderable.enabled = true;
		flyRenderable.enabled = true;
	}
}