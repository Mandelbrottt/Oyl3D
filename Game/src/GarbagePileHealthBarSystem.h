#pragma once
#include "CustomEvents.h"
#include "CustomComponents.h"

using namespace oyl;

class GarbagePileHealthBarSystem : public System
{
	OYL_CTOR(GarbagePileHealthBarSystem, System)

	void onEnter() override;
	void onExit() override;
	void onUpdate() override;
	bool onEvent(const Event& event) override;
};