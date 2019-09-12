#pragma once

#if defined (OYL_PLATFORM_WINDOWS)
    #if defined(OYL_ENGINE) && !defined(OYL_BUILD_DLL)
        #define OYL_BUILD_DLL
    #elif !defined(OYL_DLL)
        #define OYL_DLL
    #endif

    #if defined(OYL_BUILD_DLL) && defined(OYL_DLL)
        #error You must not define both OYL_BUILD_DLL and OYL_DLL at the same time!
    #endif

    #if defined(OYL_BUILD_DLL)
        #define OYLAPI __declspec(dllexport)
    #elif defined(OYL_DLL)
        #define OYLAPI __declspec(dllimport)
    #else
        #define OYLAPI
    #endif
#else
    #error Oyl3D only supports Windows!
#endif

#define OYL_CALLBACK(func)   ::std::bind(&func, this)
#define OYL_CALLBACK_1(func) ::std::bind(&func, this, std::placeholders::_1)

#define OYL_CREATE_FUNC(type) static ::oyl::Ref<type> create() { return ::oyl::Ref<type>(new type()); }

#pragma warning(disable: 4002 4003)

#if defined(OYL_DEBUG)
    #define OYL_ENABLE_ASSERTS
    #if defined(_MSC_VER)
        #define OYL_BREAKPOINT ::__debugbreak()
    #else
        #define OYL_BREAKPOINT
    #endif
#else
    #define OYL_BREAKPOINT
#endif

#if defined(OYL_ENABLE_ASSERTS)
    #define _OYL_EXPAND(x) x
    #define _OYL_CAT(a, b) a##b
    #define _OYL_SELECT(name, num) _OYL_CAT(name##_, num)
    #define _OYL_GET_COUNT(_1, _2, _3, _4, _5, count, ...) count
    #define _OYL_VA_SIZE(...) _OYL_EXPAND(_OYL_GET_COUNT(__VA_ARGS__, 5, 4, 3, 2, 1))
    #define _OYL_VA_SELECT(name, ...) _OYL_EXPAND(_OYL_SELECT(name, _OYL_VA_SIZE(__VA_ARGS__))(__VA_ARGS__))

    #define OYL_ASSERT(...) _OYL_EXPAND(_OYL_VA_SELECT(_OYL_ASSERT, __VA_ARGS__))
    #define OYL_VERIFY(...) _OYL_EXPAND(_OYL_VA_SELECT(_OYL_VERIFY, __VA_ARGS__))

    #define _OYL_ASSERT_1(expr)           { if(!(expr)) { OYL_BREAKPOINT; } }
    #define _OYL_ASSERT_2(expr, str)      { if(!(expr)) { OYL_LOG_ERROR("Assertion failed: " str); OYL_BREAKPOINT; } }
    #define _OYL_ASSERT_3(expr, str, ...) { if(!(expr)) { OYL_LOG_ERROR("Assertion failed: " str, __VA_ARGS__); OYL_BREAKPOINT; } }
    #define _OYL_ASSERT_4(expr, str, ...) _OYL_ASSERT_3
    #define _OYL_ASSERT_5(expr, str, ...) _OYL_ASSERT_3

    #define _OYL_VERIFY_1(expr)           { if(!(expr)) { OYL_BREAKPOINT; } }
    #define _OYL_VERIFY_2(expr, str)      { if(!(expr)) { OYL_LOG_ERROR("Verification failed: " str); OYL_BREAKPOINT; } }
    #define _OYL_VERIFY_3(expr, str, ...) { if(!(expr)) { OYL_LOG_ERROR("Verification failed: " str, __VA_ARGS__); OYL_BREAKPOINT; } }
    #define _OYL_VERIFY_4(expr, str, ...) _OYL_VERIFY_3
    #define _OYL_VERIFY_5(expr, str, ...) _OYL_VERIFY_3
#else
    #define OYL_ASSERT(expr) 
    #define OYL_VERIFY(expr) expr
#endif

#pragma warning(default: 4005 4003)
