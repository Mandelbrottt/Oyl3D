#pragma once

#if _INSIDE_REARM_CORE
	#define REARM_CORE_EXPORT __declspec(dllexport)
#else
	#define REARM_CORE_EXPORT __declspec(dllimport)
#endif