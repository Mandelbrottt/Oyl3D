#pragma once

#if _INSIDE_REFLY_EDITOR
	#define RF_EDITOR_EXPORT __declspec(dllexport)
#else
	#define RF_EDITOR_EXPORT __declspec(dllimport)
#endif

RF_EDITOR_EXPORT extern void Something();