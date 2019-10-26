#include <Oyl3D.h>

#include "SandboxLayer.h"

using namespace oyl;

void SandboxLayer::onEnter()
{
    addToEventMask(TypeKeyPressed);
    addToEventMask(TypeGamepadStickMoved);

    component::Renderable mr;
    mr.mesh     = Mesh::get(MONKEY_MESH_ALIAS);
    mr.material = Material::get("monkeyMat");

    component::Transform t;
    Entity e;
    for (int i = 0; i < 10; i++)
    {
        e = registry->create();
        registry->assign<component::Renderable>(e, mr);

        t.position = glm::vec3(3 * i - 15, 0.0f, -2.0f);
        registry->assign<component::Transform>(e, t);

        auto& so = registry->assign<component::SceneObject>(e);
        so.name = "Custom Name " + std::to_string(i);
    }

    mr.mesh     = Mesh::get(CUBE_MESH_ALIAS);
    mr.material = Material::get("cubeMat");
    
    e = registry->create();
    registry->assign<component::Renderable>(e, mr);

    t.position = glm::vec3(2.0f, 2.0f, 0.0f);
    t.scale    = glm::vec3(0.2f);
    registry->assign<component::Transform>(e, t);
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
