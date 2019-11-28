#include "UIManager.h"

void UIManagerSystem::onEnter()
{
	this->listenForEventCategory((OylEnum)CategoryPlayer);
}

void UIManagerSystem::onExit()
{
}

void UIManagerSystem::onUpdate(Timestep dt)
{

}

bool UIManagerSystem::onEvent(Ref<Event> event)
{
	switch (event->type)
	{
		case TypePlayerInteractResult:
		{
			auto evt = (PlayerInteractResultEvent)* event;
			switch (evt.interactionType)
			{
				case PlayerInteractionResult::nothing:
				{
					auto view = registry->view<component::GuiRenderable, PlayerInteractionType>();
					for (auto& guiEntity : view)
					{
						auto& renderable = registry->get<component::GuiRenderable>(guiEntity);
						renderable.enabled = false;
					}
					break;
				}
				case PlayerInteractionResult::cannonFiringSoon:
				{
					/*auto view = registry->view<component::GuiRenderable, PlayerInteractionType>();
					for (auto& guiEntity : view)
					{
						auto& renderable = registry->get<component::GuiRenderable>(guiEntity);
						auto& interactionType = registry->get<PlayerInteractionType>(guiEntity);

						if (interactionType.type == evt.interactionType)
						{
							renderable.enabled = true;
							break;
						}
					}

					break;*/
				}
				case PlayerInteractionResult::loadCannon:
				{
					/*auto view = registry->view<component::GuiRenderable, PlayerInteractionType>();
					for (auto& guiEntity : view)
					{
						auto& renderable = registry->get<component::GuiRenderable>(guiEntity);
						auto& interactionType = registry->get<PlayerInteractionType>(guiEntity);

						if (interactionType.type == evt.interactionType)
						{
							renderable.enabled = true;
							break;
						}
					}

					break;*/
				}
				case PlayerInteractionResult::pushCannon:
				{
					/*auto view = registry->view<component::GuiRenderable, PlayerInteractionType>();
					for (auto& guiEntity : view)
					{
						auto& renderable = registry->get<component::GuiRenderable>(guiEntity);
						auto& interactionType = registry->get<PlayerInteractionType>(guiEntity);

						if (interactionType.type == evt.interactionType)
						{
							renderable.enabled = true;
							break;
						}
					}

					break;*/
				}
				case PlayerInteractionResult::takeCannonballFromCrate:
				{
					/*auto view = registry->view<component::GuiRenderable, PlayerInteractionType>();
					for (auto& guiEntity : view)
					{
						auto& renderable = registry->get<component::GuiRenderable>(guiEntity);
						auto& interactionType = registry->get<PlayerInteractionType>(guiEntity);

						if (interactionType.type == evt.interactionType)
						{
							renderable.enabled = true;
							break;
						}
					}

					break;*/
				}
				case PlayerInteractionResult::pickUpCannonball:
				{
					/*auto view = registry->view<component::GuiRenderable, PlayerInteractionType>();
					for (auto& guiEntity : view)
					{
						auto& renderable = registry->get<component::GuiRenderable>(guiEntity);
						auto& interactionType = registry->get<PlayerInteractionType>(guiEntity);

						if (interactionType.type == evt.interactionType)
						{
							renderable.enabled = true;
							break;
						}
					}

					break;*/
				}
				case PlayerInteractionResult::pickUpMop:
				{
					/*auto view = registry->view<component::GuiRenderable, PlayerInteractionType>();
					for (auto& guiEntity : view)
					{
						auto& renderable = registry->get<component::GuiRenderable>(guiEntity);
						auto& interactionType = registry->get<PlayerInteractionType>(guiEntity);

						if (interactionType.type == evt.interactionType)
						{
							renderable.enabled = true;
							break;
						}
					}

					break;*/
				}
				case PlayerInteractionResult::cleanGarbagePile:
				{
					/*auto view = registry->view<component::GuiRenderable, PlayerInteractionType>();
					for (auto& guiEntity : view)
					{
						auto& renderable = registry->get<component::GuiRenderable>(guiEntity);
						auto& interactionType = registry->get<PlayerInteractionType>(guiEntity);

						if (interactionType.type == evt.interactionType)
						{
							renderable.enabled = true;
							break;
						}
					}

					break;*/
				}
				case PlayerInteractionResult::invalid:
				{
					auto view = registry->view<component::GuiRenderable, PlayerInteractionType>();
					for (auto& guiEntity : view)
					{
						auto& renderable = registry->get<component::GuiRenderable>(guiEntity);
						auto& interactionType = registry->get<PlayerInteractionType>(guiEntity);

						if (interactionType.type == evt.interactionType)
						{
							renderable.enabled = true;
							break;
						}
					}

					break;
				}
			}

			break;
		}
	}

	return false;
}