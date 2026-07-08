#pragma once

#include "Core/Preprocessor/Library.h"
#include "Core/Preprocessor/Reflection.h"

#ifndef OYL_RENDERING_API
	#if defined(__REFLECT_GENERATE__)
		#define OYL_RENDERING_API _OYL_EXPAND(OYL_REFLECT)
	#elif defined(OYL_BUILD_STATIC_LIB)
		#define OYL_RENDERING_API
	#elif defined(OYL_BUILD_SHARED_LIB)
		#ifdef _INSIDE_OYL_RENDERING
			#define OYL_RENDERING_API _OYL_EXPORT
		#else
			#define OYL_RENDERING_API _OYL_IMPORT
		#endif
	#endif
#endif
