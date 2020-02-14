#pragma once
#include "CustomEvents.h"
#include "CustomComponents.h"

using namespace oyl;

class CannonballSystem : public System
{
	OYL_CTOR(CannonballSystem, System)

	std::vector<glm::vec3> blueFrontSpline;
	std::vector<glm::vec3> blueMiddleSpline;
	std::vector<glm::vec3> blueBackSpline;

	std::vector<glm::vec3> redFrontSpline;
	std::vector<glm::vec3> redMiddleSpline;
	std::vector<glm::vec3> redBackSpline;

	void onEnter() override;
	void onExit() override;
	void onUpdate() override;
	bool onEvent(const Event& event) override;

	void setCannonballToCarriedForPlayer(entt::entity a_playerEntity, entt::entity a_cannonballEntity);

	void initFiringSplines();
};