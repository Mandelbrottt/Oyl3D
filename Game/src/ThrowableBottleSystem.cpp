#include "ThrowableBottleSystem.h"

void ThrowableBottleSystem::onEnter()
{
	this->listenForEventCategory((EventCategory)CategoryThrowableBottle);
	this->listenForEventCategory((EventCategory)CategoryPlayer);
}

void ThrowableBottleSystem::onExit()
{

}

void ThrowableBottleSystem::onUpdate()
{

}

bool ThrowableBottleSystem::onEvent(const Event& event)
{
	switch (event.type)
	{
		case (EventType)TypeThrowBottle:
		{
			/*auto evt = event_cast<ThrowBottleEvent>(event);

			auto& bottle   = registry->get<ThrowableBottle>(evt.bottleEntity);
			auto& bottleRB = registry->get<component::RigidBody>(evt.bottleEntity);

			bottle.playerThrowingEntity = evt.playerThrowingEntity;

			auto& player = registry->get<Player>(evt.playerThrowingEntity);
			auto cameraView = registry->view<component::Camera>();
			for (auto cameraEntity : cameraView)
			{
				auto& camera          = registry->get<component::Camera>(cameraEntity);
				auto& cameraTransform = registry->get<component::Transform>(cameraEntity);

				if (camera.player == player.playerNum)
				{
					bottleRB.addImpulse(cameraTransform.getForwardGlobal() * bottle.throwSpeed);
					break;
				}
			}

			break;*/
		}
		
		/*case (EventType)TypePlayerInteractionRequest:
		{
			auto evt = event_cast<PlayerInteractionRequestEvent>(event);
			auto& player = registry->get<Player>(evt.playerEntity);

			if (   registry->valid(player.primaryCarriedItem)
				&& registry->get<CarryableItem>(player.primaryCarriedItem).type == CarryableItemType::throwableBottle
				&& evt.itemClassifiatonToUse == PlayerItemClassifiation::primary)
			{
				auto& bottle   = registry->get<ThrowableBottle>(player.primaryCarriedItem);
				auto& bottleRB = registry->get<component::RigidBody>(player.primaryCarriedItem);

				registry->get<component::Parent>(player.primaryCarriedItem).parent     = entt::null;
				registry->get<CarryableItem>(player.primaryCarriedItem).isBeingCarried = false;

				player.primaryCarriedItem = entt::null;

				bottle.playerThrowingEntity = evt.playerEntity;

				auto cameraView = registry->view<component::Camera>();
				for (auto cameraEntity : cameraView)
				{
					auto& camera          = registry->get<component::Camera>(cameraEntity);
					auto& cameraTransform = registry->get<component::Transform>(cameraEntity);

					if (camera.player == player.playerNum)
					{
						bottleRB.addImpulse(cameraTransform.getForwardGlobal() * bottle.throwSpeed);
						break;
					}
				}
			}

			break;
		}*/

		//case EventType::PhysicsCollisionStay:
		//{
		//	auto evt = event_cast<PhysicsCollisionStayEvent>(event);

		//	entt::entity bottleEntity = entt::null;
		//	entt::entity playerEntity = entt::null;

		//	if (registry->has<ThrowableBottle>(evt.entity1))
		//		bottleEntity = evt.entity1;
		//	else if (registry->has<ThrowableBottle>(evt.entity2))
		//		bottleEntity = evt.entity2;
		//	else //no bottle involved in the collision
		//		break;

		//	auto& bottle = registry->get<ThrowableBottle>(bottleEntity);

		//	if (bottle.isBeingThrown)
		//	{
		//		if (registry->has<Player>(evt.entity1))
		//			playerEntity = evt.entity1;
		//		else if (registry->has<Player>(evt.entity2))
		//			playerEntity = evt.entity2;
		//		else //no player involved in the collision
		//		{
		//			registry->destroy(bottleEntity); //the bottle breaks
		//			break;
		//		}

		//		if (bottle.playerThrowingEntity != playerEntity)
		//		{
		//			//bottle stuns the player hit

		//		}
		//	}

		//	break;
		//}
	}

	return false;
}