#include "GarbageTickSystem.h"

void GarbageTickSystem::onEnter()
{

}

void GarbageTickSystem::onExit()
{

}

void GarbageTickSystem::onUpdate(Timestep dt)
{
	auto garbageTickView = registry->view<component::Transform, GarbageTick>();
	for (auto& garbageTickEntity : garbageTickView)
	{
		auto& garbageTick = registry->get<GarbageTick>(garbageTickEntity);
		auto& garbageTickTransform = registry->get<component::Transform>(garbageTickEntity);

		garbageTick.onScreenCountdown -= dt;
		if (garbageTick.onScreenCountdown < 0.0f)
			garbageTickTransform.setPosition(glm::vec3(100.0f));
	}
}

bool GarbageTickSystem::onEvent(Ref<Event> event)
{
	return false;
}