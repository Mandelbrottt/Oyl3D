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
	auto cleaningQTEView = registry->view<component::Transform, CleaningQuicktimeEvent>();
	for (auto& cleaningQTEEntity : cleaningQTEView)
	{
		auto& cleaningQTE           = registry->get<CleaningQuicktimeEvent>(cleaningQTEEntity);
		auto& cleaningQTERenderable = registry->get<component::GuiRenderable>(cleaningQTEEntity);

		if (cleaningQTE.isActive)
		{
			int controllerNum;

			auto& playerView = registry->view<Player>();
			for (auto& playerEntity : playerView)
			{
				auto& player = registry->get<Player>(playerEntity);

				if (player.playerNum == cleaningQTE.playerNum)
				{
					controllerNum = player.controllerNum;
					break;
				}
			}

			glm::vec2 rightStick = Input::getGamepadRightStick(controllerNum);

			if (cleaningQTE.isPointingUp)
			{
				cleaningQTERenderable.texture = Texture2D::get("cleaningQTEUp");

				if (rightStick.y < -0.9f)
				{
					cleaningQTE.isPointingUp = false;

					RequestToCleanGarbageEvent requestToCleanGarbage;
					requestToCleanGarbage.garbagePileEntity = cleaningQTE.garbagePileBeingCleaned;
					postEvent(requestToCleanGarbage);
				}
			}
			else //!isPointingUp
			{
				cleaningQTERenderable.texture = Texture2D::get("cleaningQTEDown");

				if (rightStick.y > 0.9f)
				{
					cleaningQTE.isPointingUp = true;

					RequestToCleanGarbageEvent requestToCleanGarbage;
					requestToCleanGarbage.garbagePileEntity = cleaningQTE.garbagePileBeingCleaned;
					postEvent(requestToCleanGarbage);
				}
			}
		}
	}
}

bool CleaningQuicktimeEventSystem::onEvent(const Event& event)
{
	switch (event.type)
	{
		case (EventType)TypeActivateQuicktimeCleaningEvent:
		{
			auto evt = event_cast<ActivateQuicktimeCleaningEventEvent>(event);

			auto cleaningQTEView = registry->view<component::Transform, CleaningQuicktimeEvent>();
			for (auto& cleaningQTEEntity : cleaningQTEView)
			{
				auto& cleaningQTE    = registry->get<CleaningQuicktimeEvent>(cleaningQTEEntity);
				auto& cleaningQTEGui = registry->get<component::GuiRenderable>(cleaningQTEEntity);

				auto& cancelQTEPromptGui = registry->get<component::GuiRenderable>(cleaningQTE.cancelPromptEntity);

				if (cleaningQTE.playerNum == evt.playerNum && !cleaningQTE.isActive)
				{
					cleaningQTE.garbagePileBeingCleaned = evt.garbagePileEntity;

					cleaningQTE.isActive       = true;
					cleaningQTE.isPointingUp   = true; //always start with it pointing up
					cleaningQTEGui.enabled     = true;
					cancelQTEPromptGui.enabled = true;

					auto& playerView = registry->view<Player>();
					for (auto& playerEntity : playerView)
					{
						auto& player = registry->get<Player>(playerEntity);

						if (player.playerNum == cleaningQTE.playerNum)
						{
							player.isCameraLocked = true;
							break;
						}
					}
				}
			}

			break;
		}
		
		case (EventType)TypeCancelQuicktimeCleaningEvent:
		{
			auto evt = event_cast<CancelQuicktimeCleaningEventEvent>(event);

			auto cleaningQTEView = registry->view<component::Transform, CleaningQuicktimeEvent>();
			for (auto& cleaningQTEEntity : cleaningQTEView)
			{
				auto& cleaningQTE    = registry->get<CleaningQuicktimeEvent>(cleaningQTEEntity);
				auto& cleaningQTEGui = registry->get<component::GuiRenderable>(cleaningQTEEntity);

				auto& cancelQTEPromptGui = registry->get<component::GuiRenderable>(cleaningQTE.cancelPromptEntity);

				if (cleaningQTE.playerNum == evt.playerNum && cleaningQTE.isActive)
				{
					cleaningQTE.garbagePileBeingCleaned = entt::null;

					cleaningQTE.isActive       = false;
					cleaningQTEGui.enabled     = false;
					cancelQTEPromptGui.enabled = false;

					auto& playerView = registry->view<Player>();
					for (auto& playerEntity : playerView)
					{
						auto& player = registry->get<Player>(playerEntity);

						if (player.playerNum == cleaningQTE.playerNum)
						{
							player.isCameraLocked = false;
							break;
						}
					}
				}
			}

			break;
		}
	}
	return false;
}