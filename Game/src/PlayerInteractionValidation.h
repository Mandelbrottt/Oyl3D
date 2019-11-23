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

	void validateInteraction(Player* a_player, const component::Transform& a_playerTransform);
	CannonInteractionOutcome validateRaycastHit(Player* a_player, Cannon* a_cannon);
};