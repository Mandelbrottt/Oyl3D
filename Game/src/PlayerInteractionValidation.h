#pragma once

#include <Oyl3D.h>
#include "CustomComponents.h"
#include "CustomEvents.h"

using namespace oyl;

class PlayerInteractionValidationSystem : public System
{
private:
	OYL_CTOR(PlayerInteractionValidationSystem, System)

	void onEnter() override;
	void onExit() override;
	void onUpdate() override;
	bool onEvent(const Event& event) override;

	void performRaycastAndValidateForPlayer(entt::entity a_playerEntity);
	void validateCarryableItemInteraction(entt::entity a_playerEntity, entt::entity a_carryableItemEntity);
	void validateGarbagePileInteraction(entt::entity a_playerEntity, entt::entity a_garbagePileEntity);
	void validateCannonballCrateInteraction(entt::entity a_playerEntity, entt::entity a_cannonballCrateEntity);
	void validateCannonInteraction(entt::entity a_playerEntity, entt::entity a_cannonEntity);

	void performInteractionForPlayer(entt::entity a_playerEntity, PlayerItemClassification itemClassification);
	void performCarryableItemInteraction(entt::entity a_playerEntity, entt::entity a_carryableItemEntity, PlayerItemClassification itemClassification);
	void performGarbagePileInteraction(entt::entity a_playerEntity, entt::entity a_garbagePileEntity, PlayerItemClassification itemClassification);
	void performCannonballCrateInteraction(entt::entity a_playerEntity, entt::entity a_cannonballCrateEntity, PlayerItemClassification itemClassification);
	void performCannonInteraction(entt::entity a_playerEntity, entt::entity a_cannonEntity, PlayerItemClassification itemClassification);

	void dropPlayerCarriedItems(entt::entity a_playerEntity, bool dropSpecificItemType = false, CarryableItemType itemTypeToDrop = CarryableItemType::invalid);
};