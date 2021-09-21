#pragma once

#if _INSIDE_REFLY_ENGINE
	#define RF_ENGINE_EXPORT __declspec(dllexport)
#else
	#define RF_ENGINE_EXPORT __declspec(dllimport)
#endif

RF_ENGINE_EXPORT extern void SomethingElse();