#pragma once

#ifdef REFLY_EDITOR
	#define RF_EDITOR_EXPORT __declspec(dllexport)
#else
	#define RF_EDITOR_EXPORT __declspec(dllimport)
#endif

RF_EDITOR_EXPORT extern void Something();