#pragma once

#ifdef _INSIDE_REARM_RENDERING
	#define REARM_RENDERING_EXPORT __declspec(dllexport)
#else
	#define REARM_RENDERING_EXPORT __declspec(dllimport)
#endif
