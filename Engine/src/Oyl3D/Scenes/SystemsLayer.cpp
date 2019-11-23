#include "oylpch.h"
#include "SystemsLayer.h"

#include "ECS/SystemImpl.h"

namespace oyl::internal
{
    void SystemsLayer::onEnter()
    {
        scheduleSystemUpdate<TransformUpdateSystem>();
        scheduleSystemUpdate<PhysicsSystem>();
        scheduleSystemUpdate<AnimationSystem>();
    }

    void SystemsLayer::onExit() {}

    void SystemsLayer::onUpdate(Timestep dt) {}

    void SystemsLayer::onGuiRender(Timestep dt) {}

    bool SystemsLayer::onEvent(Ref<Event> event) { return false; }
}
