#pragma once
#include <Oyl3D.h>

#include "CustomEvents.h"
#include "CustomComponents.h"

using namespace oyl;

class AnimationManager : public System
{
private:
	OYL_CTOR(AnimationManager, System)

		void onEnter() override;
	void onExit() override;
	void onUpdate() override;
	bool onEvent(const Event& event) override;
};