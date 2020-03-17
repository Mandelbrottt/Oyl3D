#include "PersistentVariables.h"

GameEndResult PersistentVariables::gameResult = GameEndResult::tie;

int PersistentVariables::controllerSensitivities[4] = { 5, 5, 5, 5 };

MenuOption PersistentVariables::mainMenuSelectedOption = MenuOption::playGame;

PlayerNumber PersistentVariables::activePlayerNum = PlayerNumber::One;