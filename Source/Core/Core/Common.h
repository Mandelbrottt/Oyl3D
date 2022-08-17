#pragma once

#if _INSIDE_REFLY_CORE
	#define REFLY_CORE_EXPORT __declspec(dllexport)
#else
	#define REFLY_CORE_EXPORT __declspec(dllimport)
#endif