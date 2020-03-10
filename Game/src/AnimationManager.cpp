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

		auto& playerTransform = registry->get<component::Transform> (evt.playerEntity);
		std::vector<entt::entity> playerChildren = playerTransform.getChildrenEntities();
		for (auto child : playerChildren)
			OYL_LOG("there is a child");


		switch (evt.itemType)
		{
		case CarryableItemType::cleaningSolution:
			playerTransform.setScaleX(2);
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

		auto evt = event_cast<PlayerDropItemRequestEvent>(event);
		switch (evt.itemClassificationToDrop)
		{
		case PlayerItemClassification::primary:
			//Set the animation here for dropping cleaning solution
			break;
		case PlayerItemClassification::secondary:
			//Set the animation here for dropping gloop
			break;
		case PlayerItemClassification::any:
			//Set the animation here for dropping cannonball
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
