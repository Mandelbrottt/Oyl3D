#include "GarbagePileHealthBarSystem.h"

void GarbagePileHealthBarSystem::onEnter()
{
	this->listenForEventCategory((EventCategory)CategoryGarbagePile);
}

void GarbagePileHealthBarSystem::onExit()
{

}

void GarbagePileHealthBarSystem::onUpdate()
{
	auto garbageHPBarView = registry->view<GarbagePileHealthBar, component::GuiRenderable, component::Transform>();
	for (auto garbageHPBarEntity : garbageHPBarView)
	{
		auto& garbageHPBar          = registry->get<GarbagePileHealthBar>(garbageHPBarEntity);
		auto& garbageHPBarGui       = registry->get<component::GuiRenderable>(garbageHPBarEntity);
		auto& garbageHPBarTransform = registry->get<component::Transform>(garbageHPBarEntity);

		auto& garbageHPBarOutlineGui       = registry->get<component::GuiRenderable>(garbageHPBar.outlineEntity);
		auto& garbageHPBarOutlineTransform = registry->get<component::Transform>(garbageHPBar.outlineEntity);

		//check if we need to interpolate the health bar fill (increase or decrease of garbage pile health)
		if (garbageHPBar.startValue != garbageHPBar.targetValue)
		{
			if (garbageHPBar.currentFillAmount > 0.75f)
				garbageHPBar.interpolationSpeed = garbageHPBar.baseInterpolationSpeed;
			else //current fill amount < 3/4
			{
				garbageHPBar.interpolationSpeed = garbageHPBar.baseInterpolationSpeed
					+ (pow(1.0f + abs(garbageHPBar.targetValue - garbageHPBar.startValue), 5.0f) - 1.0f) * 10.0f;
			}

			garbageHPBar.interpolationParam += garbageHPBar.interpolationSpeed * Time::deltaTime();

			garbageHPBar.currentFillAmount = glm::mix(garbageHPBar.startValue, garbageHPBar.targetValue, garbageHPBar.interpolationParam);

			garbageHPBarGui.upperClipping.y = garbageHPBar.currentFillAmount;
			
			if (garbageHPBar.interpolationParam >= 1.0f || garbageHPBar.currentFillAmount < 0.0f)
			{
				garbageHPBar.startValue         = garbageHPBar.targetValue;
				garbageHPBar.interpolationParam = 0.0f;

				//check if the bar has been fully depleted
				if (garbageHPBar.currentFillAmount <= 0.001f) //add a small buffer
				{
					//reset the bar to full
					garbageHPBar.startValue         = 1.0f;
					garbageHPBar.targetValue        = 1.0f;
					garbageHPBarGui.upperClipping.y = 1.0f;

					//check if the bar is supposed to be hidden (garbage pile is fully cleaned)
					if (garbageHPBar.shouldHideAfterInterpolating)
					{
						garbageHPBar.shouldBeHidden               = true;
						garbageHPBar.shouldHideAfterInterpolating = false;
					}
				}
			}
		}

		//check if the HP bar should be hidden, in which case we don't have to worry about setting its position or anything at all really
		if (garbageHPBar.shouldBeHidden)
		{
			garbageHPBarGui.enabled        = false;
			garbageHPBarOutlineGui.enabled = false;
		}
		else
		{
			//go thru each garbage pile
			auto garbagePileView = registry->view<GarbagePile, component::Transform>();
			for (auto garbagePileEntity : garbagePileView)
			{
				auto& garbagePile          = registry->get<GarbagePile>(garbagePileEntity);
				auto& garbagePileTransform = registry->get<component::Transform>(garbagePileEntity);

				//check if the garbage pile is the one that the HP bar is attached to
				if (garbageHPBar.garbagePileNum == garbagePile.relativePositionOnShip && garbageHPBar.team == garbagePile.team)
				{
					//set the HP bar position to the garbage pile's position
					garbageHPBarTransform.setPosition(garbagePileTransform.getPosition() + glm::vec3(0.0f, 0.8f, 0.0f));
					break;
				}
			}

			auto playerCameraView = registry->view<component::Camera>();
			for (auto playerCameraEntity : playerCameraView)
			{
				auto& playerCamera          = registry->get<component::Camera>(playerCameraEntity);
				auto& playerCameraTransform = registry->get<component::Transform>(playerCameraEntity);

				//check if the camera should see the HP bar
				if (playerCamera.player == garbageHPBar.playerNum)
				{
					//find distance between camera and garbage pile
					glm::vec3 playerToGarbagePile = playerCameraTransform.getPositionGlobal() - garbageHPBarTransform.getPositionGlobal();
					//disable the HP bar if the player's camera is too far away
					if (glm::length(playerToGarbagePile) > 15.0f)
					{
						garbageHPBarGui.enabled        = false;
						garbageHPBarOutlineGui.enabled = false;
					}
					else //camera is in range of garbage pile
					{
						glm::vec3 garbagePileDirection = garbageHPBarTransform.getPosition() - playerCameraTransform.getPositionGlobal();
						if (garbagePileDirection != glm::vec3(0.0f, 0.0f, 0.0f))
							garbagePileDirection = normalize(garbagePileDirection);

						float playerLookDotGarbagePileDirection = glm::dot(playerCameraTransform.getForwardGlobal(), garbagePileDirection);

						//set screen space position for the HP bar (we set the world position based on the garbage pile position above)
						glm::vec3 newPosition = playerCamera.worldToScreenSpace(garbageHPBarTransform.getPosition());
						garbageHPBarTransform.       setPosition(newPosition);
						garbageHPBarOutlineTransform.setPosition(newPosition + glm::vec3(0.0f, 0.0f, -1.0f)); //make sure the outline is drawn above the fill so we can see it

						//disable any HP bars that are behind the camera
						bool newEnabled = playerLookDotGarbagePileDirection > 0.0f;
						garbageHPBarGui.enabled        = newEnabled;
						garbageHPBarOutlineGui.enabled = newEnabled;

						break;
					}
				}
			}
		}
	}
}

