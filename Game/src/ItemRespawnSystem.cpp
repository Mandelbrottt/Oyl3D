#include "ItemRespawnSystem.h"

void ItemRespawnSystem::onEnter()
{

}

void ItemRespawnSystem::onExit()
{

}

void ItemRespawnSystem::onUpdate()
{
	auto respawnManagerView = registry->view<RespawnManager>();
	for (auto& respawnManagerEntity : respawnManagerView)
	{
		auto& respawnManager = registry->get<RespawnManager>(respawnManagerEntity);

		if (respawnManager.isRespawnTimerActive)
		{
			respawnManager.respawnTimerCountdown -= Time::deltaTime();
			if (respawnManager.respawnTimerCountdown < 0.0f)
			{
				spawnItem(respawnManagerEntity);
				respawnManager.isRespawnTimerActive = false;
			}
		}
		else //respawn timer is not active
		{
			bool areAnyItemsInSpawn = false;

			auto respawnablesView = registry->view<Respawnable, CarryableItem>();
			for (auto& respawnableEntity : respawnablesView)
			{
				auto& carryableItem = registry->get<CarryableItem>(respawnableEntity);

				if (   !carryableItem.hasBeenCarried 
					&& carryableItem.type == respawnManager.type 
					&& carryableItem.team == respawnManager.team)
				{
					areAnyItemsInSpawn = true;
					break;
				}
			}

			if (!areAnyItemsInSpawn)
			{
				respawnManager.respawnTimerCountdown = respawnManager.respawnTimerDuration;
				respawnManager.isRespawnTimerActive = true;
			}
		}
	}
}

bool ItemRespawnSystem::onEvent(const Event& event)
{
	return false;
}

void ItemRespawnSystem::spawnItem(entt::entity a_respawnManagerEntity)
{
	auto& respawnManager = registry->get<RespawnManager>(a_respawnManagerEntity);

	//spawn the new item
	auto newEntity = registry->create();
	registry->stomp(newEntity, respawnManager.entityPrefab, *registry);

	auto& newCarryableItem = registry->get<CarryableItem>(newEntity);
	auto& newRespawnable   = registry->get<Respawnable>(newEntity);
	auto& newTransform     = registry->get<component::Transform>(newEntity);
	auto& newRigidbody     = registry->get<component::RigidBody>(newEntity);
	auto& newEntityInfo    = registry->get<component::EntityInfo>(newEntity);

	newCarryableItem.hasBeenCarried = false;

	newTransform.setPosition(newRespawnable.spawnPosition);
	newTransform.setRotation(newRespawnable.spawnRotation);

	//reset acceleration/velocity for the newly spawned item so it doesnt go flying down
	newRigidbody.setAcceleration(glm::vec3(0.0f)); 
	newRigidbody.setVelocity(glm::vec3(0.0f));

	std::string itemTeamName = respawnManager.team == Team::blue ? "Blue" : "Red";

	std::string itemTypeName;
	if (newCarryableItem.type == CarryableItemType::cleaningSolution)
		itemTypeName = "CleaningSolution";
	else if (newCarryableItem.type == CarryableItemType::gloop)
		itemTypeName = "Gloop";

	auto respawnableItemsView = registry->view<Respawnable, CarryableItem, component::Transform>();
	newEntityInfo.name = itemTeamName + itemTypeName + std::to_string(respawnableItemsView.size());
}