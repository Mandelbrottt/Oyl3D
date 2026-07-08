#pragma once

#include "Environment.h"
#include "Reflection.h"

#if defined(__REFLECT_GENERATE__)
	#define OYL_REFLECT __attribute__((annotate("__REFLECT__")))
#else
	#define OYL_REFLECT
#endif

#ifndef OYL_CORE_API
	#if defined(__REFLECT_GENERATE__)
		#define OYL_CORE_API _OYL_EXPAND(OYL_REFLECT)
	#elif defined(OYL_BUILD_STATIC_LIB)
		#define OYL_CORE_API
	#elif defined(OYL_BUILD_SHARED_LIB)
		#ifdef _INSIDE_OYL_CORE
			#define OYL_CORE_API _OYL_EXPORT
		#else
			#define OYL_CORE_API _OYL_IMPORT
		#endif
	#endif
#endif
