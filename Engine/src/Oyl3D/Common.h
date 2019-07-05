#pragma once

#if defined (OYL_PLATFORM_WINDOWS)
#	if defined (OYL_BUILD_DLL)
#		if defined(OYL_ENGINE)
#			define OYL_API __declspec(dllexport)
#		else
#			define OYL_API __declspec(dllimport)
#		endif
#	else
#		define OYL_API
#	endif
#else
#	error Oyl3D only supports Windows!
#endif

#define BIND_CALLBACK_1(x) std::bind(&x, this, std::placeholders::_1)
#define BIND_CALLBACK(x) BIND_CALLBACK_1(x)

#if defined(OYL_DEBUG)
#	define ASSERT(x) { if(x) { __debugbreak(); } }
#	define VERIFY(x) ASSERT(x)
#else
#	define ASSERT(x) 
#	define VERIFY(x) x
#endif