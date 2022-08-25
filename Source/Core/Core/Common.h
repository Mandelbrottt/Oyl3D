#pragma once

#if _INSIDE_REARM_CORE
	#define REARM_CORE_EXPORT __declspec(dllexport)
#else
	#define REARM_CORE_EXPORT __declspec(dllimport)
#endif

#pragma region Macro Argument Overloading
    #define _CLEAR(x)
    #define _EXPAND(x) x
    #define _CAT(a, b) a##b
    #define _SELECT(name, num) _CAT(name##_, num)
    #define _GET_COUNT(_1, _2, _3, _4, _5, count, ...) count
    #define _VA_SIZE(...) _EXPAND(_GET_COUNT(__VA_ARGS__, 5, 4, 3, 2, 1))
    #define _VA_SELECT(name, ...) _EXPAND(_SELECT(name, _VA_SIZE(__VA_ARGS__))(__VA_ARGS__))
    #define _MACRO_OVERLOAD(name, ...) _EXPAND(_VA_SELECT(name, __VA_ARGS__))
#pragma endregion

#pragma region Macro Helpers
	#define _MACRO_REQUIRE_SEMICOLON  static_assert(true)

	// Some auto formatters don't like access specifiers being the first line in a macro
	#define _MACRO_AUTO_FORMAT_INDENT static_assert(true);
#pragma endregion