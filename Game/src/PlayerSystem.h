#pragma once
#include "CustomEvents.h"
#include "CustomComponents.h"

using namespace oyl;

class PlayerSystem : public System
{
	OYL_CTOR(PlayerSystem, System)

	void onEnter() override;
	void onExit() override;
	void onUpdate() override;
	bool onEvent(const Event& event) override;

	//state changing functions
	void changeToIdle(entt::entity a_playerEntity);
	void changeToWalking(entt::entity a_playerEntity);
	void changeToJumping(entt::entity a_playerEntity);
	void changeToFalling(entt::entity a_playerEntity);
	void changeToPushing(entt::entity a_playerEntity);
	void changeToInCleaningQuicktimeEvent(entt::entity a_playerEntity);
	void changeToCleaning(entt::entity a_playerEntity);
	void changeToThrowingBottle(entt::entity a_playerEntity);
	void changeToStunned(entt::entity a_playerEntity);

	void performBasicMovement(entt::entity a_playerEntity, const float a_speedForce);
};