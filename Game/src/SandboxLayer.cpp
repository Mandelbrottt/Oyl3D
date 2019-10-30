#include <Oyl3D.h>

#include "SandboxLayer.h"

using namespace oyl;

void SandboxLayer::onEnter()
{    
    auto mesh = Mesh::cache("res/assets/models/cube.obj");
    
    auto mat = Material::cache(Shader::get(LIGHTING_SHADER_ALIAS), 
                               Texture2D::cache("res/assets/textures/container2.jpg"), 
                               "container");

    mat->setSpecularMap(Texture2D::cache("res/assets/textures/container2_specular.jpg"));

    {
        component::Renderable mr;
        mr.mesh     = mesh;
        mr.material = mat;

        Entity e = registry->create();
        registry->assign<component::Renderable>(e, mr);

        component::Transform t;
        t.position = glm::vec3(0.0f);
        registry->assign<component::Transform>(e, t);

        auto& so = registry->assign<component::SceneObject>(e);
        so.name = "Container";
    }
    {
        component::Renderable mr;
        mr.mesh = mesh;
        mr.material = mat;

        Entity e = registry->create();
        registry->assign<component::Renderable>(e, mr);

        component::Transform t;
        t.position = glm::vec3(3.0f, 3.0f, 3.0f);
        t.scale = glm::vec3(0.3f);
        registry->assign<component::Transform>(e, t);

        auto& l = registry->assign<component::PointLight>(e);

        auto& so = registry->assign<component::SceneObject>(e);
        so.name = "Light 1";
    }
    {
        component::Renderable mr;
        mr.mesh = Mesh::cache("res/assets/models/plane.obj");
        mr.material = mat;

        Entity e = registry->create();
        registry->assign<component::Renderable>(e, mr);

        component::Transform t;
        t.position = glm::vec3(0.0f, -5.0f, 0.0f);
        registry->assign<component::Transform>(e, t);

        auto& so = registry->assign<component::SceneObject>(e);
        so.name = "Plane";

        auto& rb = registry->assign<component::RigidBody>(e);
        rb.type = RigidBody_StaticPlane;
    }
    {
        component::Renderable mr;
        mr.mesh = Mesh::cache("res/assets/models/sphere.obj");
        mr.material = mat;

        Entity e = registry->create();
        registry->assign<component::Renderable>(e, mr);

        component::Transform t;
        t.position = glm::vec3(-3.0f, -1.0f, -2.0f);
        registry->assign<component::Transform>(e, t);

        auto& so = registry->assign<component::SceneObject>(e);
        so.name = "Sphere";

        auto& rb = registry->assign<component::RigidBody>(e);
        rb.type   = RigidBody_Sphere;
        rb.radius = 1.0f;
        rb.mass   = 1.0f;
    }
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
