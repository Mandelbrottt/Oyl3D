#pragma once

#include <Oyl3D.h>

#include "Cannon.h"
#include "Player.h"

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
	void onUpdate(Timestep dt) override;
	bool onEvent(Ref<Event> event) override;

	void checkForAnyValidPlayerInteractions(entt::entity a_playerEntity, Player* a_player, component::Transform* a_playerTransform);
	void validateInteraction(entt::entity a_playerEntity, Player* a_player, component::Transform* a_playerTransform);
	CannonInteractionOutcome validateRaycastHit(Player* a_player, Cannon* a_cannon);
};