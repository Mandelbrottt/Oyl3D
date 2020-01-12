#include "CleaningQuicktimeEventSystem.h"

void CleaningQuicktimeEventSystem::onEnter()
{
	this->listenForEventCategory((EventCategory)CategoryPlayer);
	this->listenForEventCategory((EventCategory)CategoryQuicktimeCleaningEvent);
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
		auto& indicator          = registry->get<CleaningQuicktimeEventIndicator>(cleaningQuicktimeEventIndicatorEntity);
		auto& indicatorTransform = registry->get<component::Transform>(cleaningQuicktimeEventIndicatorEntity);

		if (!indicator.isActive)
			continue;

		if (indicator.lerpInformation.isMovingForward)
			indicator.lerpInformation.interpolationParam += indicator.lerpInformation.speed * Time::deltaTime();
		else //!isMovingForward
			indicator.lerpInformation.interpolationParam -= indicator.lerpInformation.speed * Time::deltaTime();

		if (indicator.lerpInformation.interpolationParam < 0.0f || indicator.lerpInformation.interpolationParam > 1.0f)
		{
			indicator.lerpInformation.isMovingForward    = !indicator.lerpInformation.isMovingForward;
			indicator.lerpInformation.interpolationParam = std::round(indicator.lerpInformation.interpolationParam); //round so it doesnt go below 0 or above 1
		}

		indicatorTransform.setPosition(glm::mix(
			indicator.lerpInformation.startPos,
			indicator.lerpInformation.destinationPos,
			indicator.lerpInformation.interpolationParam));
	}
}

bool CleaningQuicktimeEventSystem::onEvent(const Event& event)
{
	switch (event.type)
	{
		case (EventType)TypePlayerInteractResult:
		{
			auto evt = event_cast<PlayerInteractResultEvent>(event);

			//check if we need to activate the cleaning quicktime event
			if (evt.interactionType == PlayerInteractionResult::activateCleaningQuicktimeEvent)
			{
				auto cleaningQuicktimeEventIndicatorView = registry->view<component::Transform, CleaningQuicktimeEventIndicator>();
				for (auto& cleaningQuicktimeEventIndicatorEntity : cleaningQuicktimeEventIndicatorView)
				{
					auto& indicator           = registry->get<CleaningQuicktimeEventIndicator>(cleaningQuicktimeEventIndicatorEntity);
					auto& indicatorHUDElement = registry->get<PlayerHUDElement>(cleaningQuicktimeEventIndicatorEntity);

					if (evt.playerNum == indicatorHUDElement.playerNum && !indicator.isActive)
						activateCleaningQuicktimeEvent(cleaningQuicktimeEventIndicatorEntity);
				}
			}

			break;
		}
		case (EventType)TypePlayerInteractionRequest: //player pressed the interact button
		{
			auto evt = event_cast<PlayerInteractionRequestEvent>(event);

			auto cleaningQuicktimeEventIndicatorView = registry->view<component::Transform, CleaningQuicktimeEventIndicator, PlayerHUDElement>();
			for (auto& cleaningQuicktimeEventIndicatorEntity : cleaningQuicktimeEventIndicatorView)
			{
				auto& indicator           = registry->get<CleaningQuicktimeEventIndicator>(cleaningQuicktimeEventIndicatorEntity);
				auto& indicatorTransform  = registry->get<component::Transform>(cleaningQuicktimeEventIndicatorEntity);
				auto& indicatorHUDElement = registry->get<PlayerHUDElement>(cleaningQuicktimeEventIndicatorEntity);

				int playerNum = registry->get<Player>(evt.playerEntity).playerNum;

				if (playerNum != indicatorHUDElement.playerNum || !indicator.isActive)
					continue;

				//create an event that stores whether or not the quicktime event interaction was successful
				QuicktimeCleaningEventResultEvent quicktimeCleaningEventResult;
				quicktimeCleaningEventResult.playerEntity = evt.playerEntity;

				//check if the indicator was in the successful range
				if (   indicator.lerpInformation.interpolationParam >= indicator.LOWER_BOUND_FOR_SUCCESS
					&& indicator.lerpInformation.interpolationParam <= indicator.UPPER_BOUND_FOR_SUCCESS)
				{
					quicktimeCleaningEventResult.wasSuccessful = true;
				}
				else
					quicktimeCleaningEventResult.wasSuccessful = false;

				postEvent(quicktimeCleaningEventResult); //send the event out

				//deactivate quicktime cleaning event after player attempt (whether it was successful or not)
				deactivateCleaningQuicktimeEvent(cleaningQuicktimeEventIndicatorEntity);
			}

			break;
		}
		case (EventType)TypeCancelQuicktimeCleaningEvent:
		{
			auto evt = event_cast<CancelQuicktimeCleaningEventEvent>(event);

			auto cleaningQuicktimeEventIndicatorView = registry->view<component::Transform, CleaningQuicktimeEventIndicator>();
			for (auto& cleaningQuicktimeEventIndicatorEntity : cleaningQuicktimeEventIndicatorView)
			{
				auto& indicatorHUDElement = registry->get<PlayerHUDElement>(cleaningQuicktimeEventIndicatorEntity);

				if (evt.playerNum == indicatorHUDElement.playerNum)
					deactivateCleaningQuicktimeEvent(cleaningQuicktimeEventIndicatorEntity);
			}

			break;
		}
	}
	return false;
}

void CleaningQuicktimeEventSystem::activateCleaningQuicktimeEvent(entt::entity a_cleaningQuicktimeEventIndicatorEntity)
{
	auto& indicator           = registry->get<CleaningQuicktimeEventIndicator>(a_cleaningQuicktimeEventIndicatorEntity);
	auto& indicatorTransform  = registry->get<component::Transform>(a_cleaningQuicktimeEventIndicatorEntity);
	auto& indicatorHUDElement = registry->get<PlayerHUDElement>(a_cleaningQuicktimeEventIndicatorEntity);

	auto& backgroundTransform = registry->get<component::Transform>(indicator.cleaningQuicktimeEventBackground);
	auto& backgroundHUDElement = registry->get<PlayerHUDElement>(indicator.cleaningQuicktimeEventBackground);

	indicator.isActive = true;

	indicatorTransform.setPosition(indicatorHUDElement.positionWhenActive);
	indicator.lerpInformation.interpolationParam = (rand() % 99) / 100.0f; //random number between 1% and 100% for the starting interpolation param
	backgroundTransform.setPosition(backgroundHUDElement.positionWhenActive);
}

void CleaningQuicktimeEventSystem::deactivateCleaningQuicktimeEvent(entt::entity a_cleaningQuicktimeEventIndicatorEntity)
{
	auto& indicator          = registry->get<CleaningQuicktimeEventIndicator>(a_cleaningQuicktimeEventIndicatorEntity);
	auto& indicatorTransform = registry->get<component::Transform>(a_cleaningQuicktimeEventIndicatorEntity);

	auto& backgroundTransform = registry->get<component::Transform>(indicator.cleaningQuicktimeEventBackground);

	indicator.isActive = false;

	//yeet them off the screen
	indicatorTransform.setPositionX(-30.0f);
	backgroundTransform.setPositionX(-30.0f);
}