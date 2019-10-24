#include <Oyl3D.h>

#include "SandboxLayer.h"

using namespace oyl;

void SandboxLayer::onEnter()
{
    scheduleSystemUpdate<ECS::OracleCameraSystem>();

    addToEventMask(TypeKeyPressed);
    addToEventMask(TypeGamepadStickMoved);

    Component::MeshRenderer mr;
    mr.mesh     = Mesh::get(MONKEY_MESH_ALIAS);
    mr.material = Material::get("monkeyMat");

    Component::Transform t;
    Entity e;
    for (int i = 0; i < 10; i++)
    {
        e = registry->create();
        registry->assign<Component::MeshRenderer>(e, mr);

        t.position = glm::vec3(i, 0.0f, -2.0f);
        registry->assign<Component::Transform>(e, t);
    }

    mr.mesh     = Mesh::get(CUBE_MESH_ALIAS);
    mr.material = Material::get("cubeMat");
    
    e = registry->create();
    registry->assign<Component::MeshRenderer>(e, mr);

    t.position = glm::vec3(2.0f, 2.0f, 0.0f);
    t.scale    = glm::vec3(0.2f);
    registry->assign<Component::Transform>(e, t);
}

void SandboxLayer::onUpdate(Timestep dt)
{
}

void SandboxLayer::onGuiRender(Timestep dt)
{
}

bool SandboxLayer::onEvent(Ref<Event> event)
{
    return false;
}
