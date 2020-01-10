#pragma once

#include <Oyl3D.h>
#include "CustomComponents.h"
#include "CustomEvents.h"

using namespace oyl;

enum class CannonInteractionOutcome
{
    invalid,
    loadCannon,
    push,
    cannonFiringSoon
};

class PlayerInteractionValidationSystem : public System
{
private:
	OYL_CTOR(PlayerInteractionValidationSystem, System)

	void onEnter() override;
	void onExit() override;
	void onUpdate() override;
	bool onEvent(const Event& event) override;

	void checkForAnyValidPlayerInteractions(entt::entity a_playerEntity);
	void validateInteraction(entt::entity a_playerEntity);
	CannonInteractionOutcome validateRaycastHit(Player* a_player, Cannon* a_cannon);
	void dropPlayerCarriedItems(entt::entity a_playerEntity, bool dropSpecificItemType = false, CarryableItemType itemTypeToDrop = CarryableItemType::invalid);
};