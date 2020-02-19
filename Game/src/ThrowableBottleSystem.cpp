#include "ThrowableBottleSystem.h"

void ThrowableBottleSystem::onEnter()
{
	listenForEventCategory((EventCategory)CategoryThrowableBottle);
	listenForEventType(EventType::PhysicsCollisionStay);
}

void ThrowableBottleSystem::onExit()
{

}

void ThrowableBottleSystem::onUpdate()
{
	auto bottleView = registry->view<ThrowableBottle>();
	for (auto bottleEntity : bottleView)
	{
		auto& bottle          = registry->get<ThrowableBottle>(bottleEntity);
		auto& bottleTransform = registry->get<component::Transform>(bottleEntity);

		if (bottle.isBeingThrown)
			bottleTransform.rotate(bottleTransform.getRight() * -100.0f);
	}
}

bool ThrowableBottleSystem::onEvent(const Event& event)
{
	switch (event.type)
	{
		case (EventType)TypeThrowBottle:
		{
			auto evt = event_cast<ThrowBottleEvent>(event);

			auto& bottle          = registry->get<ThrowableBottle>(evt.bottleEntity);
			auto& bottleTransform = registry->get<component::Transform>(evt.bottleEntity);
			auto& bottleRB        = registry->get_or_assign<component::RigidBody>(evt.bottleEntity); //add the rigidbody back

			registry->get<component::Parent>(evt.bottleEntity).parent     = entt::null;
			registry->get<CarryableItem>(evt.bottleEntity).isBeingCarried = false;

			bottle.isBeingThrown        = true;
			bottle.playerThrowingEntity = evt.playerThrowingEntity;

			auto& player          = registry->get<Player>(evt.playerThrowingEntity);
			auto& playerTransform = registry->get<component::Transform>(evt.playerThrowingEntity);

			glm::vec3 newPosition = playerTransform.getPosition();
			newPosition += playerTransform.getForward() * 0.7f;
			newPosition += playerTransform.getRight()   * 0.25f;
			newPosition += playerTransform.getUp()      * 0.8f;

			bottleTransform.setPosition(newPosition);
			bottleTransform.setRotation(playerTransform.getRotation());

			auto cameraView = registry->view<component::Camera>();
			for (auto cameraEntity : cameraView)
			{
				auto& camera          = registry->get<component::Camera>(cameraEntity);
				auto& cameraTransform = registry->get<component::Transform>(cameraEntity);

				if (camera.player == player.playerNum)
				{
					glm::vec3 throwDir = normalize(cameraTransform.getForwardGlobal() + glm::vec3(0.0f, 0.2f, 0.0f));
					bottleRB.addImpulse(throwDir * bottle.throwSpeed);
					break;
				}
			}

			break;
		}

		case EventType::PhysicsCollisionStay:
		{
			auto evt = event_cast<PhysicsCollisionStayEvent>(event);

			//ensure the entities in the collision are valid
			if (!registry->valid(evt.entity1) || !registry->valid(evt.entity2))
				break;

			entt::entity bottleEntity = entt::null;
			entt::entity playerEntity = entt::null;

			if (registry->has<ThrowableBottle>(evt.entity1))
				bottleEntity = evt.entity1;
			else if (registry->has<ThrowableBottle>(evt.entity2))
				bottleEntity = evt.entity2;
			else //no bottle involved in the collision
				break;

			auto& bottle = registry->get<ThrowableBottle>(bottleEntity);

			if (bottle.isBeingThrown)
			{
				if (registry->has<Player>(evt.entity1))
					playerEntity = evt.entity1;
				else if (registry->has<Player>(evt.entity2))
					playerEntity = evt.entity2;
				else //no player involved in the collision
				{
					registry->destroy(bottleEntity); //the bottle breaks
					break;
				}

				if (bottle.playerThrowingEntity != playerEntity)
				{
					auto& player = registry->get<Player>(playerEntity);
					OYL_LOG("PLAYER {} WAS HIT BY A BOTTLE!", (int)player.playerNum + 1);
					registry->destroy(bottleEntity); //the bottle breaks

					//bottle stuns the player hit and makes them drop their carried items
					PlayerDropItemRequestEvent playerDropItemRequest;
					playerDropItemRequest.playerEntity             = playerEntity;
					playerDropItemRequest.itemClassificationToDrop = PlayerItemClassifiation::any;
					postEvent(playerDropItemRequest);

					PlayerStateChangeEvent playerStateChange;
					playerStateChange.playerEntity = playerEntity;
					playerStateChange.newState     = PlayerState::stunned;
					postEvent(playerStateChange);
				}
			}

			break;
		}
	}

	return false;
}