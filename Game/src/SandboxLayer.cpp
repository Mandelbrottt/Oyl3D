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

        entt::entity e = registry->create();
        registry->assign<component::Renderable>(e, mr);

        component::Transform t;
        t.setPosition(glm::vec3(0.0f));
        registry->assign<component::Transform>(e, t);

        auto& so = registry->assign<component::SceneObject>(e);
        so.name = "Container";

        auto& rb = registry->assign<component::RigidBody>(e);
        rb.mass = 1.0f;
        rb.type = RigidBody_Box;
        rb.width =  1.0f;
        rb.height = 1.0f;
        rb.length = 1.0f;

        registry->assign<entt::tag<"Container"_hs>>(e);

        entt::entity e2 = registry->create();
        registry->assign<component::Renderable>(e2, mr);

        t.setPosition(glm::vec3(3.0f, 3.0f, 3.0f));
        t.setScale(glm::vec3(0.3f));
        registry->assign<component::Transform>(e2, t);

        auto& l = registry->assign<component::PointLight>(e2);
        l.ambient = glm::vec3(0.75f);
        
        auto& so2 = registry->assign<component::SceneObject>(e2);
        so2.name = "Light 1";
    }
    {
        component::Renderable mr;
        mr.mesh = Mesh::cache("res/assets/models/plane.obj");
        mr.material = mat;

        entt::entity e = registry->create();
        registry->assign<component::Renderable>(e, mr);

        component::Transform t;
        t.setPosition(glm::vec3(0.0f, -5.0f, 0.0f));
        registry->assign<component::Transform>(e, t);

        auto& so = registry->assign<component::SceneObject>(e);
        so.name = "Plane";

        auto& rb = registry->assign<component::RigidBody>(e);
        rb.type = RigidBody_Box;
        rb.mass = 0;
        rb.width = 20;
        rb.height = 0.1f;
        rb.length = 20;
    }
    {
        component::Renderable mr;
        mr.mesh = Mesh::cache("res/assets/models/sphere.obj");
        mr.material = mat;

        entt::entity e = registry->create();
        registry->assign<component::Renderable>(e, mr);

        component::Transform t;
        t.setPosition(glm::vec3(-3.0f, -1.0f, -2.0f));
        registry->assign<component::Transform>(e, t);

        auto& so = registry->assign<component::SceneObject>(e);
        so.name = "Sphere 1";

        auto& rb = registry->assign<component::RigidBody>(e);
        rb.type   = RigidBody_Sphere;
        rb.radius = 0.5f;
        rb.mass   = 1.0f;
    }
    {
        component::Renderable mr;
        mr.mesh = Mesh::get("sphere");
        mr.material = mat;

        entt::entity e = registry->create();
        registry->assign<component::Renderable>(e, mr);

        component::Transform t;
        t.setPosition(glm::vec3(-3.0f, -1.0f, -2.0f));
        registry->assign<component::Transform>(e, t);

        auto& so = registry->assign<component::SceneObject>(e);
        so.name = "Sphere 2";

        auto& rb = registry->assign<component::RigidBody>(e);
        rb.type = RigidBody_Sphere;
        rb.radius = 0.5f;
        rb.mass = 1.0f;
    }
}

void SandboxLayer::onUpdate(Timestep dt)
{
    auto view = registry->view<entt::tag<"Container"_hs>>();
    entt::entity container = view[0];

    component::Transform& tr = registry->get<component::Transform>(container);
    component::RigidBody& rb = registry->get<component::RigidBody>(container);

    glm::vec3 desiredVel = {};
    
    if (Input::isKeyPressed(Key_W))
        desiredVel = tr.getForward() * forceSpeed;
                                         
    if (Input::isKeyPressed(Key_S))      
        desiredVel = -tr.getForward() * forceSpeed;

    if (Input::isKeyPressed(Key_A))
        desiredVel = -tr.getRight() * forceSpeed;

    if (Input::isKeyPressed(Key_D))
        desiredVel = tr.getRight() * forceSpeed;

    glm::vec3 velChange = desiredVel - rb.velocity;
    velChange.y = 0.01f;
    rb.impulse = rb.mass * velChange;
    
}

void SandboxLayer::onGuiRender(Timestep dt)
{
    ImGui::Begin("xdhaha");

    ImGui::SliderFloat("Force Speed", &forceSpeed, 0.1f, 10.0f);
    
    ImGui::End();
}

bool SandboxLayer::onEvent(Ref<Event> event)
{
    return false;
}
