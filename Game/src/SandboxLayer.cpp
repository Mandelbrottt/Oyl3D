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

		{
			entt::entity playerBlueEntity = registry->create();
			registry->assign<Player>(playerBlueEntity);
			registry->get<Player>(playerBlueEntity).team = Team::blue;

			registry->assign<component::Renderable>(playerBlueEntity, mr);

			component::Transform t2;
			t2.setPosition(glm::vec3(0.0f));
			registry->assign<component::Transform>(playerBlueEntity, t2);

			auto& so2 = registry->assign<component::SceneObject>(playerBlueEntity);
			so2.name = "Player";

			auto& rb = registry->assign<component::RigidBody>(playerBlueEntity);
			rb.setMass(5.0f);
			rb.setFriction(1.0f);
			rb.setProperties(component::RigidBody::Property::FREEZE_ROTATION_X, true);
			rb.setProperties(component::RigidBody::Property::FREEZE_ROTATION_Y, true);
			rb.setProperties(component::RigidBody::Property::FREEZE_ROTATION_Z, true);

			auto& playerCollider = registry->assign<component::Collider>(playerBlueEntity);
			auto& shapeInfo = playerCollider.pushShape(Collider_Box);
			shapeInfo.box.setSize({ 1.0f, 1.0f, 1.0f });

			registry->assign<entt::tag<"Player"_hs>>(playerBlueEntity);
		}
        
		{
			entt::entity cannonBlueEntity = registry->create();

			component::Transform cannonTransform;
			cannonTransform.setPosition(glm::vec3(0.0f));
			registry->assign<component::Transform>(cannonBlueEntity, cannonTransform);

			auto& cannon = registry->assign<Cannon>(cannonBlueEntity);
			cannon.team = Team::blue;
			cannon.fuse.timeToWait = 10.0f;

			registry->assign<component::Renderable>(cannonBlueEntity, mr);

			auto& so = registry->assign<component::SceneObject>(cannonBlueEntity);
			so.name = "BlueCannon";
/*
			auto& rb = registry->assign<component::RigidBody>(cannonBlueEntity);
			rb.setMass(5.0f);
			rb.setFriction(1.0f);
			rb.setProperties(component::RigidBody::Property::FREEZE_ROTATION_X, true);
			rb.setProperties(component::RigidBody::Property::FREEZE_ROTATION_Y, true);
			rb.setProperties(component::RigidBody::Property::FREEZE_ROTATION_Z, true);*/

			auto& cannonCollider = registry->assign<component::Collider>(cannonBlueEntity);
			auto& shapeInfo = cannonCollider.pushShape(Collider_Box);
			shapeInfo.box.setSize({ 1.0f, 1.0f, 1.0f });
		}

        {
			entt::entity mopBlueEntity = registry->create();

			component::Transform mopTransform;
			mopTransform.setPosition(glm::vec3(3.0f, 0.1f, 0.0f));
			mopTransform.setScale(glm::vec3(2.0f, 0.2f, 0.2f));
			registry->assign<component::Transform>(mopBlueEntity, mopTransform);

			auto& mop = registry->assign<CarryableItem>(mopBlueEntity);
			mop.team = Team::blue;
			mop.type = CarryableItemType::mop;

			registry->assign<component::Renderable>(mopBlueEntity, mr);

			auto& so2 = registry->assign<component::SceneObject>(mopBlueEntity);
			so2.name = "BlueMop";

			/*auto& rb = registry->assign<component::RigidBody>(mopBlueEntity);
			rb.setMass(2.0f);
			rb.setFriction(1.0f);*/

			auto& mopCollider = registry->assign<component::Collider>(mopBlueEntity);
			auto& shapeInfo = mopCollider.pushShape(Collider_Box);
			shapeInfo.box.setSize({ 1.0f, 1.0f, 1.0f });
        }

		{
			entt::entity garbagePileBlueEntity = registry->create();

			component::Transform garbagePileTransform;
			garbagePileTransform.setPosition(glm::vec3(-3.0f, 0.0f, 0.0f));
			garbagePileTransform.setScale(glm::vec3(3.0f, 0.7f, 3.0f));
			registry->assign<component::Transform>(garbagePileBlueEntity, garbagePileTransform);

			auto& garbagePile = registry->assign<GarbagePile>(garbagePileBlueEntity);
			garbagePile.team = Team::blue;

			registry->assign<component::Renderable>(garbagePileBlueEntity, mr);

			auto& so2 = registry->assign<component::SceneObject>(garbagePileBlueEntity);
			so2.name = "BlueGarbagePile";

			auto& garbagePileCollider = registry->assign<component::Collider>(garbagePileBlueEntity);
			auto& shapeInfo = garbagePileCollider.pushShape(Collider_Box);
			shapeInfo.box.setSize({ 1.0f, 1.0f, 1.0f });
		}

		{
			component::Collider boxCollider;
			auto& shi = boxCollider.pushShape(Collider_Box);
			shi.box.setSize({ 1.0f, 1.0f, 1.0f });

			entt::entity e2 = registry->create();
			registry->assign<component::Renderable>(e2, mr);

			component::Transform boxTransform;
			boxTransform.setPosition(glm::vec3(3.0f, 3.0f, 3.0f));
			boxTransform.setScale(glm::vec3(0.3f));
			registry->assign<component::Transform>(e2, boxTransform);

			auto& l = registry->assign<component::PointLight>(e2);
			l.ambient = glm::vec3(0.75f);

			auto& so3 = registry->assign<component::SceneObject>(e2);
			so3.name = "Light 1";
		}
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
		rb.setFriction(1.0f);

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
