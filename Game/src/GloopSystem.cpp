#include "GloopSystem.h"

void GloopSystem::onEnter()
{
	this->listenForEventCategory((EventCategory)CategoryGloop);
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
		case (EventType)TypeUseGloop:
		{
			auto evt = event_cast<UseGloopEvent>(event);
			auto& gloop = registry->get<Gloop>(evt.gloopEntity);
				
			gloop.numUses--;

			if (gloop.numUses == 0)
				registry->destroy(evt.gloopEntity);

			break;
		}
	}

	return false;
}