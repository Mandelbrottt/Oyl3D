#include "GameOverCheckSystem.h"

void GameOverCheckSystem::onEnter()
{
	isGameOver = false;
}

void GameOverCheckSystem::onExit()
{

}

void GameOverCheckSystem::onUpdate()
{
	if (startGameEndTimer)
	{
		waitBeforeEndingCountdown -= Time::deltaTime();
		if (waitBeforeEndingCountdown < 0.0f && !isGameOver)
		{
			GameEndEvent gameEnd;
			gameEnd.result = gameResult;
			postEvent(gameEnd);

			isGameOver = true;
		}
	}

	int numMaxedPilesBlueTeam = 0;
	int numMaxedPilesRedTeam  = 0;

	auto garbagePileView = registry->view<GarbagePile, component::Renderable, component::Transform>();
	for (auto& garbagePileEntity : garbagePileView)
	{
		auto& garbagePile = registry->get<GarbagePile>(garbagePileEntity);

		if (garbagePile.garbageLevel == garbagePile.MAX_GARBAGE_LEVEL && garbagePile.garbageTicks == garbagePile.GARBAGE_TICKS_PER_LEVEL)
		{
			if (garbagePile.team == Team::blue)
				numMaxedPilesBlueTeam++;
			else //red team
				numMaxedPilesRedTeam++;
		}
	}

	if (numMaxedPilesBlueTeam == 3)
	{
		if (numMaxedPilesRedTeam == 3)
			gameResult = GameEndResult::tie;
		else //red team isn't maxed out
			gameResult = GameEndResult::redWin;

		startGameEndTimer = true;
	}
	else if (numMaxedPilesRedTeam == 3)
	{
		gameResult = GameEndResult::blueWin;

		startGameEndTimer = true;
	}
}

bool GameOverCheckSystem::onEvent(const Event& event)
{

	return false;
}