bool GarbagePileHealthBarSystem::onEvent(const Event& event)
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
					garbageHPBar.startValue = glm::mix(garbageHPBar.startValue, garbageHPBar.targetValue, garbageHPBar.interpolationParam); //set the current value to the start value

					if (garbagePile.garbageTicks == garbagePile.GARBAGE_TICKS_PER_LEVEL)
						garbageHPBar.targetValue -= 0.06f;
					else
						garbageHPBar.targetValue = garbagePile.garbageTicks / garbagePile.GARBAGE_TICKS_PER_LEVEL;

					garbageHPBar.interpolationParam = 0.0f; //reset interpolation param

					if (garbagePile.garbageLevel == 0)
						garbageHPBar.shouldHideAfterInterpolating = true;
				}
			}

			break;
		}

		case (EventType)TypeIncreasedGarbageLevel:
		{
			auto evt = event_cast<IncreasedGarbageLevelEvent>(event);

			const auto& garbagePile = registry->get<GarbagePile>(evt.garbagePileEntity);

			//we only want to keep going if the garbage level and ticks are maxed out (garbage will no longer be cleanable)
			if (garbagePile.garbageLevel != garbagePile.MAX_GARBAGE_LEVEL || garbagePile.garbageTicks != garbagePile.GARBAGE_TICKS_PER_LEVEL)
				break;

			auto garbageHPBarView = registry->view<GarbagePileHealthBar, component::GuiRenderable, component::Transform>();
			for (auto garbageHPBarEntity : garbageHPBarView)
			{
				auto& garbageHPBar = registry->get<GarbagePileHealthBar>(garbageHPBarEntity);

				if (garbageHPBar.garbagePileNum == garbagePile.relativePositionOnShip && garbageHPBar.team == garbagePile.team)
					garbageHPBar.shouldBeHidden = true;
			}

			break;
		}

		case (EventType)TypeGarbagePileReappeared:
		{
			auto evt = event_cast<GarbagePileReappearedEvent>(event);

			const auto& garbagePile = registry->get<GarbagePile>(evt.garbagePileEntity);

			auto garbageHPBarView = registry->view<GarbagePileHealthBar, component::GuiRenderable, component::Transform>();
			for (auto garbageHPBarEntity : garbageHPBarView)
			{
				auto& garbageHPBar = registry->get<GarbagePileHealthBar>(garbageHPBarEntity);

				if (garbageHPBar.garbagePileNum == garbagePile.relativePositionOnShip && garbageHPBar.team == garbagePile.team)
				{
					garbageHPBar.shouldBeHidden = false;

					//this check prevents and edge case where the garbage pile reappears while the player is in the process of cleaning the
					//very last tick, causing the HP bar to disappear even though the garbage pile exists
					if (garbageHPBar.shouldHideAfterInterpolating)
						garbageHPBar.shouldHideAfterInterpolating = false;
				}
			}

			break;
		}
	}
	return false;
}