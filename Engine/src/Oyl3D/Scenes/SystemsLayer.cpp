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

    void SystemsLayer::onUpdate() {}

    void SystemsLayer::onGuiRender() {}

    bool SystemsLayer::onEvent(const Event& event) { return false; }
}
