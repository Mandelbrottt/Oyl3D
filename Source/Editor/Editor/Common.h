#pragma once

#if _INSIDE_OYL_EDITOR
	#define OYL_EDITOR_EXPORT __declspec(dllexport)
#else
	#define OYL_EDITOR_EXPORT __declspec(dllimport)
#endif