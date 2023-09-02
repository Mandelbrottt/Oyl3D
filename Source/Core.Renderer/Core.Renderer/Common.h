#pragma once

#ifdef _INSIDE_REARM_CORE_RENDERER
	#define REARM_CORE_RENDERER_EXPORT __declspec(dllexport)
#else
	#define REARM_CORE_RENDERER_EXPORT __declspec(dllimport)
#endif
