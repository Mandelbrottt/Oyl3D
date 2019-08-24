#pragma once

#if defined (OYL_PLATFORM_WINDOWS)

#else
#	error Oyl3D only supports Windows!
#endif

#define BIND_CALLBACK_1(x) std::bind(&x, this, std::placeholders::_1)
#define BIND_CALLBACK(x) BIND_CALLBACK_1(x)

#pragma warning(disable: 4002 4005)
#if defined(OYL_DEBUG)
#	if defined(_MSC_VER)
#		define BREAKPOINT __debugbreak()
#	endif
#	define ASSERT(x, ...) { if(!(x)) { LOG_ERROR("Assertion failed: {0}", __VA_ARGS__); BREAKPOINT; } }
#	define VERIFY(x, ...) { if(!(x)) { LOG_ERROR("Verification failed: {0}", __VA_ARGS__); BREAKPOINT; } }
#	define ASSERT(x) { if(!(x)) { BREAKPOINT; } }
#	define VERIFY(x) { if(!(x)) { BREAKPOINT; } }
#else
#	define BREAKPOINT
#	define ASSERT(x, ...) 
#	define VERIFY(x, ...) x
#endif
#pragma warning(default: 4005)
