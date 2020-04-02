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
	auto bottleView = registry->view<ThrowableBottle, CarryableItem>();
	for (auto bottleEntity : bottleView)
	{
		auto& bottle          = registry->get<ThrowableBottle>(bottleEntity);
		auto& bottleTransform = registry->get<component::Transform>(bottleEntity);
		auto& bottleCarryable = registry->get<CarryableItem>(bottleEntity);

		if (bottle.isBeingThrown)
			bottleTransform.rotate(glm::vec3(-1100.0f, 0.0f, 0.0f) * Time::deltaTime());
	}

	//figure out if any "throw bottle" prompts need to be displayed (displayed when player is carrying a throwable bottle
	auto bottlePromptView = registry->view<ThrowBottlePrompt, PlayerHUDElement>();
	for (auto bottlePromptEntity : bottlePromptView)
	{
		bool shouldBeShown = false;

		auto& promptTransform  = registry->get<component::Transform>(bottlePromptEntity);
		auto& promptHudElement = registry->get<PlayerHUDElement>(bottlePromptEntity);

		auto playerView = registry->view<Player>();
		for (auto playerEntity : playerView)
		{
			auto& player = registry->get<Player>(playerEntity);

			if (player.playerNum != promptHudElement.playerNum)
				continue;

			if (registry->valid(player.primaryCarriedItem) && registry->has<ThrowableBottle>(player.primaryCarriedItem))
				shouldBeShown = true;
		}

		if (shouldBeShown)
			promptTransform.setPosition(promptHudElement.positionWhenActive);
		else
			promptTransform.setPositionX(-30.0f);
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
			auto& bottleCarryable = registry->get<CarryableItem>(evt.bottleEntity);

			bottleTransform.setParent(entt::null);
			bottleCarryable.isBeingCarried = false;

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

			registry->get<component::Renderable>(evt.bottleEntity).cullingMask = 0b1111; //TODO: REMOVE ONCE 3PP ANIMATIONS ARE IN

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

			auto& bottle          = registry->get<ThrowableBottle>(bottleEntity);
			auto& bottleTransform = registry->get<component::Transform>(bottleEntity);

			if (bottle.isBeingThrown)
			{
				if (registry->has<Player>(evt.entity1))
					playerEntity = evt.entity1;
				else if (registry->has<Player>(evt.entity2))
					playerEntity = evt.entity2;
				else //no player involved in the collision
				{
					registry->destroy(bottleEntity); //the bottle breaks

					ThrowableBottleHitEvent bottleHit;
					bottleHit.bottleEntity = bottleEntity;
					bottleHit.hitPlayer    = false;
					postEvent(bottleHit);

					break;
				}

				if (bottle.playerThrowingEntity != playerEntity)
				{
					auto& player          = registry->get<Player>(playerEntity);
					auto& playerTransform = registry->get<component::Transform>(playerEntity);
					auto& playerRB        = registry->get<component::RigidBody>(playerEntity);

					OYL_LOG("PLAYER {} WAS HIT BY A BOTTLE!", (int)player.playerNum + 1);

					//apply knockback when hit by bottle
					glm::vec3 forceDirection = playerTransform.getPositionGlobal() - bottleTransform.getPositionGlobal();
					forceDirection.y = 0.0f;
					playerRB.addImpulse(forceDirection * 140.0f);

					registry->destroy(bottleEntity); //the bottle breaks

					ThrowableBottleHitEvent bottleHit;
					bottleHit.bottleEntity = bottleEntity;
					bottleHit.hitPlayer    = true;
					postEvent(bottleHit);

					//bottle stuns the player hit and makes them drop their carried items
					PlayerDropItemRequestEvent playerDropItemRequest;
					playerDropItemRequest.playerEntity             = playerEntity;
					playerDropItemRequest.itemClassificationToDrop = PlayerItemClassification::any;
					playerDropItemRequest.forceDrop                = true;
					postEvent(playerDropItemRequest);

					PlayerStateChangeRequestEvent playerStateChange;
					playerStateChange.playerEntity = playerEntity;
					playerStateChange.newState     = PlayerState::stunned;
					postEvent(playerStateChange);

					GamepadVibrationRequestEvent gamepadVibration;
					gamepadVibration.gid = player.controllerNum;
					gamepadVibration.leftTime   = 0.38f;
					gamepadVibration.rightTime  = 0.38f;
					gamepadVibration.leftMotor  = 1.0f;
					gamepadVibration.rightMotor = 1.0f;
					postEvent(gamepadVibration);
				}
			}

			break;
		}
	}

	return false;
}