#include "CleaningQuicktimeEventSystem.h"

void CleaningQuicktimeEventSystem::onEnter()
{
	this->listenForEventCategory((EventCategory)CategoryPlayer);
}

void CleaningQuicktimeEventSystem::onExit()
{

}

void CleaningQuicktimeEventSystem::onUpdate()
{
	//the indicators LERP continuously back and forth between the start and end positions
	auto cleaningQuicktimeEventIndicatorView = registry->view<component::Transform, CleaningQuicktimeEventIndicator>();
	for (auto& cleaningQuicktimeEventIndicatorEntity : cleaningQuicktimeEventIndicatorView)
	{
		auto& cleaningQuicktimeEventIndicator          = registry->get<CleaningQuicktimeEventIndicator>(cleaningQuicktimeEventIndicatorEntity);
		auto& cleaningQuicktimeEventIndicatorTransform = registry->get<component::Transform>(cleaningQuicktimeEventIndicatorEntity);

		if (!cleaningQuicktimeEventIndicator.isActive)
		{
			cleaningQuicktimeEventIndicatorTransform.setPositionX(-30.0f);
			registry->get<component::Transform>(cleaningQuicktimeEventIndicator.cleaningQuicktimeEventBackground).setPositionX(-30.0f);
			continue;
		}

		if (cleaningQuicktimeEventIndicator.lerpInformation.isMovingForward)
		{
			cleaningQuicktimeEventIndicator.lerpInformation.interpolationParam 
				= cleaningQuicktimeEventIndicator.lerpInformation.interpolationParam 
				+ cleaningQuicktimeEventIndicator.lerpInformation.speed 
				* Time::deltaTime();
		}
		else //!isMovingForward
		{
			cleaningQuicktimeEventIndicator.lerpInformation.interpolationParam 
				= cleaningQuicktimeEventIndicator.lerpInformation.interpolationParam 
				- cleaningQuicktimeEventIndicator.lerpInformation.speed 
				* Time::deltaTime();
		}

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

bool CleaningQuicktimeEventSystem::onEvent(const Event& event)
{
	switch (event.type)
	{
		case (EventType)TypePlayerInteractionRequest:
		{
			auto evt = event_cast<PlayerInteractionRequestEvent>(event);

			auto cleaningQuicktimeEventIndicatorView = registry->view<component::Transform, CleaningQuicktimeEventIndicator>();
			for (auto& cleaningQuicktimeEventIndicatorEntity : cleaningQuicktimeEventIndicatorView)
			{
				auto& cleaningQuicktimeEventIndicator          = registry->get<CleaningQuicktimeEventIndicator>(cleaningQuicktimeEventIndicatorEntity);
				auto& cleaningQuicktimeEventIndicatorTransform = registry->get<component::Transform>(cleaningQuicktimeEventIndicatorEntity);

				if (!cleaningQuicktimeEventIndicator.isActive)
					continue;

				//send out an event that stores whether or not the quicktime event interaction was successful
				QuicktimeCleaningEventResultEvent quicktimeCleaningEventResult;
				quicktimeCleaningEventResult.playerEntity = evt.playerEntity;
				//check if the indicator was in the successful range
				if (   cleaningQuicktimeEventIndicator.lerpInformation.interpolationParam >= cleaningQuicktimeEventIndicator.LOWER_BOUND_FOR_SUCCESS
					&& cleaningQuicktimeEventIndicator.lerpInformation.interpolationParam <= cleaningQuicktimeEventIndicator.UPPER_BOUND_FOR_SUCCESS)
				{
					quicktimeCleaningEventResult.wasSuccessful = true;
				}
				else
					quicktimeCleaningEventResult.wasSuccessful = false;
				postEvent(quicktimeCleaningEventResult);
			}

			break;
		}
		case (EventType)TypePlayerInteractResult:
		{
			auto evt = event_cast<PlayerInteractResultEvent>(event);

			if (evt.interactionType == PlayerInteractionResult::cleanGarbagePile)
			{
				auto cleaningQuicktimeEventIndicatorView = registry->view<component::Transform, CleaningQuicktimeEventIndicator>();
				for (auto& cleaningQuicktimeEventIndicatorEntity : cleaningQuicktimeEventIndicatorView)
				{
					auto& cleaningQuicktimeEventIndicator = registry->get<CleaningQuicktimeEventIndicator>(cleaningQuicktimeEventIndicatorEntity);
					auto& cleaningQuicktimeEventIndicatorTransform = registry->get<component::Transform>(cleaningQuicktimeEventIndicatorEntity);

					cleaningQuicktimeEventIndicator.isActive = true;
					cleaningQuicktimeEventIndicatorTransform.setPositionX(-4.95f);
					registry->get<component::Transform>(cleaningQuicktimeEventIndicator.cleaningQuicktimeEventBackground).setPositionX(0.0f);
				}
			}
		}
	}
	return false;
}