#pragma once

#if _INSIDE_REARM_EDITOR
	#define R_EDITOR_EXPORT __declspec(dllexport)
#else
	#define R_EDITOR_EXPORT __declspec(dllimport)
#endif