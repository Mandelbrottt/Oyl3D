#pragma once

#pragma region Platform Detection
    #if defined(_WIN32)
        #if defined(_WIN64)
            #define OYL_PLATFORM_WINDOWS
        #else
            #error x86 builds are not supported!
        #endif
    #elif defined(__APPLE__) || defined(__MACH__)
        #include <TargetConditionals.h>
        #if TARGET_IPHONE_SIMULATOR == 1
            #error "IOS simulator is not supported!"
        #elif TARGET_OS_IPHONE == 1
            #define OYL_PLATFORM_IOS
            #error "IOS is not supported!"
        #elif TARGET_OS_MAC == 1
            #define OYL_PLATFORM_MACOS
            #error "MacOS is not supported!"
        #else
            #error "Unknown Apple platform!"
        #endif
    #elif defined(__ANDROID__)
        #define OYL_PLATFORM_ANDROID
        #error "Android is not supported!"
    #elif defined(__linux__)
        #define OYL_PLATFORM_LINUX
        #error "Linux is not supported!"
    #else
        #error "Unknown platform!"
    #endif
#pragma endregion

#pragma region Shared Library Building
    #if defined (OYL_PLATFORM_WINDOWS)
        #if defined(OYL_SHARED_LIB)
            #if defined(OYL_ENGINE) && !defined(OYL_BUILD_DLL)
                #define OYL_BUILD_DLL
            #elif !defined(OYL_DLL)
                #define OYL_DLL
            #endif
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
#pragma endregion

#pragma region Macro Argument Overloading
    #define _OYL_EXPAND(x) x
    #define _OYL_CAT(a, b) a##b
    #define _OYL_SELECT(name, num) _OYL_CAT(name##_, num)
    #define _OYL_GET_COUNT(_1, _2, _3, _4, _5, count, ...) count
    #define _OYL_VA_SIZE(...) _OYL_EXPAND(_OYL_GET_COUNT(__VA_ARGS__, 5, 4, 3, 2, 1))
    #define _OYL_VA_SELECT(name, ...) _OYL_EXPAND(_OYL_SELECT(name, _OYL_VA_SIZE(__VA_ARGS__))(__VA_ARGS__))
    #define _OYL_MACRO_OVERLOAD(name, ...) _OYL_EXPAND(_OYL_VA_SELECT(name, __VA_ARGS__))
#pragma endregion

#pragma region Delegate Callbacks
    #define OYL_CALLBACK(...)   _OYL_MACRO_OVERLOAD(_OYL_CALLBACK_, __VA_ARGS__)
    #define OYL_CALLBACK_1(...) _OYL_MACRO_OVERLOAD(_OYL_CALLBACK_1, __VA_ARGS__)
    #define OYL_CALLBACK_2(...) _OYL_MACRO_OVERLOAD(_OYL_CALLBACK_2, __VA_ARGS__)
    #define OYL_CALLBACK_3(...) _OYL_MACRO_OVERLOAD(_OYL_CALLBACK_3, __VA_ARGS__)
    
    #define _OYL_CALLBACK__2(func, ptr) ::std::bind(&func, ptr)
    #define _OYL_CALLBACK__1(func)      _OYL_CALLBACK__2(func, this)
            
    #define _OYL_CALLBACK_1_2(func, ptr) ::std::bind(&func, ptr, std::placeholders::_1)
    #define _OYL_CALLBACK_1_1(func)      _OYL_CALLBACK_1_2(func, this)
            
    #define _OYL_CALLBACK_2_2(func, ptr) ::std::bind(&func, ptr, std::placeholders::_1, \
                                                                 std::placeholders::_2)
    #define _OYL_CALLBACK_2_1(func)      _OYL_CALLBACK_2_2(func, this)
            
    #define _OYL_CALLBACK_3_2(func, ptr) ::std::bind(&func, ptr, std::placeholders::_1, \
                                                                 std::placeholders::_2, \
                                                                 std::placeholders::_3)
    #define _OYL_CALLBACK_3_1(func)      _OYL_CALLBACK_3_2(func, this)
#pragma endregion

#pragma region Necessary Per-Class Override Functions
    #define OYL_CTOR(type, base)                                                         \
        struct _##type {};                                                               \
    public:                                                                              \
        static ::oyl::Ref<type> create() { return ::oyl::Ref<type>::create(_##type{}); } \
        explicit type(_##type) : Node(#type) {}                                          \
        virtual ~##type() {}

    #define OYL_CONSTRUCTOR(type, base) OYL_CTOR(type, base)
#pragma endregion

#pragma region Debug Macros
    #pragma warning(disable: 4002 4003)
    
    #if !defined(OYL_DISTRIBUTION)
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
        #define OYL_ASSERT(...) _OYL_MACRO_OVERLOAD(_OYL_ASSERT, __VA_ARGS__)
        #define OYL_VERIFY(...) _OYL_MACRO_OVERLOAD(_OYL_ASSERT, __VA_ARGS__)
    
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
#pragma endregion

#pragma region Attributes
    #define OYL_DEPRECATED(reason) [[deprecated(reason)]]
#pragma endregion