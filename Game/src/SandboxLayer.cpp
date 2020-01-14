#include <Oyl3D.h>

#include "SandboxLayer.h"

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

		/////////////////////////////////////////////////////
		///////////////////// BLUE TEAM /////////////////////
		{
			//PLAYER
			entt::entity playerBlueEntity = registry->create();
			auto& player = registry->assign<Player>(playerBlueEntity);
			player.playerNum = 0;
			player.team      = Team::blue;

			registry->assign<component::Renderable>(playerBlueEntity, mr);

			component::Transform t2;
			t2.setPosition(glm::vec3(0.0f));
			registry->assign<component::Transform>(playerBlueEntity, t2);

			auto& so2 = registry->assign<component::EntityInfo>(playerBlueEntity);
			so2.name = "Player";

			auto& rb = registry->assign<component::RigidBody>(playerBlueEntity);
			rb.setMass(5.0f);
			rb.setFriction(1.0f);
			rb.setProperties(component::RigidBody::Property::FREEZE_ROTATION_X, true);
			rb.setProperties(component::RigidBody::Property::FREEZE_ROTATION_Y, true);
			rb.setProperties(component::RigidBody::Property::FREEZE_ROTATION_Z, true);

			auto& playerCollider = registry->assign<component::Collidable>(playerBlueEntity);
			auto& shapeInfo = playerCollider.pushShape(ColliderType::Box);
			shapeInfo.box.setSize({ 1.0f, 1.0f, 1.0f });

			registry->assign<entt::tag<"Player"_hs>>(playerBlueEntity);
		}

		//PLAYER CAMERA
		{
			auto playerCameraEntity = registry->create();
			registry->assign<component::Transform>(playerCameraEntity);
			auto& camera = registry->assign<component::PlayerCamera>(playerCameraEntity);
			camera.player = 0;
			camera.projection = glm::perspective(glm::radians(60.0f), 16.0f / 9.0f, 0.01f, 1000.0f);
			auto& so = registry->assign<component::EntityInfo>(playerCameraEntity);
			so.name = "Player Camera";
		}
        
		{
			mr.mesh = Mesh::cache("res/assets/models/cannon.obj");

			//TEMP MATERIAL INIT LOCATION UNTIL SYSTEM OVERHAUL
			mr.material = Material::cache(Shader::get(LIGHTING_SHADER_ALIAS), "planks");
			mr.material->albedoMap = Texture2D::cache("res/assets/textures/woodPlanks.png");
			mr.material->specularMap = Texture2D::cache("res/assets/textures/tempSpec.jpg");

			mr.material = Material::cache(Shader::get(LIGHTING_SHADER_ALIAS), "cannonball");
			mr.material->albedoMap = Texture2D::cache("res/assets/textures/cannonballTemp.png");

			mr.material = Material::cache(Shader::get(LIGHTING_SHADER_ALIAS), "goop");
			mr.material->albedoMap = Texture2D::cache("res/assets/textures/goopTemp.png");

			mr.material = Material::cache(Shader::get(LIGHTING_SHADER_ALIAS), "pirax");
			mr.material->albedoMap = Texture2D::cache("res/assets/textures/piraxTemp.png");

			mr.material = Material::cache(Shader::get(LIGHTING_SHADER_ALIAS), "garbage");
			mr.material->albedoMap = Texture2D::cache("res/assets/textures/garbageTemp.png");

			mr.material = Material::cache(Shader::get(LIGHTING_SHADER_ALIAS), "crate");
			mr.material->albedoMap = Texture2D::cache("res/assets/textures/binTemp.png");

			mr.material = Material::cache(Shader::get(LIGHTING_SHADER_ALIAS), "mop");
			mr.material->albedoMap = Texture2D::cache("res/assets/textures/mopTemp.png");

			mr.material = Material::cache(Shader::get(LIGHTING_SHADER_ALIAS), "cannon");
			mr.material->albedoMap = Texture2D::cache("res/assets/textures/cannonTemp.png");

			mr.material = Material::cache(Shader::get(LIGHTING_SHADER_ALIAS), "lamp");
			mr.material->albedoMap = Texture2D::cache("res/assets/textures/lampTemp.png");

			//CANNON
			entt::entity cannonEntity = registry->create();

			component::Transform cannonTransform;
			cannonTransform.setPosition(glm::vec3(0.0f));
			registry->assign<component::Transform>(cannonEntity, cannonTransform);

			auto& cannon = registry->assign<Cannon>(cannonEntity);
			cannon.team = Team::blue;

			registry->assign<component::Renderable>(cannonEntity, mr);

			auto& so = registry->assign<component::EntityInfo>(cannonEntity);
			so.name = "BlueCannon";

			auto& rb = registry->assign<component::RigidBody>(cannonEntity);
			rb.setMass(0.0f);

			auto& cannonCollider = registry->assign<component::Collidable>(cannonEntity);
			auto& shapeInfo = cannonCollider.pushShape(ColliderType::Box);
			shapeInfo.box.setSize({ 2.35f, 4.15f, 2.6f });
		}

		{
			//GARBAGE PILES
			mr.mesh = Mesh::cache("res/assets/models/garbage.obj");
			for (int i = 0; i < 3; i++)
			{
				entt::entity garbagePileEntity = registry->create();

				component::Transform garbagePileTransform;
				garbagePileTransform.setPosition(glm::vec3(-3.0f, 0.2f, 0.0f));
				garbagePileTransform.setScale(glm::vec3(3.0f, 0.4f, 3.0f));
				registry->assign<component::Transform>(garbagePileEntity, garbagePileTransform);

				auto& garbagePile = registry->assign<GarbagePile>(garbagePileEntity);
				garbagePile.team = Team::blue;

				auto& rb = registry->assign<component::RigidBody>(garbagePileEntity);
				rb.setMass(0.0f);

				registry->assign<component::Renderable>(garbagePileEntity, mr);

				auto& so2 = registry->assign<component::EntityInfo>(garbagePileEntity);
				so2.name = "BlueGarbagePile" + std::to_string(i);

				auto& garbagePileCollider = registry->assign<component::Collidable>(garbagePileEntity);
				auto& shapeInfo = garbagePileCollider.pushShape(ColliderType::Box);
				shapeInfo.box.setSize({ 3.4f, 1.0f, 3.2f });
			}
		}

		{
			//CANNONBALLS
			mr.mesh = Mesh::cache("res/assets/models/sphere.obj");
			for (int i = 0; i < 6; i++)
			{
				entt::entity cannonballEntity = registry->create();

				component::Transform cannonballTransform;
				cannonballTransform.setPosition(glm::vec3(-1000.0f, -1000.0f, -1000.0f));
				cannonballTransform.setScale(glm::vec3(0.5f, 0.5f, 0.5f));
				registry->assign<component::Transform>(cannonballEntity, cannonballTransform);

				auto& carryableItem    = registry->assign<CarryableItem>(cannonballEntity);
				carryableItem.team     = Team::blue;
				carryableItem.type     = CarryableItemType::cannonball;
				carryableItem.isActive = false;

				auto& rb = registry->assign<component::RigidBody>(cannonballEntity);

				auto& cannonball = registry->assign<Cannonball>(cannonballEntity);

				mr.mesh = Mesh::get("sphere");
				registry->assign<component::Renderable>(cannonballEntity, mr);

				auto& so2 = registry->assign<component::EntityInfo>(cannonballEntity);
				so2.name = "BlueCannonball" + std::to_string(i);

				auto& cannonballCollider = registry->assign<component::Collidable>(cannonballEntity);
				auto& shapeInfo = cannonballCollider.pushShape(ColliderType::Sphere);
				shapeInfo.sphere.setRadius(0.5f);
			}
		}

        {
			mr.mesh = Mesh::cache("res/assets/models/mop.obj");
			//MOP
			entt::entity mopEntity = registry->create();

			component::Transform mopTransform;
			mopTransform.setPosition(glm::vec3(3.0f, 0.1f, 0.0f));
			mopTransform.setScale(glm::vec3(2.0f, 0.2f, 0.2f));
			registry->assign<component::Transform>(mopEntity, mopTransform);

			auto& carryableItem = registry->assign<CarryableItem>(mopEntity);
			carryableItem.team = Team::blue;
			carryableItem.type = CarryableItemType::mop;

			auto& rb = registry->assign<component::RigidBody>(mopEntity);

			mr.mesh = Mesh::get("cube");
			registry->assign<component::Renderable>(mopEntity, mr);

			auto& so2 = registry->assign<component::EntityInfo>(mopEntity);
			so2.name = "BlueMop";

			/*auto& rb = registry->assign<component::RigidBody>(mopBlueEntity);
			rb.setMass(2.0f);
			rb.setFriction(1.0f);*/

			auto& mopCollider = registry->assign<component::Collidable>(mopEntity);
			auto& shapeInfo = mopCollider.pushShape(ColliderType::Box);
			shapeInfo.box.setSize({ 2.0f, 0.2f, 0.5f });
        }

		{
			mr.mesh = Mesh::cache("res/assets/models/Pirax.obj");
			//CLEANING SOLUTION
			for (int i = 0; i < 3; i++)
			{
				entt::entity cleaningSolutionEntity = registry->create();

				component::Transform cleaningSolutionTransform;
				cleaningSolutionTransform.setPosition(glm::vec3(3.0f, 0.31f, 3.0f));
				cleaningSolutionTransform.setScale(glm::vec3(0.3f, 0.3f, 0.3f));
				registry->assign<component::Transform>(cleaningSolutionEntity, cleaningSolutionTransform);

				auto& carryableItem = registry->assign<CarryableItem>(cleaningSolutionEntity);
				carryableItem.team = Team::blue;
				carryableItem.type = CarryableItemType::cleaningSolution;

				auto& rb = registry->assign<component::RigidBody>(cleaningSolutionEntity);
				rb.setProperties(component::RigidBody::Property::IS_KINEMATIC, true);

				mr.mesh = Mesh::get("cube");
				registry->assign<component::Renderable>(cleaningSolutionEntity, mr);

				auto& so2 = registry->assign<component::EntityInfo>(cleaningSolutionEntity);
				so2.name = "BlueCleaningSolution" + std::to_string(i);

				auto& cleaningSolutionCollider = registry->assign<component::Collidable>(cleaningSolutionEntity);
				auto& shapeInfo = cleaningSolutionCollider.pushShape(ColliderType::Box);
				shapeInfo.box.setSize({ 0.8f, 1.0f, 0.85f });
			}
		}

		{
			mr.mesh = Mesh::cache("res/assets/models/Gloop.obj");
			//GLOOP
			for (int i = 0; i < 3; i++)
			{
				entt::entity gloopEntity = registry->create();

				component::Transform gloopTransform;
				gloopTransform.setPosition(glm::vec3(3.0f, 0.27f, 3.0f));
				gloopTransform.setScale(glm::vec3(0.23f, 0.23f, 0.23f));
				registry->assign<component::Transform>(gloopEntity, gloopTransform);

				auto& carryableItem = registry->assign<CarryableItem>(gloopEntity);
				carryableItem.team = Team::blue;
				carryableItem.type = CarryableItemType::gloop;

				auto& rb = registry->assign<component::RigidBody>(gloopEntity);

				registry->assign<Gloop>(gloopEntity);

				registry->assign<component::Renderable>(gloopEntity, mr);

				auto& so2 = registry->assign<component::EntityInfo>(gloopEntity);
				so2.name = "BlueGloopA" + std::to_string(i);

				auto& gloopCollider = registry->assign<component::Collidable>(gloopEntity);
				auto& shapeInfo = gloopCollider.pushShape(ColliderType::Box);
				shapeInfo.box.setSize({ 1.55f, 2.65f, 1.0f });
			}
		}

		{
			mr.mesh = Mesh::cache("res/assets/models/crateTemp.obj");
			//CANNONBALL CRATE
			entt::entity cannonballCrateEntity = registry->create();

			component::Transform cannonballCrateTransform;
			cannonballCrateTransform.setPosition(glm::vec3(-1.0f, 0.5f, -3.0f));
			cannonballCrateTransform.setScale(glm::vec3(2.0f, 1.0f, 1.0f));
			registry->assign<component::Transform>(cannonballCrateEntity, cannonballCrateTransform);

			auto& cannonballCrate = registry->assign<CannonballCrate>(cannonballCrateEntity);
			cannonballCrate.team = Team::blue;

			mr.material = mat;
			registry->assign<component::Renderable>(cannonballCrateEntity, mr);

			auto& so2 = registry->assign<component::EntityInfo>(cannonballCrateEntity);
			so2.name = "BlueCannonballCrate";

			auto& rb = registry->assign<component::RigidBody>(cannonballCrateEntity);
			rb.setMass(0.0f);

			auto& cannonballCrateCollider = registry->assign<component::Collidable>(cannonballCrateEntity);
			auto& shapeInfo = cannonballCrateCollider.pushShape(ColliderType::Box);
			shapeInfo.box.setSize({ 2.0f, 2.7f, 1.2f });
		}
		/////////////////////////////////////////////////////
		/////////////////////////////////////////////////////

		/////////////////////////////////////////////////////
		///////////////////// RED TEAM //////////////////////
		{
			//CANNON
			entt::entity cannonEntity = registry->create();

			component::Transform cannonTransform;
			cannonTransform.setPosition(glm::vec3(0.0f));
			cannonTransform.setRotationEulerY(180.0f);
			registry->assign<component::Transform>(cannonEntity, cannonTransform);

			auto& cannon = registry->assign<Cannon>(cannonEntity);
			cannon.team = Team::red;

			registry->assign<component::Renderable>(cannonEntity, mr);

			auto& so = registry->assign<component::EntityInfo>(cannonEntity);
			so.name = "RedCannon";

			auto& rb = registry->assign<component::RigidBody>(cannonEntity);
			rb.setMass(0.0f);

			auto& cannonCollider = registry->assign<component::Collidable>(cannonEntity);
			auto& shapeInfo = cannonCollider.pushShape(ColliderType::Box);
			shapeInfo.box.setSize({ 2.35f, 4.15f, 2.6f });
		}

		{
			//GARBAGE PILES
			for (int i = 0; i < 3; i++)
			{
				entt::entity garbagePileEntity = registry->create();

				component::Transform garbagePileTransform;
				garbagePileTransform.setPosition(glm::vec3(-3.0f, 0.2f, 0.0f));
				garbagePileTransform.setScale(glm::vec3(3.0f, 0.4f, 3.0f));
				registry->assign<component::Transform>(garbagePileEntity, garbagePileTransform);

				auto& garbagePile = registry->assign<GarbagePile>(garbagePileEntity);
				garbagePile.team = Team::red;

				auto& rb = registry->assign<component::RigidBody>(garbagePileEntity);
				rb.setMass(0.0f);

				registry->assign<component::Renderable>(garbagePileEntity, mr);

				auto& so2 = registry->assign<component::EntityInfo>(garbagePileEntity);
				so2.name = "RedGarbagePile" + std::to_string(i);

				auto& garbagePileCollider = registry->assign<component::Collidable>(garbagePileEntity);
				auto& shapeInfo = garbagePileCollider.pushShape(ColliderType::Box);
				shapeInfo.box.setSize({ 3.4f, 1.0f, 3.2f });
			}
		}

		{
			//CANNONBALLS
			mr.mesh = Mesh::cache("res/assets/models/sphere.obj");
			for (int i = 0; i < 6; i++)
			{
				entt::entity cannonballEntity = registry->create();

				component::Transform cannonballTransform;
				cannonballTransform.setPosition(glm::vec3(-1000.0f, -1000.0f, -1000.0f));
				cannonballTransform.setScale(glm::vec3(0.5f, 0.5f, 0.5f));
				registry->assign<component::Transform>(cannonballEntity, cannonballTransform);

				auto& carryableItem    = registry->assign<CarryableItem>(cannonballEntity);
				carryableItem.team     = Team::red;
				carryableItem.type     = CarryableItemType::cannonball;
				carryableItem.isActive = false;

				auto& rb = registry->assign<component::RigidBody>(cannonballEntity);

				auto& cannonball = registry->assign<Cannonball>(cannonballEntity);

				mr.mesh = Mesh::get("sphere");
				registry->assign<component::Renderable>(cannonballEntity, mr);

				auto& so2 = registry->assign<component::EntityInfo>(cannonballEntity);
				so2.name = "RedCannonball" + std::to_string(i);

				auto& cannonballCollider = registry->assign<component::Collidable>(cannonballEntity);
				auto& shapeInfo = cannonballCollider.pushShape(ColliderType::Sphere);
				shapeInfo.sphere.setRadius(0.5f);
			}
		}

		{
			//MOP
			entt::entity mopEntity = registry->create();

			component::Transform mopTransform;
			mopTransform.setPosition(glm::vec3(3.0f, 0.1f, 0.0f));
			mopTransform.setScale(glm::vec3(2.0f, 0.2f, 0.2f));
			registry->assign<component::Transform>(mopEntity, mopTransform);

			auto& carryableItem = registry->assign<CarryableItem>(mopEntity);
			carryableItem.team = Team::red;
			carryableItem.type = CarryableItemType::mop;

			auto& rb = registry->assign<component::RigidBody>(mopEntity);

			mr.mesh = Mesh::get("cube");
			registry->assign<component::Renderable>(mopEntity, mr);

			auto& so2 = registry->assign<component::EntityInfo>(mopEntity);
			so2.name = "RedMop";

			/*auto& rb = registry->assign<component::RigidBody>(mopBlueEntity);
			rb.setMass(2.0f);
			rb.setFriction(1.0f);*/

			auto& mopCollider = registry->assign<component::Collidable>(mopEntity);
			auto& shapeInfo = mopCollider.pushShape(ColliderType::Box);
			shapeInfo.box.setSize({ 2.0f, 0.2f, 0.5f });
		}

		{
			//CLEANING SOLUTION
			for (int i = 0; i < 3; i++)
			{
				entt::entity cleaningSolutionEntity = registry->create();

				component::Transform cleaningSolutionTransform;
				cleaningSolutionTransform.setPosition(glm::vec3(3.0f, 0.22f, 3.0f));
				cleaningSolutionTransform.setScale(glm::vec3(0.2f, 0.44f, 0.2f));
				registry->assign<component::Transform>(cleaningSolutionEntity, cleaningSolutionTransform);

				auto& carryableItem = registry->assign<CarryableItem>(cleaningSolutionEntity);
				carryableItem.team = Team::red;
				carryableItem.type = CarryableItemType::cleaningSolution;

				auto& rb = registry->assign<component::RigidBody>(cleaningSolutionEntity);

				mr.mesh = Mesh::get("cube");
				registry->assign<component::Renderable>(cleaningSolutionEntity, mr);

				auto& so2 = registry->assign<component::EntityInfo>(cleaningSolutionEntity);
				so2.name = "RedCleaningSolution" + std::to_string(i);

				auto& cleaningSolutionCollider = registry->assign<component::Collidable>(cleaningSolutionEntity);
				auto& shapeInfo = cleaningSolutionCollider.pushShape(ColliderType::Box);
				shapeInfo.box.setSize({ 0.8f, 1.0f, 0.85f });
			}
		}

		{
			//GLOOP
			for (int i = 0; i < 3; i++)
			{
				entt::entity gloopEntity = registry->create();

				component::Transform gloopTransform;
				gloopTransform.setPosition(glm::vec3(3.0f, 0.27f, 3.0f));
				gloopTransform.setScale(glm::vec3(0.2f, 0.2f, 0.2f));
				registry->assign<component::Transform>(gloopEntity, gloopTransform);

				auto& carryableItem = registry->assign<CarryableItem>(gloopEntity);
				carryableItem.team = Team::red;
				carryableItem.type = CarryableItemType::gloop;

				auto& rb = registry->assign<component::RigidBody>(gloopEntity);

				registry->assign<Gloop>(gloopEntity);

				mr.mesh = Mesh::get("Gloop");
				registry->assign<component::Renderable>(gloopEntity, mr);

				auto& so2 = registry->assign<component::EntityInfo>(gloopEntity);
				so2.name = "RedGloopA" + std::to_string(i);

				auto& gloopCollider = registry->assign<component::Collidable>(gloopEntity);
				auto& shapeInfo = gloopCollider.pushShape(ColliderType::Box);
				shapeInfo.box.setSize({ 1.55f, 2.65f, 1.0f });
			}
		}

		{
			//CANNONBALL CRATE
			entt::entity cannonballCrateEntity = registry->create();

			component::Transform cannonballCrateTransform;
			cannonballCrateTransform.setPosition(glm::vec3(-1.0f, 0.5f, -3.0f));
			cannonballCrateTransform.setScale(glm::vec3(2.0f, 1.0f, 1.0f));
			registry->assign<component::Transform>(cannonballCrateEntity, cannonballCrateTransform);

			auto& cannonballCrate = registry->assign<CannonballCrate>(cannonballCrateEntity);
			cannonballCrate.team = Team::red;

			mr.mesh = Mesh::get("cube");
			mr.material = mat;
			registry->assign<component::Renderable>(cannonballCrateEntity, mr);

			auto& so2 = registry->assign<component::EntityInfo>(cannonballCrateEntity);
			so2.name = "RedCannonballCrate";

			auto& rb = registry->assign<component::RigidBody>(cannonballCrateEntity);
			rb.setMass(0.0f);

			auto& cannonballCrateCollider = registry->assign<component::Collidable>(cannonballCrateEntity);
			auto& shapeInfo = cannonballCrateCollider.pushShape(ColliderType::Box);
			shapeInfo.box.setSize({ 2.0f, 2.7f, 1.2f });
		}
		/////////////////////////////////////////////////////
		/////////////////////////////////////////////////////

		{
			mr.mesh = Mesh::cache("res/assets/models/lamp.obj");
			component::Collidable boxCollider;
			auto& shi = boxCollider.pushShape(ColliderType::Box);
			shi.box.setSize({ 1.0f, 1.0f, 1.0f });

			entt::entity e2 = registry->create();
			registry->assign<component::Renderable>(e2, mr);

			component::Transform boxTransform;
			boxTransform.setPosition(glm::vec3(3.0f, 3.0f, 3.0f));
			boxTransform.setScale(glm::vec3(0.3f));
			registry->assign<component::Transform>(e2, boxTransform);

			auto& l = registry->assign<component::PointLight>(e2);
			l.ambient = glm::vec3(0.75f);

			auto& so3 = registry->assign<component::EntityInfo>(e2);
			so3.name = "Light 1";
		}

		{
			//CHARACTER MESH
			entt::entity characterEntity = registry->create();

			component::Transform mopTransform;
			mopTransform.setPosition(glm::vec3(10.0f, 6.0f, 0.0f));
			mopTransform.setScale(glm::vec3(1.0f, 1.0f, 1.0f));
			registry->assign<component::Transform>(characterEntity, mopTransform);

			mr.mesh = Mesh::cache("res/assets/models/character.obj");
			registry->assign<component::Renderable>(characterEntity, mr);

			auto& so2 = registry->assign<component::EntityInfo>(characterEntity);
			so2.name = "Character";

			/*auto& rb = registry->assign<component::RigidBody>(mopBlueEntity);
			rb.setMass(2.0f);
			rb.setFriction(1.0f);*/

			auto& mopCollider = registry->assign<component::Collidable>(characterEntity);
			auto& shapeInfo = mopCollider.pushShape(ColliderType::Box);
			shapeInfo.box.setSize({ 1.0f, 1.0f, 1.0f });
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
		t.setScale(glm::vec3(2.5f, 1.0f, 1.7f));
        registry->assign<component::Transform>(e, t);

        auto& so = registry->assign<component::EntityInfo>(e);
        so.name = "Plane";

        auto& rb = registry->assign<component::RigidBody>(e);
        rb.setMass(0.0f);
		rb.setFriction(1.0f);

        auto& cl = registry->assign<component::Collidable>(e);

        auto& shi = cl.pushShape(ColliderType::Box);
        shi.box.setSize({ 20.0f, 0.1f, 20.0f });
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

        auto& so = registry->assign<component::EntityInfo>(e);
        so.name = "Sphere 1";
        
        auto& rb = registry->assign<component::RigidBody>(e);
        rb.setMass(1.0f);

        auto& cl = registry->assign<component::Collidable>(e);

        auto& shi = cl.pushShape(ColliderType::Sphere);
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

        auto& so = registry->assign<component::EntityInfo>(e);
        so.name = "Sphere 2";

        auto& rb = registry->assign<component::RigidBody>(e);
        rb.setMass(1.0f);

        auto& cl = registry->assign<component::Collidable>(e);

        auto& shi = cl.pushShape(ColliderType::Sphere);
        shi.sphere.setRadius(0.5f);
    }
}

void SandboxLayer::onUpdate()
{

}

void SandboxLayer::onGuiRender()
{
    /*ImGui::Begin("xdhaha");

    ImGui::SliderFloat("Force Speed", &forceSpeed, 0.1f, 10.0f);
    
    ImGui::End();*/
}
