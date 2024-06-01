#pragma once

#ifdef _INSIDE_REARM_CORE
	#define REARM_CORE_EXPORT __declspec(dllexport)
#else
	#define REARM_CORE_EXPORT __declspec(dllimport)
#endif

#define UNUSED(_var_) (void) _var_
