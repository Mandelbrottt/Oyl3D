#include "GarbageMeterSystem.h"

void GarbageMeterSystem::onEnter()
{
	this->listenForEventCategory((EventCategory)CategoryGarbagePile);
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
		case (EventType)TypeGarbageCleaned:
		{
			auto evt = event_cast<GarbageCleanedEvent>(event);

			const auto& garbagePile = registry->get<GarbagePile>(evt.garbagePileEntity);

			auto garbageHPBarView = registry->view<GarbagePileHealthBar, component::GuiRenderable, component::Transform>();
			for (auto garbageHPBarEntity : garbageHPBarView)
			{
				auto& garbageHPBar = registry->get<GarbagePileHealthBar>(garbageHPBarEntity);

				if (garbageHPBar.garbagePileNum == garbagePile.relativePositionOnShip && garbageHPBar.team == garbagePile.team)
				{
					if (garbagePile.garbageTicks == garbagePile.GARBAGE_TICKS_PER_LEVEL)
						garbageHPBar.targetValue = 0.0f;
					else
						garbageHPBar.targetValue = garbagePile.garbageTicks / garbagePile.GARBAGE_TICKS_PER_LEVEL;

					if (garbagePile.garbageLevel == 0)
						garbageHPBar.shouldHideAfterInterpolating = true;
				}
			}

			break;
		}
	}

	return false;
}