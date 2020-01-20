#include <Oyl3D.h>

#include "SandboxLayer.h"

using namespace oyl;

void SandboxLayer::onEnter()
{
    auto mesh = Mesh::cache("res/assets/models/cube.obj");
    
    auto& mat = Material::cache(Material::create(), "container");
	mat->shader = Shader::get(LIGHTING_SHADER_ALIAS);
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
			mr.material = Material::cache(Material::create(), "planks");
			mr.material->shader = Shader::get(LIGHTING_SHADER_ALIAS);
			mr.material->albedoMap = Texture2D::cache("res/assets/textures/woodPlanks.png");
			mr.material->specularMap = Texture2D::cache("res/assets/textures/tempSpec.jpg");

			mr.material = Material::cache(Material::create(), "cannonball");
			mr.material->shader = Shader::get(LIGHTING_SHADER_ALIAS);
			mr.material->albedoMap = Texture2D::cache("res/assets/textures/cannonballTemp.png");
			mr.material->specularMap = Material::get("planks")->specularMap;

			mr.material = Material::cache(Material::create(), "goop");
			mr.material->shader = Shader::get(LIGHTING_SHADER_ALIAS);
			mr.material->albedoMap = Texture2D::cache("res/assets/textures/goopTemp.png");
			mr.material->specularMap = Material::get("planks")->specularMap;

			mr.material = Material::cache(Material::create(), "pirax");
			mr.material->shader = Shader::get(LIGHTING_SHADER_ALIAS);
			mr.material->albedoMap = Texture2D::cache("res/assets/textures/piraxTemp.png");
			mr.material->specularMap = Material::get("planks")->specularMap;

			mr.material = Material::cache(Material::create(), "garbage");
			mr.material->shader = Shader::get(LIGHTING_SHADER_ALIAS);
			mr.material->albedoMap = Texture2D::cache("res/assets/textures/garbageTemp.png");
			mr.material->specularMap = Material::get("planks")->specularMap;

			mr.material = Material::cache(Material::create(), "crate");
			mr.material->shader = Shader::get(LIGHTING_SHADER_ALIAS);
			mr.material->albedoMap = Texture2D::cache("res/assets/textures/binTemp.png");
			mr.material->specularMap = Material::get("planks")->specularMap;

			mr.material = Material::cache(Material::create(), "mop");
			mr.material->shader = Shader::get(LIGHTING_SHADER_ALIAS);
			mr.material->albedoMap = Texture2D::cache("res/assets/textures/mopTemp.png");
			mr.material->specularMap = Material::get("planks")->specularMap;

			mr.material = Material::cache(Material::create(), "cannon");
			mr.material->shader = Shader::get(LIGHTING_SHADER_ALIAS);
			mr.material->albedoMap = Texture2D::cache("res/assets/textures/cannonTemp.png");
			mr.material->specularMap = Material::get("planks")->specularMap;

			mr.material = Material::cache((Material::create()), "lamp");
			mr.material->shader = Shader::get(LIGHTING_SHADER_ALIAS);
			mr.material->albedoMap = Texture2D::cache("res/assets/textures/lampTemp.png");
			mr.material->specularMap = Material::get("planks")->specularMap;

			mr.material = Material::cache((Material::create()), "water");
			mr.material->shader = Shader::get(LIGHTING_SHADER_ALIAS);
			mr.material->albedoMap = Texture2D::cache("res/assets/textures/water.png");
			mr.material->specularMap = Material::get("container")->specularMap;

			mr.material = Material::cache((Material::create()), "siding");
			mr.material->shader = Shader::get(LIGHTING_SHADER_ALIAS);
			mr.material->albedoMap = Texture2D::cache("res/assets/textures/woodPlankSiding.png");
			mr.material->specularMap = Material::get("planks")->specularMap;



			//CANNON
			entt::entity cannonEntity = registry->create();
			mr.material = Material::get("cannon");

			component::Transform cannonTransform;
			cannonTransform.setPosition(glm::vec3(0.0f));
			registry->assign<component::Transform>(cannonEntity, cannonTransform);

			auto& cannon = registry->assign<Cannon>(cannonEntity);
			cannon.team  = Team::blue;
			cannon.firingDirection = glm::vec3(1.0f, 1.0f, 1.0f);

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
			//NOTE: MAKE SURE THESE ARE ARRANGED SO THE FIRST ONE (BlueGarbagePile0) IS ON THE LEFTMOST SIDE RELATIVE TO THE POSITIVE Z AXIS
			//      AND THE LAST ONE IS ON THE RIGHT SO THAT THE OPPOSING CANNON FIRES AT THE CORRECT GARBAGE PILE
			mr.mesh = Mesh::cache("res/assets/models/garbage.obj");
			mr.material = Material::get("garbage");
			for (int i = 0; i < 3; i++)
			{
				entt::entity garbagePileEntity = registry->create();

				component::Transform garbagePileTransform;
				garbagePileTransform.setPosition(glm::vec3(-3.0f, 0.2f, 0.0f));
				garbagePileTransform.setScale(glm::vec3(3.0f, 0.4f, 3.0f));
				registry->assign<component::Transform>(garbagePileEntity, garbagePileTransform);

				auto& garbagePile = registry->assign<GarbagePile>(garbagePileEntity);
				garbagePile.team = Team::blue;
				garbagePile.relativePositionOnShip = i - 1;

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
			mr.material = Material::get("cannonball");
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
			mr.material = Material::get("mop");
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
			mr.material = Material::get("pirax");
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

				auto& respawnable = registry->assign<Respawnable>(cleaningSolutionEntity);
				respawnable.spawnPosition = glm::vec3(2.8f, 0.31f, 0.65f);
				respawnable.spawnRotation = glm::vec3(0.0f);

				auto& rb = registry->assign<component::RigidBody>(cleaningSolutionEntity);

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
			mr.material = Material::get("goop");
			//GLOOP
			for (int i = 0; i < 3; i++)
			{
				entt::entity gloopEntity = registry->create();

				component::Transform gloopTransform;
				gloopTransform.setPosition(glm::vec3(3.0f, 0.2f, 3.0f));
				gloopTransform.setScale(glm::vec3(0.1f, 0.1f, 0.1f));
				registry->assign<component::Transform>(gloopEntity, gloopTransform);

				auto& carryableItem = registry->assign<CarryableItem>(gloopEntity);
				carryableItem.team = Team::blue;
				carryableItem.type = CarryableItemType::gloop;

				auto& respawnable = registry->assign<Respawnable>(gloopEntity);
				respawnable.spawnPosition = glm::vec3(1.4f, 0.2f, 1.6f);
				respawnable.spawnRotation = glm::vec3(0.0f);

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
			mr.material = Material::get("planks");
			//CANNONBALL CRATE
			entt::entity cannonballCrateEntity = registry->create();

			component::Transform cannonballCrateTransform;
			cannonballCrateTransform.setPosition(glm::vec3(-1.0f, 0.5f, -3.0f));
			cannonballCrateTransform.setRotation(glm::vec3(0.0f, -90.0f, 0.0f));
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

		{
			//CLEANING SOLUTION SPAWNER
			//NOTE: MAKE SURE THIS IS INITIALIZED AFTER CLEANING SOLUTION ENTITIES
			entt::entity cleaningSolutionSpawnerEntity = registry->create();

			auto& spawner = registry->assign<RespawnManager>(cleaningSolutionSpawnerEntity);
			spawner.respawnTimerDuration = 10.0f;
			spawner.team = Team::blue;
			spawner.type = CarryableItemType::cleaningSolution;

			auto respawnableItemsView = registry->view<Respawnable, CarryableItem, component::Transform>();
			for (auto& respawnableEntity : respawnableItemsView)
			{
				auto& carryableItem = registry->get<CarryableItem>(respawnableEntity);

				if (carryableItem.type == spawner.type && carryableItem.team == spawner.team)
				{
					spawner.entityPrefab = registry->create();
					registry->stomp(spawner.entityPrefab, respawnableEntity, *registry);

					auto& newTransform  = registry->get<component::Transform>(spawner.entityPrefab);
					auto& newEntityInfo = registry->get<component::EntityInfo>(spawner.entityPrefab);
					auto& newCarryable  = registry->get<CarryableItem>(spawner.entityPrefab);

					newCarryable.hasBeenCarried = true; //we have to do this or else the prefab entity will stop this spawner from ever spawning an item
					newTransform.setPosition(glm::vec3(-99999.0f));
					newEntityInfo.name = "BlueCleaningSolutionPrefab";
					break;
				}
			}

			auto& spawnerTransform = registry->assign<component::Transform>(cleaningSolutionSpawnerEntity);
			spawnerTransform.setPosition(glm::vec3(0.0f));

			auto& spawnerInfo = registry->assign<component::EntityInfo>(cleaningSolutionSpawnerEntity);
			spawnerInfo.name = "BlueCleaningSolutionSpawner";
		}

		{
			//GLOOP SPAWNER
			//NOTE: MAKE SURE THIS IS INITIALIZED AFTER GLOOP ENTITIES
			entt::entity gloopSpawnerEntity = registry->create();

			auto& spawner = registry->assign<RespawnManager>(gloopSpawnerEntity);
			spawner.respawnTimerDuration = 20.0f;
			spawner.team = Team::blue;
			spawner.type = CarryableItemType::gloop;

			auto respawnableItemsView = registry->view<Respawnable, CarryableItem, component::Transform>();
			for (auto& respawnableEntity : respawnableItemsView)
			{
				auto& carryableItem = registry->get<CarryableItem>(respawnableEntity);

				if (carryableItem.type == spawner.type && carryableItem.team == spawner.team)
				{
					spawner.entityPrefab = registry->create();
					registry->stomp(spawner.entityPrefab, respawnableEntity, *registry);

					auto& newTransform  = registry->get<component::Transform>(spawner.entityPrefab);
					auto& newEntityInfo = registry->get<component::EntityInfo>(spawner.entityPrefab);
					auto& newCarryable  = registry->get<CarryableItem>(spawner.entityPrefab);

					newCarryable.hasBeenCarried = true; //we have to do this or else the prefab entity will stop this spawner from ever spawning an item
					newTransform.setPosition(glm::vec3(-99999.0f));
					newEntityInfo.name = "BlueGloopPrefab";
					break;
				}
			}

			auto& spawnerTransform = registry->assign<component::Transform>(gloopSpawnerEntity);
			spawnerTransform.setPosition(glm::vec3(0.0f));

			auto& spawnerInfo = registry->assign<component::EntityInfo>(gloopSpawnerEntity);
			spawnerInfo.name = "BlueGloopSpawner";
		}
		/////////////////////////////////////////////////////
		/////////////////////////////////////////////////////

		/////////////////////////////////////////////////////
		///////////////////// RED TEAM //////////////////////
		{
			//CANNON
			mr.material = Material::get("cannon");
			entt::entity cannonEntity = registry->create();

			component::Transform cannonTransform;
			cannonTransform.setPosition(glm::vec3(0.0f));
			registry->assign<component::Transform>(cannonEntity, cannonTransform);

			auto& cannon = registry->assign<Cannon>(cannonEntity);
			cannon.team  = Team::red;
			cannon.firingDirection = glm::vec3(1.0f, 1.0f, -1.0f);

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
			//NOTE: MAKE SURE THESE ARE ARRANGED SO THE FIRST ONE (RedGarbagePile0) IS ON THE LEFTMOST SIDE RELATIVE TO THE POSITIVE Z AXIS
			//      AND THE LAST ONE IS ON THE RIGHT SO THAT THE OPPOSING CANNON FIRES AT THE CORRECT GARBAGE PILE
			mr.material = Material::get("garbage");

			for (int i = 0; i < 3; i++)
			{
				entt::entity garbagePileEntity = registry->create();

				component::Transform garbagePileTransform;
				garbagePileTransform.setPosition(glm::vec3(-3.0f, 0.2f, 0.0f));
				garbagePileTransform.setScale(glm::vec3(3.0f, 0.4f, 3.0f));
				registry->assign<component::Transform>(garbagePileEntity, garbagePileTransform);

				auto& garbagePile = registry->assign<GarbagePile>(garbagePileEntity);
				garbagePile.team = Team::red;
				garbagePile.relativePositionOnShip = i - 1;

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
			mr.material = Material::get("cannonball");
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
			mr.mesh = Mesh::get("mop");
			mr.material = Material::get("mop");

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
			mr.material = Material::get("pirax");
			for (int i = 0; i < 3; i++)
			{
				entt::entity cleaningSolutionEntity = registry->create();

				component::Transform cleaningSolutionTransform;
				cleaningSolutionTransform.setPosition(glm::vec3(3.0f, 0.22f, 3.0f));
				cleaningSolutionTransform.setScale(glm::vec3(0.3f, 0.3f, 0.3f));
				registry->assign<component::Transform>(cleaningSolutionEntity, cleaningSolutionTransform);

				auto& carryableItem = registry->assign<CarryableItem>(cleaningSolutionEntity);
				carryableItem.team = Team::red;
				carryableItem.type = CarryableItemType::cleaningSolution;

				auto& respawnable = registry->assign<Respawnable>(cleaningSolutionEntity);
				respawnable.spawnPosition = glm::vec3(1.7f, 0.2f, 15.4f);
				respawnable.spawnRotation = glm::vec3(0.0f);

				auto& rb = registry->assign<component::RigidBody>(cleaningSolutionEntity);

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
			mr.material = Material::get("goop");
			for (int i = 0; i < 3; i++)
			{
				entt::entity gloopEntity = registry->create();

				component::Transform gloopTransform;
				gloopTransform.setPosition(glm::vec3(3.0f, 0.27f, 3.0f));
				gloopTransform.setScale(glm::vec3(0.1f, 0.1f, 0.1f));
				registry->assign<component::Transform>(gloopEntity, gloopTransform);

				auto& carryableItem = registry->assign<CarryableItem>(gloopEntity);
				carryableItem.team = Team::red;
				carryableItem.type = CarryableItemType::gloop;

				auto& respawnable = registry->assign<Respawnable>(gloopEntity);
				respawnable.spawnPosition = glm::vec3(2.4f, 0.2f, 14.0f);
				respawnable.spawnRotation = glm::vec3(0.0f);

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
			mr.material = Material::get("crate");

			entt::entity cannonballCrateEntity = registry->create();

			component::Transform cannonballCrateTransform;
			cannonballCrateTransform.setPosition(glm::vec3(-1.0f, 0.5f, -3.0f));
			cannonballCrateTransform.setRotation(glm::vec3(0.0f, -90.0f, 0.0f));
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

		{
			//CLEANING SOLUTION SPAWNER
			//NOTE: MAKE SURE THIS IS INITIALIZED AFTER CLEANING SOLUTION ENTITIES
			entt::entity cleaningSolutionSpawnerEntity = registry->create();

			auto& spawner = registry->assign<RespawnManager>(cleaningSolutionSpawnerEntity);
			spawner.respawnTimerDuration = 10.0f;
			spawner.team = Team::red;
			spawner.type = CarryableItemType::cleaningSolution;

			auto respawnableItemsView = registry->view<Respawnable, CarryableItem, component::Transform>();
			for (auto& respawnableEntity : respawnableItemsView)
			{
				auto& carryableItem = registry->get<CarryableItem>(respawnableEntity);

				if (carryableItem.type == spawner.type && carryableItem.team == spawner.team)
				{
					spawner.entityPrefab = registry->create();
					registry->stomp(spawner.entityPrefab, respawnableEntity, *registry);

					auto& newTransform  = registry->get<component::Transform>(spawner.entityPrefab);
					auto& newEntityInfo = registry->get<component::EntityInfo>(spawner.entityPrefab);
					auto& newCarryable  = registry->get<CarryableItem>(spawner.entityPrefab);

					newCarryable.hasBeenCarried = true; //we have to do this or else the prefab entity will stop this spawner from ever spawning an item
					newTransform.setPosition(glm::vec3(-99999.0f));
					newEntityInfo.name = "RedCleaningSolutionPrefab";
					break;
				}
			}

			auto& spawnerTransform = registry->assign<component::Transform>(cleaningSolutionSpawnerEntity);
			spawnerTransform.setPosition(glm::vec3(0.0f));

			auto& spawnerInfo = registry->assign<component::EntityInfo>(cleaningSolutionSpawnerEntity);
			spawnerInfo.name = "RedCleaningSolutionSpawner";
		}

		{
			//GLOOP SPAWNER
			//NOTE: MAKE SURE THIS IS INITIALIZED AFTER GLOOP ENTITIES
			entt::entity gloopSpawnerEntity = registry->create();

			auto& spawner = registry->assign<RespawnManager>(gloopSpawnerEntity);
			spawner.respawnTimerDuration = 20.0f;
			spawner.team = Team::red;
			spawner.type = CarryableItemType::gloop;

			auto respawnableItemsView = registry->view<Respawnable, CarryableItem, component::Transform>();
			for (auto& respawnableEntity : respawnableItemsView)
			{
				auto& carryableItem = registry->get<CarryableItem>(respawnableEntity);

				if (carryableItem.type == spawner.type && carryableItem.team == spawner.team)
				{
					spawner.entityPrefab = registry->create();
					registry->stomp(spawner.entityPrefab, respawnableEntity, *registry);

					auto& newTransform  = registry->get<component::Transform>(spawner.entityPrefab);
					auto& newEntityInfo = registry->get<component::EntityInfo>(spawner.entityPrefab);
					auto& newCarryable  = registry->get<CarryableItem>(spawner.entityPrefab);

					newCarryable.hasBeenCarried = true; //we have to do this or else the prefab entity will stop this spawner from ever spawning an item
					newTransform.setPosition(glm::vec3(-99999.0f));
					newEntityInfo.name = "RedGloopPrefab";
					break;
				}
			}

			auto& spawnerTransform = registry->assign<component::Transform>(gloopSpawnerEntity);
			spawnerTransform.setPosition(glm::vec3(0.0f));

			auto& spawnerInfo = registry->assign<component::EntityInfo>(gloopSpawnerEntity);
			spawnerInfo.name = "RedGloopSpawner";
		}
		/////////////////////////////////////////////////////
		/////////////////////////////////////////////////////

		{
			//LAMP
			mr.mesh = Mesh::cache("res/assets/models/lamp.obj");
			mr.material = Material::get("lamp");

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
			//BOAT HULL
			entt::entity boatHullEntity = registry->create();
			mr.material = Material::get("siding");

			component::Transform boatHullTransform;
			boatHullTransform.setPosition(glm::vec3(-21.0f, 0.0f, -2.0f));
			boatHullTransform.setScale(glm::vec3(1.0f, 1.0f, 1.0f));
			registry->assign<component::Transform>(boatHullEntity, boatHullTransform);

			mr.mesh = Mesh::cache("res/assets/models/BoatHull.obj");
			registry->assign<component::Renderable>(boatHullEntity, mr);

			auto& so2 = registry->assign<component::EntityInfo>(boatHullEntity);
			so2.name = "Boat Hull";

			auto& boatHullCollider = registry->assign<component::Collidable>(boatHullEntity);
			auto& shapeInfo = boatHullCollider.pushShape(ColliderType::Box);
			shapeInfo.box.setSize({ 0.0f, 0.0f, 0.0f });

		}
		{
			//BOAT HULL
			entt::entity boatHullFLippedEntity = registry->create();
			mr.material = Material::get("siding");

			component::Transform boatHullFlippedTransform;
			boatHullFlippedTransform.setPosition(glm::vec3(-21.0f, 0.0f, -2.0f));
			boatHullFlippedTransform.setScale(glm::vec3(1.0f, 1.0f, 1.0f));
			registry->assign<component::Transform>(boatHullFLippedEntity, boatHullFlippedTransform);

			mr.mesh = Mesh::cache("res/assets/models/BoatHullFlipped.obj");
			registry->assign<component::Renderable>(boatHullFLippedEntity, mr);

			auto& so2 = registry->assign<component::EntityInfo>(boatHullFLippedEntity);
			so2.name = "Boat Hull Inner";

			auto& boatHullCollider = registry->assign<component::Collidable>(boatHullFLippedEntity);
			auto& shapeInfo = boatHullCollider.pushShape(ColliderType::Box);
			shapeInfo.box.setSize({ 0.0f, 0.0f, 0.0f });

		}
		{
			//BOAT DECK
			entt::entity deckBoatEntity = registry->create();
			mr.material = Material::get("planks");

			component::Transform TopDeckBoatTransform;
			TopDeckBoatTransform.setPosition(glm::vec3(3.0f, 0.0f, 2.0f));
			TopDeckBoatTransform.setScale(glm::vec3(1.0f, 1.0f, 1.0f));
			registry->assign<component::Transform>(deckBoatEntity, TopDeckBoatTransform);

			mr.mesh = Mesh::cache("res/assets/models/TopDeck.obj");
			registry->assign<component::Renderable>(deckBoatEntity, mr);

			auto& so2 = registry->assign<component::EntityInfo>(deckBoatEntity);
			so2.name = "Top Deck";

			auto& topDeckCollider = registry->assign<component::Collidable>(deckBoatEntity);
			auto& shapeInfo = topDeckCollider.pushShape(ColliderType::Box);
			shapeInfo.box.setSize({ 1.0f, 0.2f, 0.5f });
		}
		{
			//BOAT DECK RAIL
			entt::entity deckBoatRailEntity = registry->create();
			mr.material = Material::get("planks");

			component::Transform deckBoatRailTransform;
			deckBoatRailTransform.setPosition(glm::vec3(-14.0f, 1.0f, 2.0f));
			deckBoatRailTransform.setScale(glm::vec3(1.0f, 1.0f, 1.0f));
			registry->assign<component::Transform>(deckBoatRailEntity, deckBoatRailTransform);

			mr.mesh = Mesh::cache("res/assets/models/DeckRail.obj");
			registry->assign<component::Renderable>(deckBoatRailEntity, mr);

			auto& so2 = registry->assign<component::EntityInfo>(deckBoatRailEntity);
			so2.name = "DeckRail";

			auto& topDeckCollider = registry->assign<component::Collidable>(deckBoatRailEntity);
			auto& shapeInfo = topDeckCollider.pushShape(ColliderType::Box);
			shapeInfo.box.setSize({ 1.0f, 0.2f, 0.5f });
		}
		{
			//BOAT CAPN QUARTERS
			entt::entity quartersBoatEntity = registry->create();
			mr.material = Material::get("planks");

			component::Transform quartersBoatTransform;
			quartersBoatTransform.setPosition(glm::vec3(8.0f, 1.0f, -3.0f));
			quartersBoatTransform.setScale(glm::vec3(1.0f, 1.0f, 1.0f));
			registry->assign<component::Transform>(quartersBoatEntity, quartersBoatTransform);

			mr.mesh = Mesh::cache("res/assets/models/CapQuarters.obj");
			registry->assign<component::Renderable>(quartersBoatEntity, mr);

			auto& so2 = registry->assign<component::EntityInfo>(quartersBoatEntity);
			so2.name = "Captains Quarters";

			auto& topDeckCollider = registry->assign<component::Collidable>(quartersBoatEntity);
			auto& shapeInfo = topDeckCollider.pushShape(ColliderType::Box);
			shapeInfo.box.setSize({ 1.0f, 0.2f, 0.5f });
		}
		{
			//BOAT CAPN QUARTERS RAIL
			entt::entity quartersRailBoatEntity = registry->create();
			mr.material = Material::get("planks");

			component::Transform quartersRailBoatTransform;
			quartersRailBoatTransform.setPosition(glm::vec3(8.0f, 5.0f, -6.0f));
			quartersRailBoatTransform.setScale(glm::vec3(1.0f, 1.0f, 1.0f));
			registry->assign<component::Transform>(quartersRailBoatEntity, quartersRailBoatTransform);

			mr.mesh = Mesh::cache("res/assets/models/QuartersRail.obj");
			registry->assign<component::Renderable>(quartersRailBoatEntity, mr);

			auto& so2 = registry->assign<component::EntityInfo>(quartersRailBoatEntity);
			so2.name = "Captains Quarters Rail";

			auto& topDeckCollider = registry->assign<component::Collidable>(quartersRailBoatEntity);
			auto& shapeInfo = topDeckCollider.pushShape(ColliderType::Box);
			shapeInfo.box.setSize({ 1.0f, 0.2f, 0.5f });
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
		//GROUND PLANE
		for (int i = 0; i < 4; i++)
		{
			component::Renderable mr;
			mr.mesh = Mesh::cache("res/assets/models/plane.obj");
			mr.material = nullptr;

			entt::entity e = registry->create();
			registry->assign<component::Renderable>(e, mr);

			component::Transform t;
			t.setPosition(glm::vec3(0.0f, -5.0f, 0.0f));
			t.setScale(glm::vec3(1.9f, 0.2f, 0.5f));
			registry->assign<component::Transform>(e, t);

			auto& so = registry->assign<component::EntityInfo>(e);
			so.name = "Plane " + std::to_string(i);

			auto& rb = registry->assign<component::RigidBody>(e);
			rb.setMass(0.0f);
			rb.setFriction(1.0f);

			auto& cl = registry->assign<component::Collidable>(e);

			auto& shi = cl.pushShape(ColliderType::Box);
			shi.box.setSize({ 20.0f, 0.1f, 20.0f });
		}
    }

    {
		//RANDOM SPHERES

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
