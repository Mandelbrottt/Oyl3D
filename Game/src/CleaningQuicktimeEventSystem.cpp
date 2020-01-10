#include "CleaningQuicktimeEventSystem.h"

void CleaningQuicktimeEventSystem::onEnter()
{
	this->listenForEventCategory((OylEnum)CategoryPlayer);
}

void CleaningQuicktimeEventSystem::onExit()
{

}

void CleaningQuicktimeEventSystem::onUpdate(Timestep dt)
{
	//the indicators LERP to the end, and then go in reverse back to the beginning, and then forward to the end again, etc.
	auto cleaningQuicktimeEventIndicatorView = registry->view<component::Transform, CleaningQuicktimeEventIndicator>();
	for (auto& cleaningQuicktimeEventIndicatorEntity : cleaningQuicktimeEventIndicatorView)
	{
		auto& cleaningQuicktimeEventIndicator          = registry->get<CleaningQuicktimeEventIndicator>(cleaningQuicktimeEventIndicatorEntity);
		auto& cleaningQuicktimeEventIndicatorTransform = registry->get<component::Transform>(cleaningQuicktimeEventIndicatorEntity);

		if (!cleaningQuicktimeEventIndicator.isActive)
			continue;

		if (cleaningQuicktimeEventIndicator.lerpInformation.isMovingForward)
			cleaningQuicktimeEventIndicator.lerpInformation.interpolationParam = cleaningQuicktimeEventIndicator.lerpInformation.interpolationParam + cleaningQuicktimeEventIndicator.lerpInformation.speed * dt;
		else //!isMovingForward
			cleaningQuicktimeEventIndicator.lerpInformation.interpolationParam = cleaningQuicktimeEventIndicator.lerpInformation.interpolationParam - cleaningQuicktimeEventIndicator.lerpInformation.speed * dt;

		if (   cleaningQuicktimeEventIndicator.lerpInformation.interpolationParam < 0.0f
			|| cleaningQuicktimeEventIndicator.lerpInformation.interpolationParam > 1.0f)
		{
			cleaningQuicktimeEventIndicator.lerpInformation.isMovingForward    = !cleaningQuicktimeEventIndicator.lerpInformation.isMovingForward;
			cleaningQuicktimeEventIndicator.lerpInformation.interpolationParam = std::round(cleaningQuicktimeEventIndicator.lerpInformation.interpolationParam);
		}

		cleaningQuicktimeEventIndicatorTransform.setPosition(glm::mix(
			cleaningQuicktimeEventIndicator.lerpInformation.startPos,
			cleaningQuicktimeEventIndicator.lerpInformation.destinationPos,
			cleaningQuicktimeEventIndicator.lerpInformation.interpolationParam));
	}
}

bool CleaningQuicktimeEventSystem::onEvent(Ref<Event> event)
{
	switch (event->type)
	{
		case TypePlayerInteractionRequest:
		{
			auto evt = (PlayerInteractionRequestEvent)* event;

			auto cleaningQuicktimeEventIndicatorView = registry->view<component::Transform, CleaningQuicktimeEventIndicator>();
			for (auto& cleaningQuicktimeEventIndicatorEntity : cleaningQuicktimeEventIndicatorView)
			{
				auto& cleaningQuicktimeEventIndicator          = registry->get<CleaningQuicktimeEventIndicator>(cleaningQuicktimeEventIndicatorEntity);
				auto& cleaningQuicktimeEventIndicatorTransform = registry->get<component::Transform>(cleaningQuicktimeEventIndicatorEntity);

				if (!cleaningQuicktimeEventIndicator.isActive)
					continue;

				//check if the indicator was in the successful range
				if (   cleaningQuicktimeEventIndicator.lerpInformation.interpolationParam >= cleaningQuicktimeEventIndicator.LOWER_BOUND_FOR_SUCCESS
					&& cleaningQuicktimeEventIndicator.lerpInformation.interpolationParam <= cleaningQuicktimeEventIndicator.UPPER_BOUND_FOR_SUCCESS)
				{
					std::cout << "SUCCESSFULLY COMPLETED CLEANING QUICKTIME EVENT\n";
				}
				else
					std::cout << "FAILED CLEANING QUICKTIME EVENT\n";
			}

			break;
		}
	}
	return false;
}