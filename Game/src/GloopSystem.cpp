#include "GloopSystem.h"

void GloopSystem::onEnter()
{
	listenForEventCategory((EventCategory)CategoryGloop);
}

void GloopSystem::onExit()
{

}

void GloopSystem::onUpdate()
{
	
}

bool GloopSystem::onEvent(const Event& event)
{
	switch (event.type)
	{
		case (EventType)TypeUseGloopRequest:
		{
			auto evt    = event_cast<UseGloopRequestEvent>(event);
			auto& gloop = registry->get<Gloop>(evt.gloopEntity);
				
			gloop.numUses--;

			GloopedUsedEvent gloopedUsed;
			gloopedUsed.playerEntity = evt.playerEntity;
			gloopedUsed.isLastUse    = gloop.numUses == 0;
			postEvent(gloopedUsed);

			if (gloop.numUses == 0)
				registry->destroy(evt.gloopEntity);

			break;
		}
	}

	return false;
}