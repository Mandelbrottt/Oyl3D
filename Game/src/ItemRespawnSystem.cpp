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

	entt::entity entityToCopy = entt::null;

	bool isThereAnInactiveRespawnable = false;

	auto respawnableItemsView = registry->view<Respawnable, CarryableItem, component::Transform>();
	for (auto& respawnableEntity : respawnableItemsView)
	{
		auto& carryableItem = registry->get<CarryableItem>(respawnableEntity);

		//search for an item we can copy
		if (carryableItem.type == respawnManager.type && carryableItem.team == respawnManager.team)
		{
			entityToCopy = respawnableEntity;

			if (!carryableItem.isActive)
			{
				auto& respawnable   = registry->get<Respawnable>(respawnableEntity);
				auto& itemTransform = registry->get<component::Transform>(respawnableEntity);

				isThereAnInactiveRespawnable = true;
				//spawn the item back into the game
				carryableItem.isActive       = true;
				carryableItem.isBeingCarried = false;
				carryableItem.hasBeenCarried = false;

				itemTransform.setPosition(respawnable.spawnPosition);
				itemTransform.setRotation(respawnable.spawnRotation);

				break;
			}
		}
	}

	//if there were no inactive items to reuse, spawn a new one
	if (!isThereAnInactiveRespawnable)
	{
		//spawn the item
		auto newEntity = registry->create();
		registry->stomp(newEntity, entityToCopy, *registry);

		auto& newCarryableItem = registry->get<CarryableItem>(newEntity);
		auto& newRespawnable   = registry->get<Respawnable>(newEntity);
		auto& newTransform     = registry->get<component::Transform>(newEntity);
		auto& newEntityInfo    = registry->get<component::EntityInfo>(newEntity);

		newCarryableItem.isActive       = true;
		newCarryableItem.hasBeenCarried = false;
		newCarryableItem.isBeingCarried = false;

		newTransform.setPosition(newRespawnable.spawnPosition);
		newTransform.setRotation(newRespawnable.spawnRotation);

		std::string itemTeamName = respawnManager.team == Team::blue ? "Blue" : "Red";

		std::string itemTypeName;
		if (newCarryableItem.type == CarryableItemType::cleaningSolution)
			itemTypeName = "CleaningSolution";
		else if (newCarryableItem.type == CarryableItemType::gloop)
			itemTypeName = "Gloop";

		newEntityInfo.name = itemTeamName + itemTypeName + std::to_string(respawnableItemsView.size());
	}
}