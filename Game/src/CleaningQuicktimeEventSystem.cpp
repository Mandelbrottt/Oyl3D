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
		auto& indicator = registry->get<CleaningQuicktimeEventIndicator>(cleaningQuicktimeEventIndicatorEntity);
		auto& indicatorTransform = registry->get<component::Transform>(cleaningQuicktimeEventIndicatorEntity);

		auto& backgroundTransform = registry->get<component::Transform>(indicator.cleaningQuicktimeEventBackground);

		if (indicator.isActive)
		{
			if (indicator.lerpInformation.isMovingForward)
				indicator.lerpInformation.interpolationParam += indicator.lerpInformation.speed * Time::deltaTime();
			else //!isMovingForward
				indicator.lerpInformation.interpolationParam -= indicator.lerpInformation.speed * Time::deltaTime();

			if (indicator.lerpInformation.interpolationParam < 0.0f || indicator.lerpInformation.interpolationParam > 1.0f)
			{
				indicator.lerpInformation.isMovingForward = !indicator.lerpInformation.isMovingForward;
				indicator.lerpInformation.interpolationParam = std::round(indicator.lerpInformation.interpolationParam); //round so it doesnt go below 0 or above 1
			}

			indicatorTransform.setPosition(glm::mix(
				indicator.lerpInformation.startPos,
				indicator.lerpInformation.destinationPos,
				indicator.lerpInformation.interpolationParam));
		}
		else if (indicator.delayBeforeDisappearingCountdown > 0.0f)
		{
			if (indicator.shouldShake)
			{
				{
					float newIndicatorPositionY = indicator.isNumberOfShakesEven
						? indicatorTransform.getPositionY() + indicator.currentShakeValue
						: indicatorTransform.getPositionY() - indicator.currentShakeValue;

					indicatorTransform.setPositionY(newIndicatorPositionY);
				}

				{
					float newBackgroundPositionY = indicator.isNumberOfShakesEven
						? backgroundTransform.getPositionY() + indicator.currentShakeValue
						: backgroundTransform.getPositionY() - indicator.currentShakeValue;

					backgroundTransform.setPositionY(newBackgroundPositionY);
				}

				indicator.currentShakeValue -= indicator.SHAKE_DECREASE_PER_SECOND * Time::deltaTime();
				indicator.currentShakeValue = std::max(indicator.currentShakeValue, 0.0f);

				indicator.isNumberOfShakesEven = !indicator.isNumberOfShakesEven;
			}

			indicator.delayBeforeDisappearingCountdown -= Time::deltaTime();
			if (indicator.delayBeforeDisappearingCountdown < 0.0f)
			{
				indicator.shouldShake = false;
				deactivateCleaningQuicktimeEvent(cleaningQuicktimeEventIndicatorEntity);
			}
		}
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

					indicator.shouldShake = false;
				}
				else
				{
					quicktimeCleaningEventResult.wasSuccessful = false;

					//the cleaning quicktime event UI does a little shake animation on failure to add some juice
					indicator.shouldShake       = true;
					indicator.currentShakeValue = indicator.SHAKE_START_VALUE;
				}

				postEvent(quicktimeCleaningEventResult); //send the event out

				//set timer to hide quicktime cleaning event after player attempt
				indicator.delayBeforeDisappearingCountdown = indicator.DELAY_BEFORE_DISAPPEARING_DURATION;
				indicator.isActive = false;
			}

			break;
		}
		case (EventType)TypeCancelQuicktimeCleaningEvent:
		{
			auto evt = event_cast<CancelQuicktimeCleaningEventEvent>(event);

			auto cleaningQuicktimeEventIndicatorView = registry->view<component::Transform, CleaningQuicktimeEventIndicator>();
			for (auto& cleaningQuicktimeEventIndicatorEntity : cleaningQuicktimeEventIndicatorView)
			{
				auto& indicator           = registry->get<CleaningQuicktimeEventIndicator>(cleaningQuicktimeEventIndicatorEntity);
				auto& indicatorHUDElement = registry->get<PlayerHUDElement>(cleaningQuicktimeEventIndicatorEntity);

				if (evt.playerNum == indicatorHUDElement.playerNum)
				{
					if (indicator.delayBeforeDisappearingCountdown <= 0.0f)
						deactivateCleaningQuicktimeEvent(cleaningQuicktimeEventIndicatorEntity);

					break;
				}
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