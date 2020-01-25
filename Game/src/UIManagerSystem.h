#pragma once
#include <Oyl3D.h>

#include "CustomEvents.h"
#include "CustomComponents.h"

using namespace oyl;

class UIManagerSystem : public System
{
private:
	OYL_CTOR(UIManagerSystem, System)

	void onEnter() override;
	void onExit() override;
	void onUpdate() override;
	bool onEvent(const Event& event) override;

	void enableValidUIType(PlayerInteractionResult a_type, PlayerNumber a_playerNum);
	void enableInvalidUIType(PlayerInteractionResult a_type, PlayerNumber a_playerNum);
};