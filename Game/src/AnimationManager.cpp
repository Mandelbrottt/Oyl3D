#include "AnimationManager.h"

void AnimationManager::onEnter()
{
	listenForEventCategory((EventCategory)CategoryPlayer);
	listenForEventCategory((EventCategory)CategoryGarbagePile);
	listenForEventCategory((EventCategory)CategoryThrowableBottle);
	listenForEventCategory((EventCategory)CategoryCleaningSolution);
	listenForEventCategory((EventCategory)CategoryGloop);
}

void AnimationManager::onExit()
{
}

void AnimationManager::onUpdate()
{
	//dunno what I need to update here. probably something. TODO: figure out that thing
}

bool AnimationManager::onEvent(const Event& event)
{
	

	switch (event.type)
	{

		//////////////////////////// I T E M      B A S E D     E V E N T S ///////////////////////////////////
		//cases for item pickups
	case (EventType)TypePlayerPickedUpItem:
	{
		auto evt = event_cast<PlayerPickedUpItemEvent>(event);

		auto& playerTransform = registry->get<component::Transform>(evt.playerEntity);

		//Objects that need to be modified
		entt::entity playerCamera{};
		entt::entity playerArmR{};
		entt::entity playerArmL{};
		entt::entity playerArmRTarget{};
		entt::entity playerArmRObject{};
		entt::entity playerArmLTarget{};
		entt::entity playerArmLObject{};

		//Getting the Camera
		for (auto child : playerTransform.getChildrenEntities())
			if (registry->has<component::Camera>(child))
				playerCamera = child;

		//Getting the individual arms
		for (auto child : registry->get<component::Transform>(playerCamera).getChildrenEntities())
		{
			if (registry->get<component::EntityInfo>(child).name.find("Left") != std::string::npos)
			{
				playerArmL = child;
				OYL_LOG("found left arm");
			}
			if (registry->get<component::EntityInfo>(child).name.find("Right") != std::string::npos)
			{
				playerArmR = child;
				OYL_LOG("found right arm");
			}
		}

		//Getting each hand target
		for (auto child : registry->get<component::Transform>(playerArmL).getChildrenEntities())
			if (registry->has<component::BoneTarget>(child))
				playerArmLTarget = child;
		for (auto child : registry->get<component::Transform>(playerArmR).getChildrenEntities())
			if (registry->has<component::BoneTarget>(child))
				playerArmRTarget = child;

		//Getting each hand object
		for (auto child : registry->get<component::Transform>(playerArmLTarget).getChildrenEntities())
			if (registry->has<component::Renderable>(child))
				playerArmLObject = child;
		for (auto child : registry->get<component::Transform>(playerArmRTarget).getChildrenEntities())
			if (registry->has<component::Renderable>(child))
				playerArmRObject = child;

		switch (evt.itemType)
		{
		case CarryableItemType::cleaningSolution:
			registry->get<component::SkeletonAnimatable>(playerArmL).animation = "Pirax_L";
			registry->get<component::Transform>(playerArmLObject).setPosition(glm::vec3(0.83f, 0.94f, -0.98f));
			registry->get<component::Transform>(playerArmLObject).setRotationEuler(glm::vec3(116.07f, -3.19f, -106.4f));
			registry->get<component::Transform>(playerArmLObject).setScale(glm::vec3(2.0f));
			registry->get<component::Renderable>(playerArmLObject).model = Model::get("Pirax");
			if(evt.itemTeam == Team::blue)
				registry->get<component::Renderable>(playerArmLObject).material = Material::get("PiraxBlue");
			else if(evt.itemTeam == Team::red)
			registry->get<component::Renderable>(playerArmLObject).material = Material::get("PiraxRed");

			//Set the animation here for picking up cleaning solution
			break;
		case CarryableItemType::gloop:
			//Set the animation here for picking up gloop
			registry->get<component::SkeletonAnimatable>(playerArmL).animation = "Gloop_L";
			registry->get<component::Transform>(playerArmLObject).setPosition(glm::vec3(0.31f, 1.02f, 0.40f));
			registry->get<component::Transform>(playerArmLObject).setRotationEuler(glm::vec3(62.47f, -2.50f, 70.08f));
			registry->get<component::Transform>(playerArmLObject).setScale(glm::vec3(0.5f));
			registry->get<component::Renderable>(playerArmLObject).model = Model::get("Gloop");
			if (evt.itemTeam == Team::blue)
				registry->get<component::Renderable>(playerArmLObject).material = Material::get("GoopBlue");
			else if (evt.itemTeam == Team::red)
				registry->get<component::Renderable>(playerArmLObject).material = Material::get("GloopRed");
			break;
		case CarryableItemType::cannonball:
			//Set the animation here for picking up cannonball
			break;
		case CarryableItemType::mop:
			//Set the animation here for picking up mop
			registry->get<component::SkeletonAnimatable>(playerArmR).animation = "Reference";
			break;
		case CarryableItemType::throwableBottle:
			//Set the animation here for picking up throwable bottle
			break;
		}
		break;
	}

	//cases for item drops
	case (EventType)TypePlayerDroppedItem:
	{
		auto evt = event_cast<PlayerDroppedItemEvent>(event);
		auto& playerTransform = registry->get<component::Transform>(evt.playerEntity);

		//Objects that need to be modified
		entt::entity playerCamera{};
		entt::entity playerArmR{};
		entt::entity playerArmL{};
		entt::entity playerArmRTarget{};
		entt::entity playerArmRObject{};
		entt::entity playerArmLTarget{};
		entt::entity playerArmLObject{};

		//Getting the Camera
		for (auto child : playerTransform.getChildrenEntities())
			if (registry->has<component::Camera>(child))
				playerCamera = child;

		//Getting the individual arms
		for (auto child : registry->get<component::Transform>(playerCamera).getChildrenEntities())
		{
			if (registry->get<component::EntityInfo>(child).name.find("Left") != std::string::npos)
			{
				playerArmL = child;
				OYL_LOG("found left arm");
			}
			if (registry->get<component::EntityInfo>(child).name.find("Right") != std::string::npos)
			{
				playerArmR = child;
				OYL_LOG("found right arm");
			}
		}

		//Getting each hand target
		for (auto child : registry->get<component::Transform>(playerArmL).getChildrenEntities())
			if (registry->has<component::BoneTarget>(child))
				playerArmLTarget = child;
		for (auto child : registry->get<component::Transform>(playerArmR).getChildrenEntities())
			if (registry->has<component::BoneTarget>(child))
				playerArmRTarget = child;

		//Getting each hand object
		for (auto child : registry->get<component::Transform>(playerArmLTarget).getChildrenEntities())
			if (registry->has<component::Renderable>(child))
				playerArmLObject = child;
		for (auto child : registry->get<component::Transform>(playerArmRTarget).getChildrenEntities())
			if (registry->has<component::Renderable>(child))
				playerArmRObject = child;

		switch (evt.itemClassificationToDrop)
		{
		case PlayerItemClassification::primary:
			//reset right arm components
			registry->get<component::SkeletonAnimatable>(playerArmR).animation = "Idle_R";
			registry->get<component::Transform>(playerArmRObject).setPosition(glm::vec3(0.0f));
			registry->get<component::Transform>(playerArmRObject).setRotationEuler(glm::vec3(0.0f));
			registry->get<component::Transform>(playerArmRObject).setScale(glm::vec3(0.0f));
			break;
		case PlayerItemClassification::secondary:
			//reset left arm components
			registry->get<component::SkeletonAnimatable>(playerArmL).animation = "Idle_L";
			registry->get<component::Transform>(playerArmLObject).setPosition(glm::vec3(0.0f));
			registry->get<component::Transform>(playerArmLObject).setRotationEuler(glm::vec3(0.0f));
			registry->get<component::Transform>(playerArmLObject).setScale(glm::vec3(0.0f));
			break;
		case PlayerItemClassification::any:
			//reset both arms components
			registry->get<component::SkeletonAnimatable>(playerArmR).animation = "Idle_R";
			registry->get<component::Transform>(playerArmRObject).setPosition(glm::vec3(0.0f));
			registry->get<component::Transform>(playerArmRObject).setRotationEuler(glm::vec3(0.0f));
			registry->get<component::Transform>(playerArmRObject).setScale(glm::vec3(0.0f));

			registry->get<component::SkeletonAnimatable>(playerArmL).animation = "Idle_L";
			registry->get<component::Transform>(playerArmLObject).setPosition(glm::vec3(0.0f));
			registry->get<component::Transform>(playerArmLObject).setRotationEuler(glm::vec3(0.0f));
			registry->get<component::Transform>(playerArmLObject).setScale(glm::vec3(0.0f));
			break;
		}
		break;
	}

	//cases for item uses
	case (EventType)TypeCleaningSolutionUsed:
	{
		OYL_LOG("INSIDE THE CLEANING SOLUTION USE");
		auto evt = event_cast<CleaningSolutionUsedEvent>(event);
		auto& playerTransform = registry->get<component::Transform>(evt.playerEntity);

		//Objects that need to be modified
		entt::entity playerCamera{};
		entt::entity playerArmL{};
		entt::entity playerArmLTarget{};
		entt::entity playerArmLObject{};

		//Getting the Camera
		for (auto child : playerTransform.getChildrenEntities())
			if (registry->has<component::Camera>(child))
				playerCamera = child;

		//Getting the individual arms
		for (auto child : registry->get<component::Transform>(playerCamera).getChildrenEntities())
		{
			if (registry->get<component::EntityInfo>(child).name.find("Left") != std::string::npos)
			{
				playerArmL = child;
				OYL_LOG("found left arm");
			}
		}

		//Getting each hand target
		for (auto child : registry->get<component::Transform>(playerArmL).getChildrenEntities())
			if (registry->has<component::BoneTarget>(child))
				playerArmLTarget = child;

		//Getting each hand object
		for (auto child : registry->get<component::Transform>(playerArmLTarget).getChildrenEntities())
			if (registry->has<component::Renderable>(child))
				playerArmLObject = child;

		//reset left arm components TODO: MAKE THIS THE ACTUAL USE ANIMATION AND THEN LINK IT BACK TO THE IDLE OR WHATEVER THIS IS TEMP FOR THE EXPO
		registry->get<component::SkeletonAnimatable>(playerArmL).animation = "Idle_L";
		registry->get<component::Transform>(playerArmLObject).setPosition(glm::vec3(0.0f));
		registry->get<component::Transform>(playerArmLObject).setRotationEuler(glm::vec3(0.0f));
		registry->get<component::Transform>(playerArmLObject).setScale(glm::vec3(0.0f));

		break;
	}

	//////////////////////////// P L A Y E R    B A S E D     E V E N T S ///////////////////////////////////
		//cases for item uses
	case (EventType)TypePlayerStateChanged:
	{

		auto evt = event_cast<PlayerStateChangedEvent>(event);
		auto& playerTransform = registry->get<component::Transform>(evt.playerEntity);
		auto& playerAnimatable = registry->get<component::SkeletonAnimatable>(evt.playerEntity);



		switch (evt.newState)
		{
		case PlayerState::cleaning:
			//Set the animation here for using cleaning solution
			break;
		case PlayerState::idle:
			//Set the animation to idle
			playerAnimatable.animation = "Reference Pose"; //TODO: fix this up with the proper idle
			break;
		case PlayerState::jumping:
			//Set the animation here for using cannonball
			break;
		case PlayerState::walking:
			//Set the animation to moving
			playerAnimatable.animation = "run";
			break;
		case PlayerState::pushing:
			//Set the animation here for using throwable bottle
			break;
		case PlayerState::stunned:
			//Set the animation here for using throwable bottle
			break;
		case PlayerState::inCleaningQuicktimeEvent:
			//Set the animation here for using throwable bottle
			playerAnimatable.animation = "InCleaningQuicktimeEvent";
			break;
		}
		break;
	}

	}
	return false;
}
