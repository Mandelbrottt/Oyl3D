#include <Oyl3D.h>

#include "SandboxLayer.h"

using namespace oyl;

void SandboxLayer::onEnter()
{
	//any meshes that aren't loaded in the scene need to be cached here for distribution mode
	auto mesh = Model::cache("res/assets/models/cube.obj");
	mesh = Model::cache("res/assets/models/Garbage/garbageSmall.obj");
	mesh = Model::cache("res/assets/models/Garbage/garbageMedium.obj");
    mesh = Model::cache("res/assets/models/Garbage/garbageLarge.obj");
    mesh = Model::cache("res/assets/models/Garbage/garbageMassive.obj");
    mesh = Model::cache("res/assets/models/Garbage/garbageSurrender.obj");

    {
        component::Renderable mr;
        mr.model     = mesh;

		/////////////////////////////////////////////////////
		//////////// SINGLE INSTANCE ANIMATIONS /////////////
		{
			for (int i = 0; i < 2; i++)
			{
				entt::entity waterCrestEntity = registry->create();

				component::Transform waterCrestTransform;
				waterCrestTransform.setPosition(glm::vec3(0.0f, 0.0f, 0.0f));

				registry->assign<component::Transform>(waterCrestEntity, waterCrestTransform);
				registry->assign<component::Renderable>(waterCrestEntity, mr);

				auto& so1 = registry->assign<component::EntityInfo>(waterCrestEntity);
				so1.name = "WaterCrest" + std::to_string(i);

				char filename[512];
				std::string s;
				s.reserve(512);
				const int numFrames = 2; // however many frames you exported
				//auto& animator = registry->assign<component::Animatable>(waterCrestEntity);

				//loading animation
				//auto waterCrestAnimation = Ref<Animation>::create();
				//for (int i = 0; i < numFrames; i++)
				//{
				//	Animation::KeyPose kp;
				//	kp.duration = 1.0f / 5.0f; // duration in seconds

				//	sprintf(filename, "res/assets/animation/WaterCrest/WaterCrestAnim_%06d.obj", i + 1);
				//	s.assign(filename); // you have to load each mesh yourself for now
				//						// the "%06d" is because by default blender exports
				//						// frames as "animationName_000001.obj" and so on counting up

				//	kp.model = Model::create(s);
				//	waterCrestAnimation->poses.push_back(kp); // Add the keyframe to the animation

				//}
				//animator.pushAnimation("WaterCrestAnim", waterCrestAnimation); // add the animation to the animator
			}
		}





		/////////////////////////////////////////////////////
		///////////////////// BLUE TEAM /////////////////////
		{
			//PLAYER 1
			entt::entity player1BlueEntity = registry->create();
			auto& player = registry->assign<Player>(player1BlueEntity);
			player.playerNum     = PlayerNumber::One;
			player.controllerNum = (uint)player.playerNum;
			player.team          = Team::blue;

			auto& renderable = registry->assign<component::Renderable>(player1BlueEntity, mr);
			renderable.cullingMask = ~0b0001;

			component::Transform t2;
			t2.setPosition(glm::vec3(0.0f));
			registry->assign<component::Transform>(player1BlueEntity, t2);

			auto& so2 = registry->assign<component::EntityInfo>(player1BlueEntity);
			so2.name = "Player1 Blue";

			auto& rb = registry->assign<component::RigidBody>(player1BlueEntity);
			rb.setMass(5.0f);
			rb.setFriction(0.9f);
			//rb.setProperties(component::RigidBody::Property::IS_KINEMATIC, true);

			auto& playerCollider = registry->assign<component::Collidable>(player1BlueEntity);
			auto& shapeInfo = playerCollider.pushShape(ColliderType::Box);
			shapeInfo.box.setSize({ 1.0f, 1.0f, 1.0f });

			//PLAYER 1 CAMERA
			auto playerCameraEntity = registry->create();

			auto& cameraTransform = registry->assign<component::Transform>(playerCameraEntity);
			cameraTransform.setPosition(glm::vec3(0.0f, 0.8f, -0.5f));
			cameraTransform.setParent(player1BlueEntity);

			auto& camera = registry->assign<component::Camera>(playerCameraEntity);
			camera.player = player.playerNum;
			camera.cullingMask = 0b0001;

			auto& cameraBreathing = registry->assign<CameraBreathing>(playerCameraEntity);
			cameraBreathing.startPosY = cameraTransform.getPositionY();

			auto& cameraEI = registry->assign<component::EntityInfo>(playerCameraEntity);
			cameraEI.name = "Player1 Camera";
		}

		{
			//PLAYER 3
			entt::entity player3BlueEntity = registry->create();
			auto& player = registry->assign<Player>(player3BlueEntity);
			player.playerNum = PlayerNumber::Three;
			player.controllerNum = (uint)player.playerNum;
			player.team = Team::blue;

			auto& renderable = registry->assign<component::Renderable>(player3BlueEntity, mr);
			renderable.cullingMask = ~0b0100;

			component::Transform t2;
			t2.setPosition(glm::vec3(0.0f));
			registry->assign<component::Transform>(player3BlueEntity, t2);

			auto& so2 = registry->assign<component::EntityInfo>(player3BlueEntity);
			so2.name = "Player3 Blue";

			auto& rb = registry->assign<component::RigidBody>(player3BlueEntity);
			rb.setMass(5.0f);
			rb.setFriction(0.9f);
			rb.setProperties(component::RigidBody::Property::FREEZE_ROTATION_X, true);
			rb.setProperties(component::RigidBody::Property::FREEZE_ROTATION_Y, true);
			rb.setProperties(component::RigidBody::Property::FREEZE_ROTATION_Z, true);
			//rb.setProperties(component::RigidBody::Property::IS_KINEMATIC, true);

			auto& playerCollider = registry->assign<component::Collidable>(player3BlueEntity);
			auto& shapeInfo = playerCollider.pushShape(ColliderType::Box);
			shapeInfo.box.setSize({ 1.0f, 1.0f, 1.0f });

			//PLAYER 3 CAMERA
			auto playerCameraEntity = registry->create();

			auto& cameraTransform = registry->assign<component::Transform>(playerCameraEntity);
			cameraTransform.setPosition(glm::vec3(0.0f, 0.8f, -0.5f));
			cameraTransform.setParent(player3BlueEntity);

			auto& camera = registry->assign<component::Camera>(playerCameraEntity);
			camera.player = player.playerNum;
			camera.cullingMask = 0b0100;

			auto& cameraBreathing = registry->assign<CameraBreathing>(playerCameraEntity);
			cameraBreathing.startPosY = cameraTransform.getPositionY();

			auto& cameraEI = registry->assign<component::EntityInfo>(playerCameraEntity);
			cameraEI.name = "Player3 Camera";
		}
        
		{
            
			//CANNON
			entt::entity cannonEntity = registry->create();

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
			for (int i = 0; i < 3; i++)
			{
				//creating the fly
				entt::entity garbagePileFlyEntity = registry->create();

				component::Transform garbagePileFlyTransform;
				garbagePileFlyTransform.setPosition(glm::vec3(0.0f, 0.0f, 0.0f));

				registry->assign<component::Transform>(garbagePileFlyEntity, garbagePileFlyTransform);
				registry->assign<component::Renderable>(garbagePileFlyEntity, mr);

				auto& so1 = registry->assign<component::EntityInfo>(garbagePileFlyEntity);
				so1.name = "BlueGarbagePileFly" + std::to_string(i);

				char filename[512];
				std::string s;
				s.reserve(512);
				const int numFrames = 100; // however many frames you exported
				//auto& animator = registry->assign<component::Animatable>(garbagePileFlyEntity);

				////loading fly animations
				//auto tinyAnimation = Ref<Animation>::create();
				//for (int i = 0; i < numFrames; i++)
				//{
				//	Animation::KeyPose kp;
				//	kp.duration = 1.0f / 30.0f; // duration in seconds

				//	sprintf(filename, "res/assets/animation/Garbage/GarbageTiny/GarbageTinyAnim_%06d.obj", i + 1);
				//	s.assign(filename); // you have to load each mesh yourself for now
				//						// the "%06d" is because by default blender exports
				//						// frames as "animationName_000001.obj" and so on counting up

				//	kp.model = Model::create(s);
				//	tinyAnimation->poses.push_back(kp); // Add the keyframe to the animation

				//}
				//animator.pushAnimation("GarbageTinyAnim", tinyAnimation); // add the animation to the animator

				////Small Garbage fly anim
				//auto smallAnimation = Ref<Animation>::create();
				//for (int i = 0; i < numFrames; i++)
				//{
				//	Animation::KeyPose kp;
				//	kp.duration = 1.0f / 30.0f; // duration in seconds

				//	sprintf(filename, "res/assets/animation/Garbage/GarbageSmall/GarbageSmallAnim_%06d.obj", i + 1);
				//	s.assign(filename); // you have to load each mesh yourself for now
				//						// the "%06d" is because by default blender exports
				//						// frames as "animationName_000001.obj" and so on counting up

				//	kp.model = Model::create(s);
				//	smallAnimation->poses.push_back(kp); // Add the keyframe to the animation

				//}
				//animator.pushAnimation("GarbageSmallAnim", smallAnimation); // add the animation to the animator

				////Medium Garbage fly anim
				//auto mediumAnimation = Ref<Animation>::create();
				//for (int i = 0; i < numFrames; i++)
				//{
				//	Animation::KeyPose kp;
				//	kp.duration = 1.0f / 30.0f; // duration in seconds

				//	sprintf(filename, "res/assets/animation/Garbage/GarbageMedium/GarbageMediumAnim_%06d.obj", i + 1);
				//	s.assign(filename); // you have to load each mesh yourself for now
				//						// the "%06d" is because by default blender exports
				//						// frames as "animationName_000001.obj" and so on counting up

				//	kp.model = Model::create(s);
				//	mediumAnimation->poses.push_back(kp); // Add the keyframe to the animation

				//}
				//animator.pushAnimation("GarbageMediumAnim", mediumAnimation); // add the animation to the animator

				////Large Garbage fly anim
				//auto largeAnimation = Ref<Animation>::create();
				//for (int i = 0; i < numFrames; i++)
				//{
				//	Animation::KeyPose kp;
				//	kp.duration = 1.0f / 30.0f; // duration in seconds

				//	sprintf(filename, "res/assets/animation/Garbage/GarbageLarge/GarbageLargeAnim_%06d.obj", i + 1);
				//	s.assign(filename); // you have to load each mesh yourself for now
				//						// the "%06d" is because by default blender exports
				//						// frames as "animationName_000001.obj" and so on counting up

				//	kp.model = Model::create(s);
				//	largeAnimation->poses.push_back(kp); // Add the keyframe to the animation

				//}
				//animator.pushAnimation("GarbageLargeAnim", largeAnimation); // add the animation to the animator

				////Massive Garbage fly anim
				//auto massiveAnimation = Ref<Animation>::create();
				//for (int i = 0; i < numFrames; i++)
				//{
				//	Animation::KeyPose kp;
				//	kp.duration = 1.0f / 30.0f; // duration in seconds

				//	sprintf(filename, "res/assets/animation/Garbage/GarbageMassive/GarbageMassiveAnim_%06d.obj", i + 1);
				//	s.assign(filename); // you have to load each mesh yourself for now
				//						// the "%06d" is because by default blender exports
				//						// frames as "animationName_000001.obj" and so on counting up

				//	kp.model = Model::create(s);
				//	massiveAnimation->poses.push_back(kp); // Add the keyframe to the animation

				//}
				//animator.pushAnimation("GarbageMassiveAnim", massiveAnimation); // add the animation to the animator

				////Surrender Garbage fly anim
				//auto surrenderAnimation = Ref<Animation>::create();
				//for (int i = 0; i < numFrames; i++)
				//{
				//	Animation::KeyPose kp;
				//	kp.duration = 1.0f / 30.0f; // duration in seconds

				//	sprintf(filename, "res/assets/animation/Garbage/GarbageSurrender/GarbageSurrenderAnim_%06d.obj", i + 1);
				//	s.assign(filename); // you have to load each mesh yourself for now
				//						// the "%06d" is because by default blender exports
				//						// frames as "animationName_000001.obj" and so on counting up

				//	kp.model = Model::create(s);
				//	surrenderAnimation->poses.push_back(kp); // Add the keyframe to the animation

				//}
				//animator.pushAnimation("GarbageSurrenderAnim", surrenderAnimation); // add the animation to the animator

				////set default
				//animator.setNextAnimation("GarbageTinyAnim");

				//garbage
				entt::entity garbagePileEntity = registry->create();

				component::Transform garbagePileTransform;
				garbagePileTransform.setPosition(glm::vec3(-3.0f, 0.2f, 0.0f));
				garbagePileTransform.setScale(glm::vec3(0.43f, 0.4f, 0.43f));
				registry->assign<component::Transform>(garbagePileEntity, garbagePileTransform);

				auto& garbagePile = registry->assign<GarbagePile>(garbagePileEntity);
				garbagePile.team = Team::blue;
				garbagePile.relativePositionOnShip = i - 1;
				garbagePile.flyEntity = garbagePileFlyEntity;

				auto& rb = registry->assign<component::RigidBody>(garbagePileEntity);
				rb.setMass(0.0f);

				registry->assign<component::Renderable>(garbagePileEntity, mr);

				auto& so2 = registry->assign<component::EntityInfo>(garbagePileEntity);
				so2.name = "BlueGarbagePile" + std::to_string(i);

				auto& garbagePileCollider = registry->assign<component::Collidable>(garbagePileEntity);
				auto& shapeInfo = garbagePileCollider.pushShape(ColliderType::Box);
				shapeInfo.box.setSize({ 3.4f, 1.0f, 3.2f });

				//Parenting the fly to the garbage
				/*auto& flyParent = registry->assign<component::Parent>(garbagePileFlyEntity);
				flyParent.parent = garbagePileEntity;*/

			}
		}

		{
			//CANNONBALLS
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

				mr.model = Model::get("sphere");
				registry->assign<component::Renderable>(cannonballEntity, mr);

				auto& so2 = registry->assign<component::EntityInfo>(cannonballEntity);
				so2.name = "BlueCannonball" + std::to_string(i);

				auto& cannonballCollider = registry->assign<component::Collidable>(cannonballEntity);
				auto& shapeInfo = cannonballCollider.pushShape(ColliderType::Sphere);
				shapeInfo.sphere.setRadius(0.5f);
			}
		}

        {
			//MOPS
			for (int i = 0; i < 2; i++)
			{
				entt::entity mopEntity = registry->create();

				component::Transform mopTransform;
				mopTransform.setPosition(glm::vec3(3.0f, 0.1f, 0.0f));
				mopTransform.setScale(glm::vec3(2.0f, 0.2f, 0.2f));
				registry->assign<component::Transform>(mopEntity, mopTransform);

				auto& carryableItem = registry->assign<CarryableItem>(mopEntity);
				carryableItem.team = Team::blue;
				carryableItem.type = CarryableItemType::mop;

				auto& rb = registry->assign<component::RigidBody>(mopEntity);
				rb.setMass(2.0f);
				rb.setFriction(0.6f);

				mr.model = Model::get("cube");
				registry->assign<component::Renderable>(mopEntity, mr);

				auto& so2 = registry->assign<component::EntityInfo>(mopEntity);
				so2.name = "BlueMop " + std::to_string(i+1);

				auto& mopCollider = registry->assign<component::Collidable>(mopEntity);
				auto& shapeInfo = mopCollider.pushShape(ColliderType::Box);
				shapeInfo.box.setSize({ 2.0f, 0.2f, 0.5f });
			}
        }

		{
			//CLEANING SOLUTION
			entt::entity cleaningSolutionEntity = registry->create();

			component::Transform cleaningSolutionTransform;
			cleaningSolutionTransform.setPosition(glm::vec3(9.43f, 0.99f, -5.56f));
			cleaningSolutionTransform.setScale(glm::vec3(0.3f, 0.3f, 0.3f));
			registry->assign<component::Transform>(cleaningSolutionEntity, cleaningSolutionTransform);

			auto& carryableItem = registry->assign<CarryableItem>(cleaningSolutionEntity);
			carryableItem.team = Team::blue;
			carryableItem.type = CarryableItemType::cleaningSolution;

			auto& respawnable = registry->assign<Respawnable>(cleaningSolutionEntity);
			respawnable.spawnPosition = glm::vec3(9.43f, 1.04f, -5.56f);
			respawnable.spawnRotation = glm::vec3(0.0f, 87.7f, 0.0f);

			auto& rb = registry->assign<component::RigidBody>(cleaningSolutionEntity);

			registry->assign<component::Renderable>(cleaningSolutionEntity, mr);

			auto& so2 = registry->assign<component::EntityInfo>(cleaningSolutionEntity);
			so2.name = "BlueCleaningSolution 0";

			auto& cleaningSolutionCollider = registry->assign<component::Collidable>(cleaningSolutionEntity);
			auto& shapeInfo = cleaningSolutionCollider.pushShape(ColliderType::Box);
			shapeInfo.box.setSize({ 0.8f, 1.0f, 0.85f });
		}

		{
			//GLOOP
			entt::entity gloopEntity = registry->create();

			component::Transform gloopTransform;
			gloopTransform.setPosition(glm::vec3(9.46f, 2.0f, -5.95f));
			gloopTransform.setScale(glm::vec3(0.1f, 0.1f, 0.1f));
			registry->assign<component::Transform>(gloopEntity, gloopTransform);

			auto& carryableItem = registry->assign<CarryableItem>(gloopEntity);
			carryableItem.team = Team::blue;
			carryableItem.type = CarryableItemType::gloop;

			auto& respawnable = registry->assign<Respawnable>(gloopEntity);
			respawnable.spawnPosition = glm::vec3(9.46f, 2.07f, -5.95f);
			respawnable.spawnRotation = glm::vec3(0.0f, 83.82f, 0.0f);

			auto& rb = registry->assign<component::RigidBody>(gloopEntity);

			registry->assign<Gloop>(gloopEntity);

			registry->assign<component::Renderable>(gloopEntity, mr);

			auto& so2 = registry->assign<component::EntityInfo>(gloopEntity);
			so2.name = "BlueGloop 0";

			auto& gloopCollider = registry->assign<component::Collidable>(gloopEntity);
			auto& shapeInfo = gloopCollider.pushShape(ColliderType::Box);
			shapeInfo.box.setSize({ 1.55f, 2.65f, 1.0f });
		}

		{
			//CANNONBALL CRATE
			entt::entity cannonballCrateEntity = registry->create();

			component::Transform cannonballCrateTransform;
			cannonballCrateTransform.setPosition(glm::vec3(-1.0f, 0.5f, -3.0f));
			cannonballCrateTransform.setRotation(glm::vec3(0.0f, -90.0f, 0.0f));
			cannonballCrateTransform.setScale(glm::vec3(2.0f, 1.0f, 1.0f));
			registry->assign<component::Transform>(cannonballCrateEntity, cannonballCrateTransform);

			auto& cannonballCrate = registry->assign<CannonballCrate>(cannonballCrateEntity);
			cannonballCrate.team = Team::blue;

			//mr.material = mat;
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
			spawner.respawnTimerDuration = 5.0f;
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
			spawner.respawnTimerDuration = 10.0f;
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
			//PLAYER 2
			entt::entity player2RedEntity = registry->create();
			auto& player = registry->assign<Player>(player2RedEntity);
			player.playerNum = PlayerNumber::Two;
			player.controllerNum = (uint)player.playerNum;
			player.team = Team::red;

			auto& renderable = registry->assign<component::Renderable>(player2RedEntity, mr);
			renderable.cullingMask = ~0b0010;

			component::Transform t2;
			t2.setPosition(glm::vec3(0.0f));
			registry->assign<component::Transform>(player2RedEntity, t2);

			auto& so2 = registry->assign<component::EntityInfo>(player2RedEntity);
			so2.name = "Player2 Red";

			auto& rb = registry->assign<component::RigidBody>(player2RedEntity);
			rb.setMass(5.0f);
			rb.setFriction(0.9f);
			rb.setProperties(component::RigidBody::Property::FREEZE_ROTATION_X, true);
			rb.setProperties(component::RigidBody::Property::FREEZE_ROTATION_Y, true);
			rb.setProperties(component::RigidBody::Property::FREEZE_ROTATION_Z, true);
			//rb.setProperties(component::RigidBody::Property::IS_KINEMATIC, true);

			auto& playerCollider = registry->assign<component::Collidable>(player2RedEntity);
			auto& shapeInfo = playerCollider.pushShape(ColliderType::Box);
			shapeInfo.box.setSize({ 1.0f, 1.0f, 1.0f });

			//PLAYER 2 CAMERA
			auto playerCameraEntity = registry->create();

			auto& cameraTransform = registry->assign<component::Transform>(playerCameraEntity);
			cameraTransform.setPosition(glm::vec3(0.0f, 0.8f, -0.5f));
			cameraTransform.setParent(player2RedEntity);

			auto& camera = registry->assign<component::Camera>(playerCameraEntity);
			camera.player = player.playerNum;
			camera.cullingMask = 0b0010;

			auto& cameraBreathing = registry->assign<CameraBreathing>(playerCameraEntity);
			cameraBreathing.startPosY = cameraTransform.getPositionY();

			auto& cameraEI = registry->assign<component::EntityInfo>(playerCameraEntity);
			cameraEI.name = "Player2 Camera";
		}

		{
			//PLAYER 4
			entt::entity player4RedEntity = registry->create();
			auto& player = registry->assign<Player>(player4RedEntity);
			player.playerNum = PlayerNumber::Four;
			player.controllerNum = (uint)player.playerNum;
			player.team = Team::red;

			auto& renderable = registry->assign<component::Renderable>(player4RedEntity, mr);
			renderable.cullingMask = ~0b1000;

			component::Transform t2;
			t2.setPosition(glm::vec3(0.0f));
			registry->assign<component::Transform>(player4RedEntity, t2);

			auto& so2 = registry->assign<component::EntityInfo>(player4RedEntity);
			so2.name = "Player4 Red";

			auto& rb = registry->assign<component::RigidBody>(player4RedEntity);
			rb.setMass(5.0f);
			rb.setFriction(0.9f);
			rb.setProperties(component::RigidBody::Property::FREEZE_ROTATION_X, true);
			rb.setProperties(component::RigidBody::Property::FREEZE_ROTATION_Y, true);
			rb.setProperties(component::RigidBody::Property::FREEZE_ROTATION_Z, true);
			//rb.setProperties(component::RigidBody::Property::IS_KINEMATIC, true);

			auto& playerCollider = registry->assign<component::Collidable>(player4RedEntity);
			auto& shapeInfo = playerCollider.pushShape(ColliderType::Box);
			shapeInfo.box.setSize({ 1.0f, 1.0f, 1.0f });

			//PLAYER 4 CAMERA
			auto playerCameraEntity = registry->create();

			auto& cameraTransform = registry->assign<component::Transform>(playerCameraEntity);
			cameraTransform.setPosition(glm::vec3(0.0f, 0.8f, -0.5f));
			cameraTransform.setParent(player4RedEntity);

			auto& camera = registry->assign<component::Camera>(playerCameraEntity);
			camera.player = player.playerNum;
			camera.cullingMask = 0b1000;

			auto& cameraBreathing = registry->assign<CameraBreathing>(playerCameraEntity);
			cameraBreathing.startPosY = cameraTransform.getPositionY();

			auto& cameraEI = registry->assign<component::EntityInfo>(playerCameraEntity);
			cameraEI.name = "Player4 Camera";
		}

		{
			//CANNON
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

			for (int i = 0; i < 3; i++)
			{
				//creating the fly
				entt::entity garbagePileFlyEntity = registry->create();

				component::Transform garbagePileFlyTransform;
				garbagePileFlyTransform.setPosition(glm::vec3(0.0f, 0.0f, 0.0f));
				registry->assign<component::Transform>(garbagePileFlyEntity, garbagePileFlyTransform);

				registry->assign<component::Renderable>(garbagePileFlyEntity, mr);

				auto& so1 = registry->assign<component::EntityInfo>(garbagePileFlyEntity);
				so1.name = "RedGarbagePileFly" + std::to_string(i);

				char filename[512];
				std::string s;
				s.reserve(512);
				const int numFrames = 100; // however many frames you exported
				//auto& animator = registry->assign<component::Animatable>(garbagePileFlyEntity);

				//loading fly animations
				//auto tinyAnimation = Ref<Animation>::create();
				//for (int i = 0; i < numFrames; i++)
				//{
				//	Animation::KeyPose kp;
				//	kp.duration = 1.0f / 30.0f; // duration in seconds

				//	sprintf(filename, "res/assets/animation/Garbage/GarbageTiny/GarbageTinyAnim_%06d.obj", i + 1);
				//	s.assign(filename); // you have to load each mesh yourself for now
				//						// the "%06d" is because by default blender exports
				//						// frames as "animationName_000001.obj" and so on counting up

				//	kp.model = Model::create(s);
				//	tinyAnimation->poses.push_back(kp); // Add the keyframe to the animation

				//}
				//animator.pushAnimation("GarbageTinyAnim", tinyAnimation); // add the animation to the animator

				////Small Garbage fly anim
				//auto smallAnimation = Ref<Animation>::create();
				//for (int i = 0; i < numFrames; i++)
				//{
				//	Animation::KeyPose kp;
				//	kp.duration = 1.0f / 30.0f; // duration in seconds

				//	sprintf(filename, "res/assets/animation/Garbage/GarbageSmall/GarbageSmallAnim_%06d.obj", i + 1);
				//	s.assign(filename); // you have to load each mesh yourself for now
				//						// the "%06d" is because by default blender exports
				//						// frames as "animationName_000001.obj" and so on counting up

				//	kp.model = Model::create(s);
				//	smallAnimation->poses.push_back(kp); // Add the keyframe to the animation

				//}
				//animator.pushAnimation("GarbageSmallAnim", smallAnimation); // add the animation to the animator

				////Medium Garbage fly anim
				//auto mediumAnimation = Ref<Animation>::create();
				//for (int i = 0; i < numFrames; i++)
				//{
				//	Animation::KeyPose kp;
				//	kp.duration = 1.0f / 30.0f; // duration in seconds

				//	sprintf(filename, "res/assets/animation/Garbage/GarbageMedium/GarbageMediumAnim_%06d.obj", i + 1);
				//	s.assign(filename); // you have to load each mesh yourself for now
				//						// the "%06d" is because by default blender exports
				//						// frames as "animationName_000001.obj" and so on counting up

				//	kp.model = Model::create(s);
				//	mediumAnimation->poses.push_back(kp); // Add the keyframe to the animation

				//}
				//animator.pushAnimation("GarbageMediumAnim", mediumAnimation); // add the animation to the animator

				////Large Garbage fly anim
				//auto largeAnimation = Ref<Animation>::create();
				//for (int i = 0; i < numFrames; i++)
				//{
				//	Animation::KeyPose kp;
				//	kp.duration = 1.0f / 30.0f; // duration in seconds

				//	sprintf(filename, "res/assets/animation/Garbage/GarbageLarge/GarbageLargeAnim_%06d.obj", i + 1);
				//	s.assign(filename); // you have to load each mesh yourself for now
				//						// the "%06d" is because by default blender exports
				//						// frames as "animationName_000001.obj" and so on counting up

				//	kp.model = Model::create(s);
				//	largeAnimation->poses.push_back(kp); // Add the keyframe to the animation

				//}
				//animator.pushAnimation("GarbageLargeAnim", largeAnimation); // add the animation to the animator

				////Massive Garbage fly anim
				//auto massiveAnimation = Ref<Animation>::create();
				//for (int i = 0; i < numFrames; i++)
				//{
				//	Animation::KeyPose kp;
				//	kp.duration = 1.0f / 30.0f; // duration in seconds

				//	sprintf(filename, "res/assets/animation/Garbage/GarbageMassive/GarbageMassiveAnim_%06d.obj", i + 1);
				//	s.assign(filename); // you have to load each mesh yourself for now
				//						// the "%06d" is because by default blender exports
				//						// frames as "animationName_000001.obj" and so on counting up

				//	kp.model = Model::create(s);
				//	massiveAnimation->poses.push_back(kp); // Add the keyframe to the animation

				//}
				//animator.pushAnimation("GarbageMassiveAnim", massiveAnimation); // add the animation to the animator

				////Surrender Garbage fly anim
				//auto surrenderAnimation = Ref<Animation>::create();
				//for (int i = 0; i < numFrames; i++)
				//{
				//	Animation::KeyPose kp;
				//	kp.duration = 1.0f / 30.0f; // duration in seconds

				//	sprintf(filename, "res/assets/animation/Garbage/GarbageSurrender/GarbageSurrenderAnim_%06d.obj", i + 1);
				//	s.assign(filename); // you have to load each mesh yourself for now
				//						// the "%06d" is because by default blender exports
				//						// frames as "animationName_000001.obj" and so on counting up

				//	kp.model = Model::create(s);
				//	surrenderAnimation->poses.push_back(kp); // Add the keyframe to the animation

				//}
				//animator.pushAnimation("GarbageSurrenderAnim", surrenderAnimation); // add the animation to the animator

				//animator.setNextAnimation("GarbageTinyAnim");

				//creating the garbage
				entt::entity garbagePileEntity = registry->create();

				component::Transform garbagePileTransform;
				garbagePileTransform.setPosition(glm::vec3(-3.0f, 0.2f, 0.0f));
				garbagePileTransform.setScale(glm::vec3(0.43f, 0.4f, 0.43f));
				registry->assign<component::Transform>(garbagePileEntity, garbagePileTransform);

				auto& garbagePile = registry->assign<GarbagePile>(garbagePileEntity);
				garbagePile.team = Team::red;
				garbagePile.relativePositionOnShip = i - 1;
				garbagePile.flyEntity = garbagePileFlyEntity;

				auto& rb = registry->assign<component::RigidBody>(garbagePileEntity);
				rb.setMass(0.0f);

				registry->assign<component::Renderable>(garbagePileEntity, mr);

				auto& so2 = registry->assign<component::EntityInfo>(garbagePileEntity);
				so2.name = "RedGarbagePile" + std::to_string(i);

				auto& garbagePileCollider = registry->assign<component::Collidable>(garbagePileEntity);
				auto& shapeInfo = garbagePileCollider.pushShape(ColliderType::Box);
				shapeInfo.box.setSize({ 3.4f, 1.0f, 3.2f });

				//Parenting the fly to the garbage
				/*auto& flyParent = registry->assign<component::Parent>(garbagePileFlyEntity);
				flyParent.parent = garbagePileEntity;*/


			}
		}

		{
			//CANNONBALLS
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

				mr.model = Model::get("sphere");
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
			for (int i = 0; i < 2; i++)
			{

				entt::entity mopEntity = registry->create();

				component::Transform mopTransform;
				mopTransform.setPosition(glm::vec3(3.0f, 0.1f, 0.0f));
				mopTransform.setScale(glm::vec3(2.0f, 0.2f, 0.2f));
				registry->assign<component::Transform>(mopEntity, mopTransform);

				auto& carryableItem = registry->assign<CarryableItem>(mopEntity);
				carryableItem.team = Team::red;
				carryableItem.type = CarryableItemType::mop;

				auto& rb = registry->assign<component::RigidBody>(mopEntity);
				rb.setMass(2.0f);
				rb.setFriction(0.6f);

				mr.model = Model::get("cube");
				registry->assign<component::Renderable>(mopEntity, mr);

				auto& so2 = registry->assign<component::EntityInfo>(mopEntity);
				so2.name = "RedMop " + std::to_string(i+1);

				auto& mopCollider = registry->assign<component::Collidable>(mopEntity);
				auto& shapeInfo = mopCollider.pushShape(ColliderType::Box);
				shapeInfo.box.setSize({ 2.0f, 0.2f, 0.5f });
			}
		}

		{
			//CLEANING SOLUTION
			entt::entity cleaningSolutionEntity = registry->create();

			component::Transform cleaningSolutionTransform;
			cleaningSolutionTransform.setPosition(glm::vec3(9.54f, 0.99f, 26.34f));
			cleaningSolutionTransform.setScale(glm::vec3(0.3f, 0.3f, 0.3f));
			registry->assign<component::Transform>(cleaningSolutionEntity, cleaningSolutionTransform);

			auto& carryableItem = registry->assign<CarryableItem>(cleaningSolutionEntity);
			carryableItem.team = Team::red;
			carryableItem.type = CarryableItemType::cleaningSolution;

			auto& respawnable = registry->assign<Respawnable>(cleaningSolutionEntity);
			respawnable.spawnPosition = glm::vec3(9.54f, 1.04f, 26.34f);
			respawnable.spawnRotation = glm::vec3(0.0f, 87.7f, 0.0f);

			auto& rb = registry->assign<component::RigidBody>(cleaningSolutionEntity);

			registry->assign<component::Renderable>(cleaningSolutionEntity, mr);

			auto& so2 = registry->assign<component::EntityInfo>(cleaningSolutionEntity);
			so2.name = "RedCleaningSolution 0";

			auto& cleaningSolutionCollider = registry->assign<component::Collidable>(cleaningSolutionEntity);
			auto& shapeInfo = cleaningSolutionCollider.pushShape(ColliderType::Box);
			shapeInfo.box.setSize({ 0.8f, 1.0f, 0.85f });
		}

		{
			//GLOOP
			entt::entity gloopEntity = registry->create();

			component::Transform gloopTransform;
			gloopTransform.setPosition(glm::vec3(9.61f, 2.1f, 25.89f));
			gloopTransform.setScale(glm::vec3(0.1f, 0.1f, 0.1f));
			registry->assign<component::Transform>(gloopEntity, gloopTransform);

			auto& carryableItem = registry->assign<CarryableItem>(gloopEntity);
			carryableItem.team = Team::red;
			carryableItem.type = CarryableItemType::gloop;

			auto& respawnable = registry->assign<Respawnable>(gloopEntity);
			respawnable.spawnPosition = glm::vec3(9.61f, 2.07f, 25.89f);
			respawnable.spawnRotation = glm::vec3(0.0f, 88.34f, 0.0f);

			auto& rb = registry->assign<component::RigidBody>(gloopEntity);

			registry->assign<Gloop>(gloopEntity);

			mr.model = Model::get("Gloop");
			registry->assign<component::Renderable>(gloopEntity, mr);

			auto& so2 = registry->assign<component::EntityInfo>(gloopEntity);
			so2.name = "RedGloop 0";

			auto& gloopCollider = registry->assign<component::Collidable>(gloopEntity);
			auto& shapeInfo = gloopCollider.pushShape(ColliderType::Box);
			shapeInfo.box.setSize({ 1.55f, 2.65f, 1.0f });
		}

		{
			//CANNONBALL CRATE

			entt::entity cannonballCrateEntity = registry->create();

			component::Transform cannonballCrateTransform;
			cannonballCrateTransform.setPosition(glm::vec3(-1.0f, 0.5f, -3.0f));
			cannonballCrateTransform.setRotation(glm::vec3(0.0f, -90.0f, 0.0f));
			cannonballCrateTransform.setScale(glm::vec3(2.0f, 1.0f, 1.0f));
			registry->assign<component::Transform>(cannonballCrateEntity, cannonballCrateTransform);

			auto& cannonballCrate = registry->assign<CannonballCrate>(cannonballCrateEntity);
			cannonballCrate.team = Team::red;

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
			spawner.respawnTimerDuration = 5.0f;
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
			spawner.respawnTimerDuration = 10.0f;
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
			//THROWABLE BOTTLE
			entt::entity bottleEntity = registry->create();

			component::Transform bottleTransform;
			bottleTransform.setPosition(glm::vec3(-7.04f, 0.37f, 10.14f));
			bottleTransform.setScale(glm::vec3(0.25f, 0.6f, 0.25f));
			registry->assign<component::Transform>(bottleEntity, bottleTransform);

			auto& carryableItem = registry->assign<CarryableItem>(bottleEntity);
			carryableItem.type = CarryableItemType::throwableBottle;
			carryableItem.team = Team::neutral;

			auto& respawnable = registry->assign<Respawnable>(bottleEntity);
			respawnable.spawnPosition = glm::vec3(-7.04f, 0.61f, 10.14f);
			respawnable.spawnRotation = glm::vec3(0.0f, 0.0f, 0.0f);

			auto& throwableBottle = registry->assign<ThrowableBottle>(bottleEntity);

			auto& rb = registry->assign<component::RigidBody>(bottleEntity);

			mr.model = Model::get("Gloop");
			registry->assign<component::Renderable>(bottleEntity, mr);

			auto& so2 = registry->assign<component::EntityInfo>(bottleEntity);
			so2.name = "ThrowableBottle";

			auto& gloopCollider = registry->assign<component::Collidable>(bottleEntity);
			auto& shapeInfo = gloopCollider.pushShape(ColliderType::Box);
			shapeInfo.box.setSize({ 1.0f, 1.0f, 1.0f });


			
			//THROWABLE BOTTLE SPAWNER
			//NOTE: MAKE SURE THIS IS INITIALIZED AFTER THROWABLE BOTTLE ENTITY
			entt::entity bottleSpawnerEntity = registry->create();

			auto& spawner = registry->assign<RespawnManager>(bottleSpawnerEntity);
			spawner.respawnTimerDuration = 30.0f;
			spawner.type = CarryableItemType::throwableBottle;
			spawner.team = Team::neutral;
			spawner.entityPrefab = registry->create();
			registry->stomp(spawner.entityPrefab, bottleEntity, *registry);

			auto& newTransform  = registry->get<component::Transform>(spawner.entityPrefab);
			auto& newEntityInfo = registry->get<component::EntityInfo>(spawner.entityPrefab);
			auto& newCarryable  = registry->get<CarryableItem>(spawner.entityPrefab);

			newCarryable.hasBeenCarried = true; //we have to do this or else the prefab entity will stop this spawner from ever spawning an item
			newTransform.setPosition(glm::vec3(-99999.0f));
			newEntityInfo.name = "ThrowableBottlePrefab";

			auto& spawnerTransform = registry->assign<component::Transform>(bottleSpawnerEntity);
			spawnerTransform.setPosition(glm::vec3(0.0f));

			auto& spawnerInfo = registry->assign<component::EntityInfo>(bottleSpawnerEntity);
			spawnerInfo.name  = "ThrowableBottleSpawner";
		}

		{
			//LAMP

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

			//mr.model = Model::cache("res/assets/models/character.obj");
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

        entt::entity e = registry->create();
        //registry->assign<component::Renderable>(e, mr);

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

        entt::entity e = registry->create();
        //registry->assign<component::Renderable>(e, mr);

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