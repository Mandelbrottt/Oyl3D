#pragma once

#if _INSIDE_REARM_EDITOR
	#define REARM_EDITOR_EXPORT __declspec(dllexport)
#else
	#define REARM_EDITOR_EXPORT __declspec(dllimport)
#endif