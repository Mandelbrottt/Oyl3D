#include <Oyl3D.h>

#include "SandboxLayer.h"
#include "Cannon.h"
#include "Player.h"

using namespace oyl;

void SandboxLayer::onEnter()
{    
    auto mesh = Mesh::cache("res/assets/models/cube.obj");
    
    auto& mat = Material::cache(Shader::get(LIGHTING_SHADER_ALIAS), "container");
    mat->albedoMap   = Texture2D::cache("res/assets/textures/container2.jpg");
    mat->specularMap = Texture2D::cache("res/assets/textures/container2_specular.jpg");

    {
        component::Renderable mr;
        mr.mesh     = mesh;
        mr.material = mat;

		entt::entity cannonBlue = registry->create();
        
		component::Transform t;
		t.setPosition(glm::vec3(0.0f));
		registry->assign<component::Transform>(cannonBlue, t);
        
		registry->assign<Cannon>(cannonBlue);
		registry->get<Cannon>(cannonBlue).team = Team::blue;
		registry->get<Cannon>(cannonBlue).fuse.timeToWait = 10.0f;
		registry->get<Cannon>(cannonBlue).initialPosition = t.getPosition();

		registry->assign<component::Renderable>(cannonBlue, mr);


		auto& so = registry->assign<component::SceneObject>(cannonBlue);
		so.name = "BlueCannon";

        entt::entity player = registry->create();
		registry->assign<Player>(player);
		registry->get<Player>(player).team = Team::blue;
        
        registry->assign<component::Renderable>(player, mr);

        component::Transform t2;
        t2.setPosition(glm::vec3(0.0f));
        registry->assign<component::Transform>(player, t2);

        auto& so2 = registry->assign<component::SceneObject>(player);
        so2.name = "Player";

        auto& rb = registry->assign<component::RigidBody>(player);
        rb.mass = 1.0f;
        rb.type = RigidBody_Box;
        rb.width =  1.0f;
        rb.height = 1.0f;
        rb.length = 1.0f;

        registry->assign<entt::tag<"Player"_hs>>(player);

        auto& shi = cl.pushShape(Collider_Box);
        shi.box.setSize({ 1.0f, 1.0f, 1.0f });

        entt::entity e2 = registry->create();
        registry->assign<component::Renderable>(e2, mr);

        t.setPosition(glm::vec3(3.0f, 3.0f, 3.0f));
        t.setScale(glm::vec3(0.3f));
        registry->assign<component::Transform>(e2, t);

        auto& l = registry->assign<component::PointLight>(e2);
        l.ambient = glm::vec3(0.75f);
        
        auto& so3 = registry->assign<component::SceneObject>(e2);
        so3.name = "Light 1";
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
        rb.setMass(0.0f);

        auto& cl = registry->assign<component::Collider>(e);

        auto& shi = cl.pushShape(Collider_Box);
        shi.box.setSize({ 20.0f, 0.1f, 20.0f });
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
        rb.setMass(1.0f);

        auto& cl = registry->assign<component::Collider>(e);

        auto& shi = cl.pushShape(Collider_Sphere);
        shi.sphere.setRadius(0.5f);
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
        rb.setMass(1.0f);

        auto& cl = registry->assign<component::Collider>(e);

        auto& shi = cl.pushShape(Collider_Sphere);
        shi.sphere.setRadius(0.5f);
    }
}

void SandboxLayer::onUpdate(Timestep dt)
{

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
