#pragma once

#include "CustomComponents.h"

class PersistentVariables
{
public:
	static MenuOption mainMenuSelectedOption;
	static GameEndResult gameResult;
	static int controllerSensitivities[4];
	static PlayerNumber activePlayerNum; //TODO: REMOVE FOR FINAL VERSION, THIS IS ONLY FOR TESTING
};