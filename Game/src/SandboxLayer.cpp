#include <Oyl3D.h>

#include "SandboxLayer.h"

using namespace oyl;

void SandboxLayer::onAttach()
{
    scheduleSystemUpdate<ECS::OracleCameraSystem>();

    addToEventMask(TypeKeyPressed);
    addToEventMask(TypeGamepadStickMoved);

    auto shader = Shader::create({
        { VertexShader, "../Engine/res/staticLighting.vert" },
        { FragmentShader, "../Engine/res/staticLighting.frag" },
    });

    auto mesh = Mesh::create("res/monkey.obj");

    auto mat = Material::create(shader);

    Component::MeshRenderer mr;
    mr.mesh     = mesh;
    mr.material = mat;

    Component::Transform t;
    Entity e;
    for (int i = 0; i < 10; i++)
    {
        e = registry->create();
        registry->assign<Component::MeshRenderer>(e, mr);

        t.position = glm::vec3(i, 0.0f, -2.0f);
        registry->assign<Component::Transform>(e, t);
    }

    mr.mesh = Mesh::create("res/cube.obj");
    mr.material->loadTexture("res/capsule0.jpg");
    
    e = registry->create();
    registry->assign<Component::MeshRenderer>(e, mr);

    t.position = glm::vec3(2.0f, 2.0f, 0.0f);
    t.scale    = glm::vec3(0.2f);
    registry->assign<Component::Transform>(e, t);
}

void SandboxLayer::onUpdate(Timestep dt)
{
}

void SandboxLayer::onGuiRender()
{
}

bool SandboxLayer::onEvent(Ref<Event> event)
{
    return false;
}
