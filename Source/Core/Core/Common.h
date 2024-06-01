#pragma once

#ifdef _INSIDE_REARM_CORE
	#define R_CORE_EXPORT __declspec(dllexport)
#else
	#define R_CORE_EXPORT __declspec(dllimport)
#endif

#define R_UNUSED(_var_) (void) _var_
