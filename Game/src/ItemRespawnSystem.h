#pragma once

#include "Oyl3D.h"
#include "CustomEvents.h"
#include "CustomComponents.h"

using namespace oyl;

class ItemRespawnSystem : public System
{
	OYL_CTOR(ItemRespawnSystem, System)

	void onEnter() override;
	void onExit() override;
	void onUpdate() override;
	bool onEvent(const Event& event) override;

	void spawnItem(entt::entity a_respawnManagerEntity);
};