#pragma once

#if _INSIDE_OYL_EDITOR
	#define OYL_EDITOR_API __declspec(dllexport)
#else
	#define OYL_EDITOR_API __declspec(dllimport)
#endif