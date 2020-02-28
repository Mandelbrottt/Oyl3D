#pragma once

#include "CustomEvents.h"
#include "CustomComponents.h"

using namespace oyl;

//this system checks if the game is over and should end
class GameOverCheckSystem : public System
{
	OYL_CTOR(GameOverCheckSystem, System)

	bool isGameOver        = false;
	bool startGameEndTimer = false;

	float waitBeforeEndingCountdown = 1.5f;

	GameEndResult gameResult;

	void onEnter() override;
	void onExit() override;
	void onUpdate() override;
	bool onEvent(const Event& event) override;
};
