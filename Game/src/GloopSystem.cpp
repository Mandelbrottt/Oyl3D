#include "GloopSystem.h"

void GloopSystem::onEnter()
{
	this->listenForEventCategory((OylEnum)CategoryGloop);
}

void GloopSystem::onExit()
{

}

void GloopSystem::onUpdate(Timestep dt)
{
	
}

bool GloopSystem::onEvent(Ref<Event> event)
{
	switch (event->type)
	{
		case TypeUseGloop:
		{
			auto evt = (UseGloopEvent)* event;
			auto& gloop = registry->get<Gloop>(evt.gloopEntity);
				
			gloop.numUses--;

			if (gloop.numUses == 0)
				registry->destroy(evt.gloopEntity);

			break;
		}
	}

	return false;
}