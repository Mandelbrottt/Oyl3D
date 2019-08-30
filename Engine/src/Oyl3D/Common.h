#pragma once

#if defined (OYL_PLATFORM_WINDOWS)
#	if defined(OYL_ENGINE) && !defined(OYL_BUILD_DLL)
#		define OYL_BUILD_DLL
#	elif !defined(OYL_DLL)
#		define OYL_DLL
#	endif
#
#	if defined(OYL_BUILD_DLL) && defined(OYL_DLL)
#		error You must not define both OYL_BUILD_DLL and OYL_DLL at the same time!
#	endif
#
#	if defined(OYL_BUILD_DLL)
#		define OYLAPI __declspec(dllexport)	
#	elif defined(OYL_DLL)
#		define OYLAPI __declspec(dllimport)
#	else
#		define OYLAPI
#	endif
#else
#	error Oyl3D only supports Windows!
#endif

#define OYL_CALLBACK(func)   ::std::bind(&func, this)
#define OYL_CALLBACK_1(func) ::std::bind(&func, this, std::placeholders::_1)

#define OYL_CREATE_FUNC(type) static ::oyl::Ref<type> create() { return ::oyl::Ref<type>(new type()); }

#pragma warning(disable: 4002 4005)

#if defined(OYL_DEBUG)
#	if defined(_MSC_VER)
#		define OYL_BREAKPOINT ::__debugbreak()
#	else
#		define OYL_BREAKPOINT
#	endif
#else
#	define OYL_BREAKPOINT
#endif

#if defined(OYL_LOG_CONSOLE)
#	define OYL_ASSERT(expr, ...) { if(!(expr)) { OYL_LOG_ERROR("Assertion failed: {0}", __VA_ARGS__); OYL_BREAKPOINT; } }
#	define OYL_VERIFY(expr, ...) { if(!(expr)) { OYL_LOG_ERROR("Verification failed: {0}", __VA_ARGS__); OYL_BREAKPOINT; } }
#	define OYL_ASSERT(expr) { if(!(expr)) { OYL_BREAKPOINT; } }
#	define OYL_VERIFY(expr) { if(!(expr)) { OYL_BREAKPOINT; } }
#else
#	define OYL_ASSERT(expr, ...) 
#	define OYL_VERIFY(expr, ...) expr
#	define OYL_ASSERT(expr) 
#	define OYL_VERIFY(expr) expr
#endif

#pragma warning(default: 4005)
