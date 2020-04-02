#include "GarbageMeterSystem.h"

void GarbageMeterSystem::onEnter()
{
	listenForEventCategory((EventCategory)CategoryGarbagePile);
}

void GarbageMeterSystem::onExit()
{

}

void GarbageMeterSystem::onUpdate()
{
	auto garbageMeterView = registry->view<GarbageMeterDisplay, component::Transform>();
	for (auto meterEntity : garbageMeterView)
	{
		auto& meter          = registry->get<GarbageMeterDisplay>(meterEntity);
		auto& meterTransform = registry->get<component::Transform>(meterEntity);
		
		for (auto barEntity : meter.garbageMeterBars)
		{
			auto& bar    = registry->get<GarbageMeterBar>(barEntity);
			auto& barGui = registry->get<component::GuiRenderable>(barEntity);

			//check if we need to interpolate the health bar fill (increase or decrease of garbage pile health)
			if (bar.startValue != bar.targetValue)
			{
				bar.interpolationParam += bar.interpolationSpeed * Time::deltaTime();
				float currentFillAmount = glm::mix(bar.startValue, bar.targetValue, bar.interpolationParam);

				barGui.upperClipping.y = currentFillAmount;

				if (bar.interpolationParam >= 1.0f)
				{
					bar.startValue         = bar.targetValue;
					bar.interpolationParam = 0.0f;
				}
			}
		}
	}
}

bool GarbageMeterSystem::onEvent(const Event& event)
{
	switch (event.type)
	{
		case (EventType)TypeIncreasedGarbageLevel:
		{
			auto evt = event_cast<IncreasedGarbageLevelEvent>(event);

			updateMeterBarsForGarbagePile(evt.garbagePileEntity);

			break;
		}

		case (EventType)TypeGarbageCleaned:
		{
			auto evt = event_cast<GarbageCleanedEvent>(event);

			updateMeterBarsForGarbagePile(evt.garbagePileEntity);

			break;
		}

		case (EventType)TypeSetMaxGarbageLevel:
		{
			auto view = registry->view<GarbagePile, component::Transform>();
			for (auto& garbagePileEntity : view)
				updateMeterBarsForGarbagePile(garbagePileEntity);

			break;
		}
	}

	return false;
}

void GarbageMeterSystem::updateMeterBarsForGarbagePile(entt::entity a_garbagePileEntity)
{
	const auto& garbagePile = registry->get<GarbagePile>(a_garbagePileEntity);

	auto garbageMeterView = registry->view<GarbageMeterDisplay, component::Transform>();
	for (auto garbageMeterEntity : garbageMeterView)
	{
		auto& garbageMeter = registry->get<GarbageMeterDisplay>(garbageMeterEntity);

		if (garbageMeter.team != garbagePile.team)
			continue;

		for (auto barEntity : garbageMeter.garbageMeterBars)
		{
			auto& bar = registry->get<GarbageMeterBar>(barEntity);

			if (bar.garbagePileNum == garbagePile.relativePositionOnShip)
			{
				bar.startValue = glm::mix(bar.startValue, bar.targetValue, bar.interpolationParam); //set the current value to the start value

				//calculate garbage level relative to max and then reduce it based on number of ticks missing as a percentage of one garbage level
				//ex. max ticks will subtract zero, 1/4 ticks will subtract by 1/4 of 1/5 (where 1/5 represents one full garbage level)
				bar.targetValue = (float)garbagePile.garbageLevel / (float)garbagePile.MAX_GARBAGE_LEVEL
					- ((1.0f / (float)garbagePile.MAX_GARBAGE_LEVEL) - (garbagePile.garbageTicks / garbagePile.GARBAGE_TICKS_PER_LEVEL / (float)garbagePile.MAX_GARBAGE_LEVEL));

				bar.interpolationParam = 0.0f; //reset interpolation parameter in case the bar was already moving
			}
		}
	}
}