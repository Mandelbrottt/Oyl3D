#pragma once
#include "CustomEvents.h"
#include "CustomComponents.h"

using namespace oyl;

class CannonballSystem : public System
{
	OYL_CTOR(CannonballSystem, System)

	void onEnter() override;
	void onExit() override;
	void onUpdate() override;
	bool onEvent(const Event& event) override;

	void setCannonballToCarriedForPlayer(entt::entity a_playerEntity, entt::entity a_cannonballEntity);
};