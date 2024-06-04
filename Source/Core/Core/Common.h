#pragma once

#ifdef _INSIDE_OYL_CORE
	#define OYL_CORE_EXPORT __declspec(dllexport)
#else
	#define OYL_CORE_EXPORT __declspec(dllimport)
#endif

#define OYL_UNUSED(_var_) (void) _var_
