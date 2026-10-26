#pragma once

#include <Core/Preprocessor/Common.h>

//---- Define attributes of all API symbols declarations, e.g. for DLL under Windows
// Using Dear ImGui via a shared library is not recommended, because of function call overhead and because we don't guarantee backward nor forward ABI compatibility.
// - Windows DLL users: heaps and globals are not shared across DLL boundaries! You will need to call SetCurrentContext() + SetAllocatorFunctions()
//   for each static/DLL boundary you are calling from. Read "Context and Memory Allocators" section of imgui.cpp for more details.
#if defined(OYL_PACKAGE_AVAILABLE_IMGUI) && defined(IMGUI_SHAREDLIB)
#	if defined(OYL_PACKAGE_WITHIN_IMGUI)
#		define IMGUI_API __OYL__EXPORT_ATTR
#	else
#		define IMGUI_API __OYL__IMPORT_ATTR
#	endif
#endif