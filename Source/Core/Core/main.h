#pragma once

#if _INSIDE_REFLY_CORE
	#define REFLY_ENGINE_EXPORT __declspec(dllexport)
#else
	#define REFLY_ENGINE_EXPORT __declspec(dllimport)
#endif

REFLY_ENGINE_EXPORT extern void SomethingElse();