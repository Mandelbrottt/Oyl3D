#pragma once

#if _INSIDE_REFLY_EDITOR
	#define REFLY_EDITOR_EXPORT __declspec(dllexport)
#else
	#define REFLY_EDITOR_EXPORT __declspec(dllimport)
#endif

REFLY_EDITOR_EXPORT extern void Something();