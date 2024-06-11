#pragma once

#pragma region Common Macros

#define OYL_DEPRECATED(_message_) [[deprecated(_message_)]]
#define OYL_UNUSED(_var_) static_cast<void>(_var_)

#pragma endregion

#pragma region Internal Macros

#define _OYL_EXPAND(_x_) _x_

#pragma endregion

#pragma region Shared Library
	#if defined(OYL_EDITOR)
		#define OYL_BUILD_SHARED_LIB
	#else
		#define OYL_BUILD_STATIC_LIB
	#endif

	#if defined(OYL_BUILD_STATIC_LIB)
		#define OYL_CORE_API
	#elif defined(OYL_BUILD_SHARED_LIB)
		#if defined(_MSC_VER) || defined(__MINGW32__) || defined(__MINGW64__)
			#define _OYL_CORE_EXPORT __declspec(dllexport)
			#define _OYL_CORE_IMPORT __declspec(dllimport)
		#else
			#define _OYL_CORE_EXPORT __attribute__((visibility("default")))
			#define _OYL_CORE_IMPORT __attribute__((visibility("default")))
		#endif
	
		#ifdef _INSIDE_OYL_CORE
			#define OYL_CORE_API _OYL_EXPAND(_OYL_CORE_EXPORT)
		#else
			#define OYL_CORE_API _OYL_EXPAND(_OYL_CORE_IMPORT)
		#endif
	#endif
#pragma endregion