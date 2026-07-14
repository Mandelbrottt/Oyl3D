#pragma once

#include "Core/Preprocessor/Util.h"

#if !defined(OYL_DISTRIBUTION)
	#define OYL_ENABLE_ASSERTS
#endif

#define OYL_ASSERT(...) OYL_MACRO_OVERLOAD(_OYL_ASSERT, __VA_ARGS__)

#if defined(OYL_ENABLE_ASSERTS)
	#define _OYL_ASSERT_1(_expr_)             { if(!(_expr_)) { OYL_BREAKPOINT; } } _OYL_REQUIRE_SEMICOLON
	#define _OYL_ASSERT_2(_expr_, _str_)      { if(!(_expr_)) { OYL_LOG_ERROR("Assert Failed [" __FILE__ ":" _OYL_STRINGIFY_MACRO(__LINE__) "] (" #_expr_ "): " _str_); OYL_BREAKPOINT; } } _OYL_REQUIRE_SEMICOLON
	#define _OYL_ASSERT_3(_expr_, _str_, ...) { if(!(_expr_)) { OYL_LOG_ERROR("Assert Failed [" __FILE__ ":" _OYL_STRINGIFY_MACRO(__LINE__) "] (" #_expr_ "): " _str_, ##__VA_ARGS__); OYL_BREAKPOINT; } } _OYL_REQUIRE_SEMICOLON
	#define _OYL_ASSERT_4(_expr_, _str_, ...) _OYL_EXPAND(_OYL_ASSERT_3(_expr_, _str_, __VA_ARGS__))
	#define _OYL_ASSERT_5(_expr_, _str_, ...) _OYL_EXPAND(_OYL_ASSERT_3(_expr_, _str_, __VA_ARGS__))
	#define _OYL_ASSERT_6(_expr_, _str_, ...) _OYL_EXPAND(_OYL_ASSERT_3(_expr_, _str_, __VA_ARGS__))
	#define _OYL_ASSERT_7(_expr_, _str_, ...) _OYL_EXPAND(_OYL_ASSERT_3(_expr_, _str_, __VA_ARGS__))
	#define _OYL_ASSERT_8(_expr_, _str_, ...) _OYL_EXPAND(_OYL_ASSERT_3(_expr_, _str_, __VA_ARGS__))
	#define _OYL_ASSERT_9(_expr_, _str_, ...) _OYL_EXPAND(_OYL_ASSERT_3(_expr_, _str_, __VA_ARGS__))
#else
	#define _OYL_ASSERT_1(_expr_)             OYL_UNUSED(_expr_)
	#define _OYL_ASSERT_2(_expr_, _str_)      OYL_UNUSED(_expr_); OYL_UNUSED(_str_)
	#define _OYL_ASSERT_3(_expr_, _str_, ...) _OYL_EXPAND(_OYL_ASSERT_2(_expr_, _str_, __VA_ARGS__))
	#define _OYL_ASSERT_4(_expr_, _str_, ...) _OYL_EXPAND(_OYL_ASSERT_2(_expr_, _str_, __VA_ARGS__))
	#define _OYL_ASSERT_5(_expr_, _str_, ...) _OYL_EXPAND(_OYL_ASSERT_2(_expr_, _str_, __VA_ARGS__))
	#define _OYL_ASSERT_6(_expr_, _str_, ...) _OYL_EXPAND(_OYL_ASSERT_2(_expr_, _str_, __VA_ARGS__))
	#define _OYL_ASSERT_7(_expr_, _str_, ...) _OYL_EXPAND(_OYL_ASSERT_2(_expr_, _str_, __VA_ARGS__))
	#define _OYL_ASSERT_8(_expr_, _str_, ...) _OYL_EXPAND(_OYL_ASSERT_2(_expr_, _str_, __VA_ARGS__))
	#define _OYL_ASSERT_9(_expr_, _str_, ...) _OYL_EXPAND(_OYL_ASSERT_2(_expr_, _str_, __VA_ARGS__))
#endif