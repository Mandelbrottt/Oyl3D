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
			if (registry->get<component::EntityInfo>(child).name.find("Left"))
				playerArmL = child;
			else if (registry->get<component::EntityInfo>(child).name.find("Right"))
				playerArmR = child;

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
			//if(blah blah component item team = teamEnum::blue)
			registry->get<component::Renderable>(playerArmLObject).material = Material::get("PiraxBlue");
			//else if(blah blah component item team = teamEnum::red)
			//registry->get<component::Renderable>(playerArmLObject).material = Material::get("PiraxRed");

			//Set the animation here for picking up cleaning solution
			break;
		case CarryableItemType::gloop:
			//Set the animation here for picking up gloop
			break;
		case CarryableItemType::cannonball:
			//Set the animation here for picking up cannonball
			break;
		case CarryableItemType::mop:
			//Set the animation here for picking up mop
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
		OYL_LOG("dropped the thing");
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
			if (registry->get<component::EntityInfo>(child).name.find("Left"))
				playerArmL = child;
			else if (registry->get<component::EntityInfo>(child).name.find("Right"))
				playerArmR = child;

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
			registry->get<component::Transform>(playerArmRObject).setScale(glm::vec3(1.0f));
			registry->get<component::Renderable>(playerArmRObject).model = Model::get("None");
			registry->get<component::Renderable>(playerArmRObject).material = Material::get("None");
			break;
		case PlayerItemClassification::secondary:
			//reset left arm components
			registry->get<component::SkeletonAnimatable>(playerArmL).animation = "Idle_L";
			registry->get<component::Transform>(playerArmLObject).setPosition(glm::vec3(0.0f));
			registry->get<component::Transform>(playerArmLObject).setRotationEuler(glm::vec3(0.0f));
			registry->get<component::Transform>(playerArmLObject).setScale(glm::vec3(1.0f));
			registry->get<component::Renderable>(playerArmLObject).model = Model::get("None");
			registry->get<component::Renderable>(playerArmLObject).material = Material::get("None");
			OYL_LOG("dropped the thing");
			break;
		case PlayerItemClassification::any:
			//reset both arms components
			registry->get<component::SkeletonAnimatable>(playerArmR).animation = "Idle_R";
			registry->get<component::Transform>(playerArmRObject).setPosition(glm::vec3(0.0f));
			registry->get<component::Transform>(playerArmRObject).setRotationEuler(glm::vec3(0.0f));
			registry->get<component::Transform>(playerArmRObject).setScale(glm::vec3(1.0f));
			registry->get<component::Renderable>(playerArmRObject).model = Model::get("None");
			registry->get<component::Renderable>(playerArmRObject).material = Material::get("None");

			registry->get<component::SkeletonAnimatable>(playerArmL).animation = "Idle_L";
			registry->get<component::Transform>(playerArmLObject).setPosition(glm::vec3(0.0f));
			registry->get<component::Transform>(playerArmLObject).setRotationEuler(glm::vec3(0.0f));
			registry->get<component::Transform>(playerArmLObject).setScale(glm::vec3(1.0f));
			registry->get<component::Renderable>(playerArmLObject).model = Model::get("None");
			registry->get<component::Renderable>(playerArmLObject).material = Material::get("None");
			break;
		}
		break;
	}

	//cases for item uses
	//case (EventType)TypePlayerInteractResult:
	//{

	//	auto evt = event_cast<PlayerInteractResultEvent>(event);
	//	switch (evt.interactionType)
	//	{
	//	case:
	//		//Set the animation here for using cleaning solution
	//		break;
	//	case PlayerInteractResultEvent::gloop:
	//		//Set the animation here for using gloop
	//		break;
	//	case PlayerInteractResultEvent::cannonball:
	//		//Set the animation here for using cannonball
	//		break;
	//	case PlayerInteractResultEvent::mop:
	//		//Set the animation here for using mop
	//		break;
	//	case PlayerInteractResultEvent::throwableBottle:
	//		//Set the animation here for using throwable bottle
	//		break;
	//	}
	//	break;
	//}

	//////////////////////////// P L A Y E R    B A S E D     E V E N T S ///////////////////////////////////
		//cases for item uses
	//case (EventType)TypePlayerStateChanged:
	//{

	//	auto evt = event_cast<PlayerStateChangedEvent>(event);
	//	switch (evt.)
	//	{
	//	case PlayerState::cleaning:
	//		//Set the animation here for using cleaning solution
	//		break;
	//	case PlayerState::idle:
	//		//Set the animation here for using gloop
	//		break;
	//	case PlayerState::cleaning:
	//		//Set the animation here for using cannonball
	//		break;
	//	case CarryableItemType::mop:
	//		//Set the animation here for using mop
	//		break;
	//	case CarryableItemType::throwableBottle:
	//		//Set the animation here for using throwable bottle
	//		break;
	//	}
	//	break;
	//}

	}
	return false;
}
