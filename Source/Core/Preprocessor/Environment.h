#pragma once

#if defined(OYL_DISTRIBUTION) && defined(OYL_EDITOR)
	#error Oyl Editor doesn't support Distribution configuration!
#endif

#if defined(OYL_EDITOR)
	#define OYL_BUILD_SHARED_LIB 1
#else
	#define OYL_BUILD_STATIC_LIB 1
#endif

#if defined(_WIN32)
	#define OYL_WINDOWS 1
	#ifdef _WIN64
		#define OYL_WIN64 1
	#else
		#define OYL_WIN32 1
	#endif
	#define WIN32_LEAN_AND_MEAN
	#define NOMINMAX
#else
	#warning "Oyl3D only supports windows!"
#endif

#if !defined(_OYL_EXPORT)
	#if defined(_MSC_VER) || defined(__MINGW32__) || defined(__MINGW64__)
		#define _OYL_EXPORT __declspec(dllexport)
		#define _OYL_IMPORT __declspec(dllimport)
	#else
		#define _OYL_EXPORT __attribute__((visibility("default")))
		#define _OYL_IMPORT __attribute__((visibility("default")))
	#endif
#endif