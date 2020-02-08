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
	if (isGameOver)
		return;

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
		GameEndEvent gameEnd;

		if (numMaxedPilesRedTeam == 3)
			gameEnd.result = GameEndResult::tie;
		else //red team isn't maxed out
			gameEnd.result = GameEndResult::redWin;

		postEvent(gameEnd);

		isGameOver = true;
	}
	else if (numMaxedPilesRedTeam == 3)
	{
		GameEndEvent gameEnd;
		gameEnd.result = GameEndResult::blueWin;
		postEvent(gameEnd);

		isGameOver = true;
	}
}

bool GameOverCheckSystem::onEvent(const Event& event)
{

	return false;
}