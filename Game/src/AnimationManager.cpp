#include "AnimationManager.h"

void AnimationManager::setAnimationEntities(oyl::Event event)
{
	auto evt = event_cast<PlayerDroppedItemEvent>(event);
	auto& playerTransform = registry->get<component::Transform>(evt.playerEntity);

	//Getting the Camera
	for (auto child : playerTransform.getChildrenEntities())
		if (registry->has<component::Camera>(child))
			playerCamera = child;

	//Getting the individual arms
	for (auto child : registry->get<component::Transform>(playerCamera).getChildrenEntities())
	{
		if (registry->get<component::EntityInfo>(child).name.find("Left") != std::string::npos)
			playerArmL = child;
		if (registry->get<component::EntityInfo>(child).name.find("Right") != std::string::npos)
			playerArmR = child;
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

}

void AnimationManager::setAnimationProperties(std::string p_tag, AnimationProperties p_type, bool p_loop)
{
	if (p_tag.find("_R") != std::string::npos)
	{
		registry->get<component::SkeletonAnimatable>(playerArmR).animation = p_tag;
		registry->get<component::SkeletonAnimatable>(playerArmR).loop = p_loop;
	}
	if (p_tag.find("_L") != std::string::npos)
	{
		registry->get<component::SkeletonAnimatable>(playerArmL).animation = p_tag;
		registry->get<component::SkeletonAnimatable>(playerArmL).loop = p_loop;
	}
	if (p_tag.find("_A") != std::string::npos)
	{
		registry->get<component::SkeletonAnimatable>(playerArmL).animation = p_tag;
		registry->get<component::SkeletonAnimatable>(playerArmR).animation = p_tag;
		registry->get<component::SkeletonAnimatable>(playerArmL).loop = p_loop;
		registry->get<component::SkeletonAnimatable>(playerArmR).loop = p_loop;
	}

	switch (p_type)
	{
	case(AnimationProperties::Cannonball):
		registry->get<component::Transform>(playerArmLObject).setPosition(glm::vec3(2.80f, -1.01f, 0.63f));
		registry->get<component::Transform>(playerArmLObject).setRotationEuler(glm::vec3(92.21f, 46.24f, -27.55f));
		registry->get<component::Transform>(playerArmLObject).setScale(glm::vec3(2.0f));
		registry->get<component::Renderable>(playerArmLObject).model = Model::get("garbageBall");
		registry->get<component::Renderable>(playerArmLObject).material = Material::get("DecorationAtlas");
		break;
	case(AnimationProperties::Gloop):
		registry->get<component::Transform>(playerArmLObject).setPosition(glm::vec3(0.31f, 1.02f, 0.40f));
		registry->get<component::Transform>(playerArmLObject).setRotationEuler(glm::vec3(62.47f, -2.50f, 70.08f));
		registry->get<component::Transform>(playerArmLObject).setScale(glm::vec3(0.5f));
		registry->get<component::Renderable>(playerArmLObject).model = Model::get("Gloop");
		break;
	case(AnimationProperties::Pirax):
		registry->get<component::Transform>(playerArmLObject).setPosition(glm::vec3(0.83f, 0.94f, -0.98f));
		registry->get<component::Transform>(playerArmLObject).setRotationEuler(glm::vec3(116.07f, -3.19f, -106.4f));
		registry->get<component::Transform>(playerArmLObject).setScale(glm::vec3(2.0f));
		registry->get<component::Renderable>(playerArmLObject).model = Model::get("Pirax");
		break;
	case(AnimationProperties::Mop):
		registry->get<component::Transform>(playerArmRObject).setPosition(glm::vec3(0.53f, 0.20f, 0.82f));
		registry->get<component::Transform>(playerArmRObject).setRotationEuler(glm::vec3(9.87f, 9.99f, -114.0f));
		registry->get<component::Transform>(playerArmRObject).setScale(glm::vec3(1.0f));
		registry->get<component::Renderable>(playerArmRObject).model = Model::get("mop");
		break;
	case(AnimationProperties::MopUse):
		registry->get<component::Transform>(playerArmRObject).setPosition(glm::vec3(-1.35f, -0.63f, 1.89f));
		registry->get<component::Transform>(playerArmRObject).setRotationEuler(glm::vec3(-130.3f, 64.95f, 79.01f));
		registry->get<component::Transform>(playerArmRObject).setScale(glm::vec3(5.0f, 3.0f, 3.0f));
		registry->get<component::Renderable>(playerArmRObject).model = Model::get("mop");
		break;
	case(AnimationProperties::ThrowableBottle):
		registry->get<component::Transform>(playerArmRObject).setPosition(glm::vec3(-0.08f, 0.86f, 0.48f));
		registry->get<component::Transform>(playerArmRObject).setRotationEuler(glm::vec3(-67.71f, 15.28f, 115.29f));
		registry->get<component::Transform>(playerArmRObject).setScale(glm::vec3(1.5f, 1.75f, 1.5f));
		registry->get<component::Renderable>(playerArmRObject).model = Model::get("ThrowBottle");
		registry->get<component::Renderable>(playerArmRObject).material = Material::get("ThrowableBottle");
		break;
	case(AnimationProperties::EmptyL):
		registry->get<component::Transform>(playerArmLObject).setPosition(glm::vec3(0.0f));
		registry->get<component::Transform>(playerArmLObject).setRotationEuler(glm::vec3(0.0f));
		registry->get<component::Transform>(playerArmLObject).setScale(glm::vec3(0.0f));
		break;
	case(AnimationProperties::EmptyR):
		registry->get<component::Transform>(playerArmRObject).setPosition(glm::vec3(0.0f));
		registry->get<component::Transform>(playerArmRObject).setRotationEuler(glm::vec3(0.0f));
		registry->get<component::Transform>(playerArmRObject).setScale(glm::vec3(0.0f));
		break;
	case(AnimationProperties::None):
		break;
	}


}

void AnimationManager::onEnter()
{
	listenForEventCategory((EventCategory)CategoryPlayer);
	listenForEventCategory((EventCategory)CategoryGarbagePile);
	listenForEventCategory((EventCategory)CategoryThrowableBottle);
	listenForEventCategory((EventCategory)CategoryCleaningSolution);
	listenForEventCategory((EventCategory)CategoryGloop);
	listenForEventCategory((EventCategory)CategoryCannonball);
	listenForEventCategory((EventCategory)CategoryCannon);
	listenForEventCategory((EventCategory)CategoryQuicktimeCleaningEvent);
	listenForEventCategory(EventCategory::Animation);
}

void AnimationManager::onExit()
{
}

void AnimationManager::onUpdate()
{
	//Need to update every frame and check which animation is playing in order to have things like bottles dissapear mid animation
	auto playerView = registry->view<Player, component::Transform>();

	if (playerView != 0)
	{
		entt::entity l_playerCamera{};
		entt::entity l_playerArmR{};
		entt::entity l_playerArmL{};
		entt::entity l_playerArmRTarget{};
		entt::entity l_playerArmRObject{};
		entt::entity l_playerArmLTarget{};
		entt::entity l_playerArmLObject{};

		for (auto& playerEntity : playerView)
		{
			auto& playerTransform = registry->get<component::Transform>(playerEntity);
			auto& playerAnimatible = registry->get<component::SkeletonAnimatable>(playerEntity);

			//Getting the Camera
			for (auto child : playerTransform.getChildrenEntities())
				if (registry->has<component::Camera>(child))
					l_playerCamera = child;

			//Getting the individual arms
			for (auto child : registry->get<component::Transform>(l_playerCamera).getChildrenEntities())
			{
				if (registry->get<component::EntityInfo>(child).name.find("Left") != std::string::npos)
					l_playerArmL = child;
				if (registry->get<component::EntityInfo>(child).name.find("Right") != std::string::npos)
					l_playerArmR = child;
			}

			//Getting each hand target
			for (auto child : registry->get<component::Transform>(l_playerArmL).getChildrenEntities())
				if (registry->has<component::BoneTarget>(child))
					l_playerArmLTarget = child;
			for (auto child : registry->get<component::Transform>(l_playerArmR).getChildrenEntities())
				if (registry->has<component::BoneTarget>(child))
					l_playerArmRTarget = child;

			//Getting each hand object
			for (auto child : registry->get<component::Transform>(l_playerArmLTarget).getChildrenEntities())
				if (registry->has<component::Renderable>(child))
					l_playerArmLObject = child;
			for (auto child : registry->get<component::Transform>(l_playerArmRTarget).getChildrenEntities())
				if (registry->has<component::Renderable>(child))
					l_playerArmRObject = child;

			if (registry->get<component::SkeletonAnimatable>(l_playerArmL).animation == "PiraxUse_L" && registry->get<component::SkeletonAnimatable>(l_playerArmL).time >= 2.10)
			{
				registry->get<component::Transform>(l_playerArmLObject).setPosition(glm::vec3(0.0f));
				registry->get<component::Transform>(l_playerArmLObject).setRotationEuler(glm::vec3(0.0f));
				registry->get<component::Transform>(l_playerArmLObject).setScale(glm::vec3(0.0f));
			}
			if (registry->get<component::SkeletonAnimatable>(l_playerArmL).animation == "GloopUse2_L" && registry->get<component::SkeletonAnimatable>(l_playerArmL).time >= 2.10)
			{
				registry->get<component::Transform>(l_playerArmLObject).setPosition(glm::vec3(0.0f));
				registry->get<component::Transform>(l_playerArmLObject).setRotationEuler(glm::vec3(0.0f));
				registry->get<component::Transform>(l_playerArmLObject).setScale(glm::vec3(0.0f));
			}

		}

	}
	else
		OYL_LOG("no players");
}

bool AnimationManager::onEvent(const Event& event)
{


	switch (event.type)
	{

	case EventType::AnimationFinished:
	{
		auto evt = event_cast<AnimationFinishedEvent>(event);

		//for player animations do player things, otherwise do first person things
		if (registry->get<component::EntityInfo>(evt.entity).name.find("Player") != std::string::npos)
		{
			auto& playerTransform = registry->get<component::Transform>(evt.entity);
		}
		if (registry->get<component::EntityInfo>(evt.entity).name.find("Arm") != std::string::npos)//TODO: else if this with other scenarios, like flies else if(viemodel){} else { do nothing }
		{

			//Animation Tag to check what action should be taken
			std::string animationTag = registry->get<component::SkeletonAnimatable>(evt.entity).animation;

			if (registry->get<component::EntityInfo>(evt.entity).name.find("L") != std::string::npos) //if left arm
			{
				if (animationTag.find("Use") != std::string::npos && animationTag.find("Gloop") == std::string::npos && evt.stopped)
				{
					registry->get<component::SkeletonAnimatable>(evt.entity).animation = "Idle_L";
					registry->get<component::SkeletonAnimatable>(evt.entity).play = true;
					registry->get<component::SkeletonAnimatable>(evt.entity).loop = true;
				}
				if (animationTag.find("Use") != std::string::npos && animationTag.find("Gloop") != std::string::npos && evt.stopped)
				{
					if (gloopLast)
					{
						registry->get<component::SkeletonAnimatable>(evt.entity).animation = "Idle_L";
						registry->get<component::SkeletonAnimatable>(evt.entity).play = true;
						registry->get<component::SkeletonAnimatable>(evt.entity).loop = true;
					}
					else if (!gloopLast)
					{
						registry->get<component::SkeletonAnimatable>(evt.entity).animation = "Gloop_L";
						registry->get<component::SkeletonAnimatable>(evt.entity).play = true;
						registry->get<component::SkeletonAnimatable>(evt.entity).loop = true;
					}
				}
				if (animationTag.find("Pushing_A") != std::string::npos && evt.stopped)
				{
					registry->get<component::SkeletonAnimatable>(evt.entity).animation = "Idle_L";
					registry->get<component::SkeletonAnimatable>(evt.entity).play = true;
					registry->get<component::SkeletonAnimatable>(evt.entity).loop = true;
				}
				if (animationTag.find("MopUse_A") != std::string::npos && evt.stopped)
				{

				}

			}
			else if (registry->get<component::EntityInfo>(evt.entity).name.find("R") != std::string::npos) //if right arm
			{

				if (animationTag.find("Use") != std::string::npos && evt.stopped)
				{
					registry->get<component::SkeletonAnimatable>(evt.entity).animation = "Idle_R";
					registry->get<component::SkeletonAnimatable>(evt.entity).play = true;
					registry->get<component::SkeletonAnimatable>(evt.entity).loop = true;
				}
				if (animationTag.find("Pushing_A") != std::string::npos && evt.stopped)
				{
					registry->get<component::SkeletonAnimatable>(evt.entity).animation = "Idle_R";
					registry->get<component::SkeletonAnimatable>(evt.entity).play = true;
					registry->get<component::SkeletonAnimatable>(evt.entity).loop = true;
				}
				if (animationTag.find("MopUse_A") != std::string::npos && evt.stopped)
				{

				}
			}
			else
				OYL_LOG("Could not find any arm");
		}
		break;
	}
	//////////////////////////// I T E M      B A S E D     E V E N T S ///////////////////////////////////
	//unique case for spawning cannonballs

	case (EventType)TypeSpawnCannonballForPlayer:
	{
		//set the entities
		setAnimationEntities(event);
		//set the animation
		setAnimationProperties("Cannonball_A", AnimationProperties::Cannonball, true);
		break;
	}
	//Player Picked up item
	case (EventType)TypePlayerPickedUpItem:
	{
		auto evt = event_cast<PlayerPickedUpItemEvent>(event);

		//set the entities
		setAnimationEntities(event);

		switch (evt.itemType)
		{
		case CarryableItemType::cleaningSolution:
			setAnimationProperties("Pirax_L", AnimationProperties::Pirax, true);

			if (evt.itemTeam == Team::blue)
				registry->get<component::Renderable>(playerArmLObject).material = Material::get("PiraxBlue");
			else if (evt.itemTeam == Team::red)
				registry->get<component::Renderable>(playerArmLObject).material = Material::get("PiraxRed");
			break;

		case CarryableItemType::gloop:
			setAnimationProperties("Gloop_L", AnimationProperties::Gloop, true);

			if (evt.itemTeam == Team::blue)
				registry->get<component::Renderable>(playerArmLObject).material = Material::get("GoopBlue");
			else if (evt.itemTeam == Team::red)
				registry->get<component::Renderable>(playerArmLObject).material = Material::get("GloopRed");
			break;

		case CarryableItemType::cannonball:
			setAnimationProperties("Cannonball_A", AnimationProperties::Cannonball, true);
			break;

		case CarryableItemType::mop:
			//Set the animation here for picking up mop
			setAnimationProperties("Mop_R", AnimationProperties::Mop, true);

			if (evt.itemTeam == Team::blue)
				registry->get<component::Renderable>(playerArmRObject).material = Material::get("MopBlue");
			else if (evt.itemTeam == Team::red)
				registry->get<component::Renderable>(playerArmRObject).material = Material::get("MopRed");
			break;

		case CarryableItemType::throwableBottle:
			setAnimationProperties("Throwable_R", AnimationProperties::ThrowableBottle, true);
			break;
		}
		break;
	}

	//cases for item drops
	case (EventType)TypePlayerDroppedItem:
	{
		auto evt = event_cast<PlayerDroppedItemEvent>(event);

		setAnimationEntities(event);

		switch (evt.itemClassificationToDrop)
		{
		case PlayerItemClassification::primary:
			//reset right arm components
			setAnimationProperties("Idle_R", AnimationProperties::EmptyR, true);
			break;
		case PlayerItemClassification::secondary:
			//reset left arm components
			setAnimationProperties("Idle_L", AnimationProperties::EmptyL, true);
			break;
		case PlayerItemClassification::any:
			//reset both arms components
			setAnimationProperties("Idle_R", AnimationProperties::EmptyR, true);
			setAnimationProperties("Idle_L", AnimationProperties::EmptyL, true);
			break;
		}
		break;
	}

	//cases for item uses
	case (EventType)TypeCleaningSolutionUsed:
	{

		setAnimationEntities(event);

		//reset left arm components TODO: MAKE THIS THE ACTUAL USE ANIMATION AND THEN LINK IT BACK TO THE IDLE OR WHATEVER THIS IS TEMP FOR THE EXPO
		registry->get<component::SkeletonAnimatable>(playerArmL).time = 0.01f;
		setAnimationProperties("PiraxUse_L", AnimationProperties::None, false);

		break;
	}

	case (EventType)TypeGloopUsed:
	{
		setAnimationEntities(event);
		auto evt = event_cast<GloopedUsedEvent>(event);
		//set this so that it can be accessed by the rest of the class
		gloopLast = evt.isLastUse;
		if (evt.isLastUse)
		{
			registry->get<component::SkeletonAnimatable>(playerArmL).time = 0.01f;
			setAnimationProperties("GloopUse2_L", AnimationProperties::None, false);
		}
		else
		{
			registry->get<component::SkeletonAnimatable>(playerArmL).time = 0.01f;
			setAnimationProperties("GloopUse_L", AnimationProperties::None, false);
		}

		break;
	}

	case (EventType)TypeCannonLoaded:
	{
		setAnimationEntities(event);

		//reset left arm components TODO: MAKE THIS THE ACTUAL USE ANIMATION AND THEN LINK IT BACK TO THE IDLE OR WHATEVER THIS IS TEMP FOR THE EXPO
		setAnimationProperties("Idle_L", AnimationProperties::EmptyL, true);
		setAnimationProperties("Idle_R", AnimationProperties::EmptyR, true);

		break;
	}
	case (EventType)TypeThrowBottle:
	{
		setAnimationEntities(event);

		//reset left arm components TODO: MAKE THIS THE ACTUAL USE ANIMATION AND THEN LINK IT BACK TO THE IDLE OR WHATEVER THIS IS TEMP FOR THE EXPO
		registry->get<component::SkeletonAnimatable>(playerArmR).time = 0.01f;
		setAnimationProperties("ThrowableUse_R", AnimationProperties::EmptyR, false);

		break;
	}

	//////////////////////////// P L A Y E R    B A S E D     E V E N T S ///////////////////////////////////
		//cases for item uses
	case (EventType)TypePlayerStateChanged:
	{
		setAnimationEntities(event);
		auto evt = event_cast<PlayerStateChangedEvent>(event);
		auto& playerTransform = registry->get<component::Transform>(evt.playerEntity);
		auto& playerAnimatable = registry->get<component::SkeletonAnimatable>(evt.playerEntity);

		bool changeImmediate = true;

		if (evt.oldState == PlayerState::walking || evt.oldState == PlayerState::idle)
			changeImmediate = true;
		else
			changeImmediate = false;


		switch (evt.newState)
		{
		case PlayerState::cleaning:
			//Set the animation here for using cleaning solution
			playerAnimatable.animation = "cleaning";
			break;
		case PlayerState::idle:
			//Set the animation to idle
			playerAnimatable.animation = "idle1"; //TODO: fix this up with the proper idle
			break;
		case PlayerState::jumping:
			//Set the animation here for using cannonball
			playerAnimatable.animation = "jump";
			break;
		case PlayerState::walking:
			if (changeImmediate)
			playerAnimatable.animation = "running";
			break;
		case PlayerState::pushing:
			//Set the animation here for using throwable bottle
			registry->get<component::SkeletonAnimatable>(playerArmR).time = 0.01f;
			registry->get<component::SkeletonAnimatable>(playerArmL).time = 0.01f;
			setAnimationProperties("Pushing_A", AnimationProperties::None, false);

			playerAnimatable.animation = "pushing canon";

			break;
		case PlayerState::stunned:
			//Set the animation here for using throwable bottle
			playerAnimatable.animation = "stunned";
			break;
		case PlayerState::inCleaningQuicktimeEvent:
			//Set the animation here for using mop
			registry->get<component::SkeletonAnimatable>(playerArmL).time = 0.01;
			registry->get<component::SkeletonAnimatable>(playerArmR).time = 0.01;

			registry->get<component::SkeletonAnimatable>(playerArmL).play = false;
			registry->get<component::SkeletonAnimatable>(playerArmR).play = false;

			setAnimationProperties("MopUse_A", AnimationProperties::MopUse, false);

			playerAnimatable.animation = "cleaning";
			break;

		}
		break;
	}
	case(EventType)TypeStickMovedDuringQuicktimeCleaningEvent:
	{
		auto evt = event_cast<StickMovedDuringQuicktimeCleaningEventEvent>(event);

		setAnimationEntities(event);
		registry->get<component::SkeletonAnimatable>(playerArmL).time = (1.01 - evt.stickPosY);
		registry->get<component::SkeletonAnimatable>(playerArmR).time = (1.01 - evt.stickPosY);
	}

	}
	return false;
}
