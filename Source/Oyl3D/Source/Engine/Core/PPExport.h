#pragma once

#if defined(_WIN32)
#	define OYL_WINDOWS 1
#	if defined(_WIN64)
#		define OYL_WIN64 1
#	else
#		define OYL_WIN32 1
#	endif
#
#	define NOMINMAX
#else
#
#endif

#if defined(_MSC_VER) || defined(__MINGW32__) || defined(__MINGW64__)
#	define __OYL_EXPORT_ATTR __declspec(dllexport)
#	define __OYL_IMPORT_ATTR __declspec(dllimport)
#else
#	define __OYL_EXPORT_ATTR __attribute__((visibility("default")))
#	define __OYL_IMPORT_ATTR __attribute__((visibility("default")))
#endif

#if !defined(CORE_EXPORT)
#	if defined(CORE_SHAREDLIB)
#		if defined(__MODULE_CORE)
#			define CORE_EXPORT __OYL_EXPORT_ATTR
#		else
#			define CORE_EXPORT __OYL_IMPORT_ATTR
#		endif
#	else
#		define CORE_STATICLIB
#		define CORE_EXPORT
#	endif
#endif