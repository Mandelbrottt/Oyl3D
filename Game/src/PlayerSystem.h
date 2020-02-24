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
	void changeToIdle(Player* a_player);
	void changeToWalking(Player* a_player);
	void changeToJumping(Player* a_player);
	void changeToFalling(Player* a_player);
	void changeToPushing(Player* a_player);
	void changeToInCleaningQuicktimeEvent(Player* a_player);
	void changeToCleaning(Player* a_player);
	void changeToThrowingBottle(Player* a_player);
	void changeToStunned(Player* a_player);

	void performBasicMovement(entt::entity a_playerEntity, const float a_speedForce);
};