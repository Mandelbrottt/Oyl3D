#pragma once

#ifdef _INSIDE_REARM_CORE
	#define REARM_CORE_EXPORT export __declspec(dllexport)
#else
	#define REARM_CORE_EXPORT export __declspec(dllimport)
#endif
