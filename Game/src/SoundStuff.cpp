#include "SoundStuff.h"
#include "CustomEvents.h"
#include "Tutorial.h"

using namespace oyl;

void SoundLayer::onEnter()
{
	listenForAllEvents();

	//Audio::loadBank("res/assets/sounds/Master.bank"); // not sure we want music in anyways so what the hell
	//
	//Audio::loadEvent("Washbucklers Theme 1 Main");
	//Audio::playEvent("Washbucklers Theme 1 Main");
	
	Audio::playSound("res/assets/sounds/wavesonshore.mp3", 0.2f);
}

void SoundLayer::onUpdate() {}

bool SoundLayer::onEvent(const oyl::Event& event)
{
	switch (event.type)
	{
		case (EventType) TypeCannonFired:
		{
			Audio::playSound("res/assets/sounds/CannonShot.mp3", 0.1f);
			break;
		}

		case (EventType)TypeThrowBottle:
		{
			Audio::playSound("res/assets/sounds/ThrowBottle.mp3", 0.5f);
			break;
		}

		case (EventType)TypeSpawnCannonballForPlayer:
		{
			Audio::playSound("res/assets/sounds/Huh.mp3", 0.7f);
			break;
		}

		case (EventType)TypeUseGloopRequest:
		{
			if (rand() % 2)
				Audio::playSound("res/assets/sounds/Gloop1.mp3", 0.4f);
			else
				Audio::playSound("res/assets/sounds/Gloop2.mp3", 0.4f);
			break;
		}

		case (EventType)TypeCleaningSolutionUsed:
		{
			Audio::playSound("res/assets/sounds/Detergent Sound + Bottle Throw.mp3", 0.7f);
			break;
		}
		
		case (EventType)TypeCleanedWithMop:
		{
			Audio::playSound("res/assets/sounds/GarbageCleaning.mp3", 0.5f);
			break;
		}
		
		case (EventType)TypeThrowableBottleHit:
		{
			Audio::playSound("res/assets/sounds/GlassBottleBreak.mp3", 0.7f);
			break;
		}
		
		case (EventType)TypeGameEnd:
		{
			int v1 = (rand() % 3 + 0);
			if (v1 == 0)
				Audio::playSound("res/assets/sounds/Deck.mp3", 0.5f);
			else if (v1 == 1)
				Audio::playSound("res/assets/sounds/Marooned.mp3", 0.5f);
			else
				Audio::playSound("res/assets/sounds/Waved.mp3", 0.5f);
			break;
		}

		// Tutorial Stuff
		//case (EventType)intro:
		//{
		//	Audio::playSound("res/assets/sounds/GlassBottleBreak.mp3", 0.7f);
		//	break;
		//}
		//
		//case (EventType)segment1:
		//{
		//	Audio::playSound("res/assets/sounds/GlassBottleBreak.mp3", 0.7f);
		//	break;
		//}
		//
		//case (EventType)segment2:
		//{
		//	Audio::playSound("res/assets/sounds/GlassBottleBreak.mp3", 0.7f);
		//	break;
		//}
		//
		//case (EventType)segment3:
		//{
		//	Audio::playSound("res/assets/sounds/GlassBottleBreak.mp3", 0.7f);
		//	break;
		//}
		//
		//case (EventType)segment4:
		//{
		//	Audio::playSound("res/assets/sounds/GlassBottleBreak.mp3", 0.7f);
		//	break;
		//}
		//
		//case (EventType)segment5:
		//{
		//	Audio::playSound("res/assets/sounds/GlassBottleBreak.mp3", 0.7f);
		//	break;
		//}
		//
		//case (EventType)segment6:
		//{
		//	Audio::playSound("res/assets/sounds/GlassBottleBreak.mp3", 0.7f);
		//	break;
		//}
		//
		//case (EventType)outro:
		//{
		//	Audio::playSound("res/assets/sounds/GlassBottleBreak.mp3", 0.7f);
		//	break;
		//}

		//case EventType::KeyPressed:
		//{
		//	auto e = event_cast<KeyPressedEvent>(event);
		//
		//	if (e.keycode == Key::Space && e.repeatCount == 0)
		//		Audio::playSound("res/assets/sounds/Huh.mp3", 0.07f);
		//	break;
		//}
	}
	return false;
}