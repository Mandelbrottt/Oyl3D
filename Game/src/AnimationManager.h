#pragma once
#include <Oyl3D.h>

#include "CustomEvents.h"
#include "CustomComponents.h"

using namespace oyl;

enum AnimationProperties : int
{
	Pirax,
	Gloop,
	Cannonball,
	Mop,
	ThrowableBottle,
	EmptyL,
	EmptyR,
	None,
	MopUse,
};

class AnimationManager : public System
{
public:
	bool gloopLast = false;

	void setAnimationEntities(oyl::Event event);
	void setAnimationProperties(std::string tag, AnimationProperties type, bool loop);
	//void clearEntities();

	entt::entity playerCamera{};
	entt::entity playerArmR{};
	entt::entity playerArmL{};
	entt::entity playerArmRTarget{};
	entt::entity playerArmRObject{};
	entt::entity playerArmLTarget{};
	entt::entity playerArmLObject{};

private:
	OYL_CTOR(AnimationManager, System)

		void onEnter() override;
	void onExit() override;
	void onUpdate() override;
	bool onEvent(const Event& event) override;




};